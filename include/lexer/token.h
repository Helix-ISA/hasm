#ifndef HX_TOKEN_H
#define HX_TOKEN_H

#include "types.h"

typedef enum {
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

	TOKEN_EOF,
	TOKEN_UNKNOWN
} hx_token_type;

typedef struct {
	hx_token_type type;

	const char *text;
	u32 text_length;

	u32 line;
	u32 column;
} hx_token;

const char *token_type_name(hx_token_type type);

#endif
