#include "hasm/hasm.h"

#include <stdio.h>
#include <stdlib.h>

#include "cli/cli_parser.h"
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

static const cli_arg args[] = {
	{
		.sname = "v",
		.lname = "verbose",
		.arg_type = CLI_ARG_NONE,
		.description = "Enable verbose mode"
	},
	{
		.sname = "o",
		.lname = "output",
		.arg_type = CLI_ARG_REQUIRED,
		.value_type = "FILE",
		.default_value = "out",
		.description = "Specifies output file",
	},
	{
		.sname = "d",
		.lname = "disassemble",
		.arg_type = CLI_ARG_REQUIRED,
		.value_type = "FILE",
		.description = "Specify file to disassemble",
	},
	{
		.lname = "lexer-debug",
		.arg_type = CLI_ARG_NONE,
		.description = "Enable lexer debug output",
	},
	{
		.lname = "parser-debug",
		.arg_type = CLI_ARG_NONE,
		.description = "Enable parser debug output",
	},
	{
		.lname = "symbol-debug",
		.arg_type = CLI_ARG_NONE,
		.description = "Enable symbol debug output",
	},
};

int hasm(int argc, char **argv)
{
	u8 status = 0;

	const cli_config config = {
		.program_name = argv[0],
		.version = "0.1.0",
		.description = "Helix assembler",
		.args = args,
		.arg_count = sizeof(args) / sizeof(args[0])
	};

	cli_result result;
	if (!cli_parse(&config, argc, argv, &result)) {
		cli_print_error(&config, result.error);
		status = 1;
		goto free_cli;
	}

	/* Disassemble */
	const cli_parsed_arg *disassemble = cli_get_arg(&result, "disassemble");
	if (disassemble) {
		decoded_disassemble(disassemble->value);
		goto free_cli;
	}

	/* Assemble */
	if (result.positions.count < 1) {
		error("expected input files");
		status = 1;
		goto free_cli;
	} else if (result.positions.count > 1) {
		error("currently only supports a single input file");
		status = 1;
		goto free_cli;
	}

	/* Source bytes */
	u32 source_length = 0;
	FILE *in = fopen(result.positions.values[0], "rb");
	char *source = read_file(in, &source_length);

	if (source == NULL) {
		status = 1;
		goto free_cli;
	}

	/* Tokenize the source */
	u32 token_count = 0;
	hx_token *tokens = lexer_tokenize(
			source,
			source_length,
			&token_count
			);

	if (tokens == NULL) {
		fprintf(stderr, "lexer_tokenize failed\n");
		status = 1;
		goto cleanup_source;
	}

	/* Prepare parser and nodes */
	hx_parser parser;
	parser_init(&parser, tokens, token_count);

	hx_program program;
	program_init(&program);

	/* Parse the tokens */
	if (!parser_parse(&parser, &program)) {
		status = 1;
		goto cleanup_program;
	}

	/* Parse complete open write file */
	const cli_parsed_arg *output = cli_get_arg(&result, "output");
	FILE *out;
	if (output) {
		out = fopen(output->value, "wb");
		if (out == NULL) {
			perror("fopen");
			status = 1;
			goto cleanup_program;
		}
	}

	/* Encode the nodes into binary */
	if (!encoder_init(&program)) {
		fprintf(stderr, "failed to encode nodes\n");
		status = 1;
		goto cleanup_encoder;
	}

	hx_binary binary = {0};
	encoder_encode(&program, &binary, out);

	for (u32 i = 0; i < binary.size; i++) {
		fwrite(&binary.data[i], sizeof(u8), 1, out);
	}

	/* Free encoder */
	if (!encoder_free(&program)) {
		fprintf(stderr, "failed to free encoder\n");
		status = 1;
		goto cleanup_encoder;
	}
		goto free_cli;

cleanup_encoder:
	fclose(out);

cleanup_program:
	if (!program_free(&program)) {
		fprintf(stderr, "failed to free program");

		if (status == 0)
			status = 1;
	}

cleanup_source:
	free(source);
free_cli:
	cli_result_free(&result);

	return status;
}
