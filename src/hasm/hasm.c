#include "hasm/hasm.h"
#include "codegen/codegen.h"
#include "disassembler/disassembler.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser/ast.h"
#include "parser/parser.h"

#include <flagparser/flagparser.h>
#include <isac/instruction.h>
#include <stdio.h>
#include <stdlib.h>


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

static b8 extract_source(FILE *file, char **source, u32 *source_length)
{
	if (fseek(file, 0, SEEK_END) != 0) {
		perror("fseek");
		return failure;
	}

	long file_size = ftell(file);

	if (file_size < 0) {
		perror("ftell");
		return failure;
	}

	rewind(file);

	char *out = malloc((u32)file_size + 1);
	if (out == NULL) {
		fprintf(stderr, "out of memory\n");
		return failure;
	}

	u32 out_length = fread(out, 1, (u32)file_size, file);
	if (out_length != (u32)file_size) {
		fprintf(stderr, "failed to read file\n");
		return failure;
	}

	out[out_length] = '\0';

	*source = out;
	*source_length = out_length;

	return success;
}

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
	u32 source_length = 0;
	b8 lexer_success = false;
	b8 parser_success = false;
	b8 ast_success = false;
	b8 codegen_success = false;

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
	
	in = fopen(result.positions.values[0], "rb");
	if (in == NULL) {
		perror(result.positions.values[0]);
		status = 1;
		goto cleanup;
	}

	const fp_parsed_flag *disassemble = fp_get_flag(&result, "disassemble");
	if (disassemble) {
		disassembler_disassemble(in);
		return 0;
	}

	/*----------------------- ASSEMBLER START -----------------------*/
	if (!extract_source(in, &source, &source_length)) {
		fprintf(stderr, "failed to extract source\n");
		status = 1;
		goto cleanup;
	}

	hx_lexer lexer = {0};
	if (!lexer_init(&lexer, source, source_length)) {
		fprintf(stderr, "failed to initialize lexer\n");
		status = 1;
		goto cleanup;
	}
	lexer_success = true;

	hx_ast ast = {0};
	ast_init(&ast);

	hx_parser parser = {0};
	if (!parser_init(&parser, &lexer, &ast)) {
		fprintf(stderr, "failed to initialize parser\n");
		status = 1;
		goto cleanup;
	}
	parser_success = true;

	if (!parser_parse(&parser)) {
		fprintf(stderr, "failed to parse\n");
		status = 1;
		goto cleanup;
	}
	ast_success = true;

	if (!ast_resolve_symbols(&ast)) {
		fprintf(stderr, "failed to resolve symbols\n");
		status = 1;
		goto cleanup;
	}

	if (!ast_optimize(&ast)) {
		fprintf(stderr, "failed to optimize\n");
		status = 1;
		goto cleanup;
	}

	hx_codegen codegen = {0};
	if (!codegen_init(&codegen, &ast)) {
		fprintf(stderr, "failed to initialize codegen\n");
		status = 1;
		goto cleanup;
	}
	codegen_success = true;

	if (!codegen_generate(&codegen)) {
		fprintf(stderr, "failed to generate codegen\n");
		status = 1;
		goto cleanup;
	}
	
	/* Open output file */
	const fp_parsed_flag *out_file_flag = fp_get_flag(&result, "output");
	out = fopen(out_file_flag->value, "wb");
	if (out == NULL) {
		perror(out_file_flag->value);
		status = 1;
		goto cleanup;
	}

	if (!codegen_write(&codegen, out)) {
		fprintf(stderr, "failed to write codegen\n");
		status = 1;
		goto cleanup;
	}

cleanup:

	if (codegen_success)
		codegen_free(&codegen);

	if (ast_success)
		ast_free(&ast);

	if (parser_success)
		parser_free(&parser);

	if (lexer_success)
		lexer_free(&lexer);

	if (out != NULL)
		fclose(out);

	if (in != NULL)
		fclose(in);

	free(source);

	fp_result_free(&result);

	return status;
}
