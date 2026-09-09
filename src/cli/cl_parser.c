#include "cli/cl_parser.h"
#include "types.h"
#include <stdio.h>
#include <string.h>

b8 parse_args(s32 count, char **args, hx_cli *cli)
{
	cli->lexer_debug = false;
	cli->parser_debug = false;
	cli->input_file = NULL;
	cli->output_file = NULL;

	for (s32 i = 1; i < count; i++) {
		if (strcmp(args[i], "--lexer-debug") == 0) {
			cli->lexer_debug = true;
		} else if (strcmp(args[i], "--parser-debug") == 0) {
			cli->parser_debug = true;
		} else if (strcmp(args[i], "-o") == 0) {
			if (i + 1 == count) {
				fprintf(stderr, "expected output after \"-o\"\n");
				return failure;
			}

			i++;

			cli->output_file = fopen(args[i], "wb");

			if (cli->output_file == NULL) {
				perror("fopen");
				return failure;
			}
		} else {
			cli->input_file = fopen(args[i], "rb");

			if (cli->input_file == NULL) {
				perror("fopen");
				return failure;
			}
		}
	}

	if (cli->output_file == NULL) {
		cli->output_file = fopen("out.hxo", "wb");
	}

	return success;
}
