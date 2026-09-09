#ifndef HX_PARSER_H
#define HX_PARSER_H

#include "isa/program.h"
#include "lexer/token.h"
#include "types.h"

typedef struct {
	hx_token *tokens;
	u32 token_count;

	u32 position;
} hx_parser;

b8 parser_init(hx_parser *parser, hx_token *tokens, u32 token_count);
b8 parser_parse(hx_parser *parser, hx_program *program);

#endif
