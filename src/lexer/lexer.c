#include "lexer/lexer.h"

#include "lexer/token.h"
#include "types.h"

#include <ctype.h>
#include <flagparser/types.h>
#include <stdio.h>
#include <stdlib.h>

static int lexer_at_end(const hx_lexer *lexer)
{
	return lexer->position >= lexer->source_length;
}

static char lexer_current(const hx_lexer *lexer)
{
	if (lexer_at_end(lexer))
		return '\0';

	return lexer->source[lexer->position];
}

static char lexer_peek(const hx_lexer *lexer)
{
	if (lexer->position + 1 >= lexer->source_length)
		return '\0';

	 return lexer->source[lexer->position + 1];
}

static void lexer_advance(hx_lexer *lexer)
{
	if (lexer_at_end(lexer))
		return;

	char c = lexer->source[lexer->position];

	lexer->position++;

	if (c == '\n') {
		lexer->line++;
		lexer->column = 1;
	} else {
		lexer->column++;
	}
}

b8 lexer_init(hx_lexer *lexer, const char *source, u32 source_length)
{
	if (lexer == NULL)
		return failure;

	lexer->source = source;
	lexer->source_length = source_length;

	lexer->position = 0;

	lexer->line = 0;
	lexer->column = 1;

	return success;
}

b8 lexer_free(hx_lexer *lexer)
{
	if (lexer == NULL)
		return failure;

	lexer->source = NULL;

	return success;
}

static void skip_whitespace(hx_lexer *lexer)
{
	for (;;) {
		char c = lexer_current(lexer);

		if (c == ' ' || c == '\t' || c == '\r') {
			lexer_advance(lexer);
			continue;
		}
		
		if (c == '#') {
			while (!lexer_at_end(lexer) && lexer_current(lexer) != '\n') {

				lexer_advance(lexer);
			}

			continue;
		}

		break;
	}
}

static hx_token token_create(const hx_lexer *lexer, hx_token_type type, u32 start, u32 line, u32 column)
{
	hx_token token;

	token.type = type;
	token.lexme = lexer->source + start;
	token.lexme_length = lexer->position - start;

	token.location.line = line;
	token.location.column = column;

	return token;
}

static b8 is_identifier_start(char c)
{
	return isalpha((u8)c) || c == '_';
}

static b8 is_identifier_char(char c)
{
	return isalnum((u8)c) || c == '_';
}

static b8 is_number_start(char c)
{
	return isdigit((u8)c) || c == '+' || c == '-';
}

static hx_token lex_identifier(hx_lexer *lexer)
{
	u32 start = lexer->position;
	u32 line = lexer->line;
	u32 column = lexer->column;

	lexer_advance(lexer);

	while (!lexer_at_end(lexer) && is_identifier_char(lexer_current(lexer))) {
		lexer_advance(lexer);
	}

	return token_create(lexer, TOKEN_IDENTIFIER, start, line, column);
}

static hx_token lex_number(hx_lexer *lexer)
{
	u32 start = lexer->position;
	u32 line = lexer->line;
	u32 column = lexer->column;

	if (lexer_current(lexer) == '+' || lexer_current(lexer) == '-')
		lexer_advance(lexer);

	/* Base 16 */
	if (lexer_current(lexer) == '0' &&
			(lexer_peek(lexer) == 'x' || lexer_peek(lexer) == 'X')) {
		lexer_advance(lexer);
		lexer_advance(lexer);

		while (!lexer_at_end(lexer) && isxdigit((u8)lexer_current(lexer))) {
			lexer_advance(lexer);
		}

		return token_create(lexer, TOKEN_NUMBER, start, line, column);
	}

	while(!lexer_at_end(lexer) && isdigit((u8)lexer_current(lexer))) {
		lexer_advance(lexer);
	}

	/* Base 10 */
	if ((lexer_current(lexer) == '.' && isdigit((u8)lexer_peek(lexer)))) {
		lexer_advance(lexer);

		while (!lexer_at_end(lexer) && isdigit((u8)lexer_current(lexer)))
			lexer_advance(lexer);
	}

	return token_create(lexer, TOKEN_NUMBER, start, line, column);
}

hx_token lexer_next(hx_lexer *lexer)
{
	skip_whitespace(lexer);

	u32 start = lexer->position;
	u32 line = lexer->line;
	u32 column = lexer->column;

	if (lexer_at_end(lexer))
		return token_create(lexer, TOKEN_EOF, start, line, column);

	char test_char = lexer_current(lexer);

	if (is_identifier_start(test_char))
		return lex_identifier(lexer);

	if (is_number_start(test_char))
		return lex_number(lexer);

	switch (test_char) {
		case '[':
			lexer_advance(lexer);

			return token_create(lexer, TOKEN_LBRACKET, start, line, column);
		case ']':
			lexer_advance(lexer);

			return token_create(lexer, TOKEN_RBRACKET, start, line, column);
		case '(':
			lexer_advance(lexer);

			return token_create(lexer, TOKEN_LPAREN, start, line, column);
		case ')':
			lexer_advance(lexer);

			return token_create(lexer, TOKEN_RPAREN, start, line, column);
		case ',':
			lexer_advance(lexer);

			return token_create(lexer, TOKEN_COMMA, start, line, column);
		case ':':
			lexer_advance(lexer);

			return token_create(lexer, TOKEN_COLON, start, line, column);
		case '.':
			lexer_advance(lexer);

			return token_create(lexer, TOKEN_DOT, start, line, column);
		case '+':
			lexer_advance(lexer);

			return token_create(lexer, TOKEN_PLUS, start, line, column);
		case '-':
			lexer_advance(lexer);

			return token_create(lexer, TOKEN_MINUS, start, line, column);
		case '\n':
			lexer_advance(lexer);

			return token_create(lexer, TOKEN_NEWLINE, start, line, column);
		default:
			lexer_advance(lexer);

			return token_create(lexer, TOKEN_UNKNOWN, start, line, column);
	}
}
