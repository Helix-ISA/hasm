#include "cli/cl_parser.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		return 1;
	}

	hx_cli cli;

	parse_args(argc, argv, &cli);

	if (fseek(cli.input_file, 0, SEEK_END) != 0) {
		perror("fseek");
		fclose(cli.input_file);
		return 1;
	}

	u64 file_size = ftell(cli.input_file);

	if (file_size < 0) {
		perror("ftell");
		fclose(cli.input_file);
		return 1;
	}

	rewind(cli.input_file);

	char *source = malloc((u32)file_size + 1);

	u32 bytes_read = fread(source, 1, file_size, cli.input_file);

	fclose(cli.input_file);

	if (bytes_read != file_size) {
		fprintf(stderr, "failed to read file\n");
		free(source);
		return 1;
	}

	source[file_size] = '\0';

	hx_lexer lexer;

	lexer_init(&lexer, source, bytes_read);

	if (cli.lexer_debug) {
		for (;;) {
			hx_token token = lexer_next(&lexer);

			print_token(token);

			if (token.type == TOKEN_EOF)
				break;

		}
	}

	free(source);

	return 0;

}
