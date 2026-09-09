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

	u32 token_count;
	hx_token *tokens = lexer_tokenize(source, source_length, &token_count);
	if (tokens == NULL) {
		fprintf(stderr, "lexer_tokenize failed\n");
		free(source);
		return 1;
	}

	if (cli.lexer_debug) {
		for (u32 i = 0; i < token_count; i++) {
			print_token(tokens[i]);
		}
		return 0;
	}



	free(tokens);
	free(source);
	return 0;
}
