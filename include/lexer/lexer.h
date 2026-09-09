#ifndef HX_LEXER_H
#define HX_LEXER_H

#include "lexer/token.h"
#include "types.h"
typedef struct {
	const char *source;
	u32 source_length;

	u32 position;
	u32 line;
	u32 column;
} hx_lexer;

b8 lexer_init(hx_lexer *lexer, const char *source, u32 source_length);

hx_token *lexer_tokenize(const char *source, u32 source_length, u32 *token_count);

#endif
