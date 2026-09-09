#ifndef HX_CL_PARSER
#define HX_CL_PARSER

#include "types.h"
#include <stdio.h>

typedef struct {
	b8 lexer_debug;
	b8 parser_debug;

	FILE *input_file;
	FILE *output_file;
} hx_cli;

b8 cl_parse_init(hx_cli *cli);
b8 cl_parse_free(hx_cli *cli);

b8 cl_parse_args(s32 count, char **args, hx_cli *cli);

#endif
