#include "hasm/hasm.h"

#include <stdio.h>
#include <stdlib.h>

#include "cli/cl_parser.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "io/file.h"

static void print_token(hx_token token)
{
	printf(
		"%-10s  line=%u  column=%u  text=\"%.*s\"\n",
		token_type_name(token.type),
		token.line,
		token.column,
		(int)token.text_length,
		token.text
	);
}

static void lexer_debug(const char *source, u32 source_length)
{
	hx_lexer lexer;

	lexer_init(&lexer, source, source_length);

	for (;;) {
		hx_token token = lexer_next(&lexer);

		print_token(token);

		if (token.type == TOKEN_EOF)
			break;
	}
}

int hasm(int argc, char** argv)
{
	hx_cli cli;

	if (parse_args(argc, argv, &cli) == 0)
		return 1;

	u32 source_length = 0;

	char *source = read_file(cli.input_file, &source_length);

	fclose(cli.input_file);

	if (source == NULL)
		return 1;

	if (cli.lexer_debug)
		lexer_debug(source, source_length);

	free(source);

	return 0;
}
