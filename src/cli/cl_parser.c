#include "cli/cl_parser.h"
#include "types.h"
#include <stdio.h>
#include <string.h>

b8 parse_args(s32 count, char **args, hx_cli *cli)
{
	cli->lexer_debug = false;

	for (s32 i = 1; i < count; i++) {
		if (strcmp(args[i], "--lexer-debug") == 0) {
			cli->lexer_debug = true;
		} else {
			cli->input_file = fopen(args[i], "rb");

			if (cli->input_file == NULL) {
				perror("fopen");
				return failure;
			}
		}
	}

	return success;
}
