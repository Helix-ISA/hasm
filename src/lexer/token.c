#include "lexer/token.h"

const char *token_type_name(hx_token_type type)
{
	switch (type) {
		case TOKEN_IDENTIFIER: return "identifier";
		case TOKEN_NUMBER: return "number";
		case TOKEN_LBRACKET: return "lbracket";
		case TOKEN_RBRACKET: return "rbracket";
		case TOKEN_LPAREN: return "lparen";
		case TOKEN_RPAREN: return "rparen";
		case TOKEN_COMMA: return "comma";
		case TOKEN_COLON: return "colon";
		case TOKEN_DOT: return "dot";
		case TOKEN_PLUS: return "plus";
		case TOKEN_MINUS: return "minus";
		case TOKEN_NEWLINE: return "newline";
		case TOKEN_EOF: return "eof";
		case TOKEN_UNKNOWN: return "unknown";
		default: return "unknown";
	}
}
