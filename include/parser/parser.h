#ifndef HX_PARSER_H
#define HX_PARSER_H

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser/ast.h"
#include "types.h"

typedef struct {
	hx_lexer *lexer;

	hx_token peek;
	hx_token current;
	hx_token previous;

	hx_ast *ast;

	b8 has_current;
	b8 had_error;
	u64 instruction_address;
} hx_parser;

b8 parser_init(hx_parser *parser, hx_lexer *lexer, hx_ast *ast);
b8 parser_free(hx_parser *parser);

b8 parser_parse(hx_parser *parser);

#endif
