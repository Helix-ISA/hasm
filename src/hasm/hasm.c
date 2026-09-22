#include "hasm/hasm.h"

#include <stdio.h>
#include <stdlib.h>
#include <flagparser/flagparser.h>

#include "encoder/encoder.h"
#include "decoder/decoder.h"
#include "isa/program.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "io/file.h"
#include "parser/parser.h"
#include "types.h"

static void error(char *message) {
	fprintf(stderr, "hasm: %s\n", message);
}

static const fp_flag flags[] = {
	{
		.sname = "h",
		.lname = "help",
		.flag_type = FLAG_ARG_NONE,
		.hidden = true
	},
	{
		.sname = "v",
		.lname = "verbose",
		.flag_type = FLAG_ARG_NONE,
		.description = "Enable verbose mode"
	},
	{
		.sname = "o",
		.lname = "output",
		.flag_type = FLAG_ARG_REQUIRED,
		.value_type = "FILE",
		.default_value = "out",
		.description = "Specifies output file",
	},
	{
		.sname = "d",
		.lname = "disassemble",
		.flag_type = FLAG_ARG_REQUIRED,
		.value_type = "FILE",
		.description = "Specify file to disassemble",
	},
	{
		.lname = "lexer-debug",
		.flag_type = FLAG_ARG_NONE,
		.description = "Enable lexer debug output",
	},
	{
		.lname = "parser-debug",
		.flag_type = FLAG_ARG_NONE,
		.description = "Enable parser debug output",
	},
	{
		.lname = "symbol-debug",
		.flag_type = FLAG_ARG_NONE,
		.description = "Enable symbol debug output",
	},
};

int hasm(int argc, char **argv)
{
	u8 status = 0;

	const fp_config config = {
		.program_name = argv[0],
		.version = "0.1.0",
		.description = "Helix assembler",
		.flags = flags,
		.flag_count = sizeof(flags) / sizeof(flags[0])
	};

	fp_result result = {0};

	FILE *in = NULL;
	FILE *out = NULL;
	char *source = NULL;
	hx_token *tokens = NULL;

	hx_parser parser;
	hx_program program;

	b8 program_initialized = false;
	b8 encoder_initialized = false;

	/* Pares flags */
	if (!fp_flag_parse(&config, argc, argv, &result)) {
		fp_print_error(&config, result.error);
		status = 1;
		goto cleanup;
	}

	/* Help */
	const fp_parsed_flag *help = fp_get_flag(&result, "help");
	if (help) {
		fp_print_usage(&config);
		goto cleanup;
	}
	
	/* Disassemble */
	const fp_parsed_flag *disassemble = fp_get_flag(&result, "disassemble");
	if (disassemble) {
		decoded_disassemble(disassemble->value);
		goto cleanup;
	}

	/* Assemble */
	if (result.positions.count < 1) {
		error("expected input files");
		status = 1;
		goto cleanup;
	} else if (result.positions.count > 1) {
		error("currently only supports a single input file");
		status = 1;
		goto cleanup;
	}

	/* Input validation */
	if (result.positions.count == 0) {
		error("expected input file");
		status = 1;
		goto cleanup;
	}
	
	if (result.positions.count > 0) {
		error("currently only supports a single input file");
		status = 1;
		goto cleanup;
	}

	/* Output input file */
	in = fopen(result.positions.values[0], "rb");
	if (in == NULL) {
		perror(result.positions.values[0]);
		status = 1;
		goto cleanup;
	}

	/* Read source */
	u32 source_length = 0;
	source = read_file(in, &source_length);

	fclose(in);
	in = NULL;

	if (source == NULL) {
		status = 1;
		goto cleanup;
	}

	/* Tokenize */
	u32 token_count = 0;
	tokens = lexer_tokenize(source, source_length, &token_count);
	if (tokens == NULL) {
		error("lexer_tokenize failed");
		status = 1;
		goto cleanup;
	}

	/* Initialize parser */
	parser_init(&parser, tokens, token_count);

	program_init(&program);
	program_initialized = true;

	if (!parser_parse(&parser, &program)) {
		status = 1;
		goto cleanup;
	}

	/* Open output file */
	const fp_parsed_flag *output = fp_get_flag(&result, "output");
	out = fopen(output->value, "wb");
	if (out == NULL) {
		perror(output->value);
		status = 1;
		goto cleanup;
	}

	if (!encoder_init(&program)) {
		error("failed to initialize encoder");
		status = 1;
		goto cleanup;
	}

	encoder_initialized = true;

	hx_binary binary = {0};
	encoder_encode(&program, &binary);

	if (binary.data == NULL && binary.size != 0) {
		error("encoder produced invalid binary");
		status = 1;
		goto cleanup;
	}

	if (binary.size > 0) {
		u32 written = fwrite(binary.data, sizeof(*binary.data), binary.size, out);

		if (written != binary.size) {
			perror("fwrite");
			status = 1;
			goto cleanup;
		}
	}

cleanup:
	if (encoder_initialized) {
		if (!encoder_free(&program)) {
			error("failed to free encoder");
			status = 1;
		}
	}

	if (program_initialized) {
		if (!program_free(&program)) {
			error("failed to free program");
			status = 1;
		}
	}

	if (out != NULL)
		fclose(out);

	if (in != NULL)
		fclose(in);

	free(tokens);
	free(source);

	fp_result_free(&result);

	return status;
}
