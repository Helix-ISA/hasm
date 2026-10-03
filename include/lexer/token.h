#ifndef HX_TOKEN_H
#define HX_TOKEN_H

#include "types.h"

typedef enum {
	TOKEN_EOF = 0,

	TOKEN_IDENTIFIER,
	TOKEN_NUMBER,

	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_LPAREN,
	TOKEN_RPAREN,

	TOKEN_COMMA,
	TOKEN_COLON,
	TOKEN_DOT,

	TOKEN_PLUS,
	TOKEN_MINUS,

	TOKEN_NEWLINE,

	TOKEN_UNKNOWN
} hx_token_type;

typedef struct {
	u32 line;
	u32 column;
} hx_source_location;

typedef struct {
	hx_token_type type;

	const char *lexme;
	u32 lexme_length;

	hx_source_location location;
} hx_token;

b8 token_equals(const hx_token *token, const char *name);
const char *token_type_name(hx_token_type type);

#endif
