#include "parser/parser.h"

#include "isa/instruction.h"
#include "isa/label.h"
#include "isa/mnemonic.h"
#include "isa/node.h"
#include "isa/operand.h"
#include "isa/program.h"
#include "isa/register.h"
#include "lexer/token.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static hx_token *parser_current(hx_parser *parser)
{
	if (parser->position >= parser->token_count)
		return NULL;

	return &parser->tokens[parser->position];
}

static void parser_advance(hx_parser *parser)
{
	if (parser->position < parser->token_count)
		parser->position++;
}

static u32 parser_check(hx_parser *parser, hx_token_type type)
{
	hx_token *token = parser_current(parser);

	return token != NULL && token->type == type;
}

static b8 parser_match(hx_parser *parser, hx_token_type type)
{
	if (!parser_check(parser, type))
		return false;

	parser_advance(parser);

	return true;
}

static void parser_error(hx_parser *parser, const char *message)
{
	hx_token *token = parser_current(parser);

	if (token == NULL) {
		fprintf(stderr, "parser error: %s\n", message);
		return;
	}

	fprintf(
		stderr,
		"parser error at %u:%u: %s\n",
		token->line,
		token->column,
		message
	);

}

static b8 parse_width(hx_parser *parser, hx_width *width)
{
	if (!parser_match(parser, TOKEN_DOT))
		return false;

	hx_token *token = parser_current(parser);

	if (token == NULL || token->type != TOKEN_IDENTIFIER)
		return false;

	if (token_equals(token, "b"))
		*width = HX_WIDTH_8;
	else if (token_equals(token, "w"))
		*width = HX_WIDTH_16;
	else if (token_equals(token, "d"))
		*width = HX_WIDTH_32;
	else if (token_equals(token, "q"))
		*width = HX_WIDTH_64;
	else
		return false;

	parser_advance(parser);

	return true;
}

static b8 parse_register(hx_token token, u32 *reg)
{
	if (token.type != TOKEN_IDENTIFIER)
		return false;

	if (token.text_length < 2)
		return 0;

	if (token.text[0] != 'r')
		return false;

	u32 value = 0;

	for (u32 i = 1; i < token.text_length; i++) {
		char c = token.text[i];

		if (c < '0' || c > '9')
			return false;

		value = value * 10 + (u32)(c - '0');

		if (value > 31)
			return false;
	}

	*reg = value;

	return true;
}

static b8 parse_integer(hx_token token, s64 *value)
{
	char buffer[64];

	if (token.text_length >= sizeof(buffer))
		return false;

	memcpy(buffer, token.text, token.text_length);

	buffer[token.text_length] = '\0';

	char *end;

	s64 result = strtoll(buffer, &end, 0);

	if (*end != '\0')
		return false;

	*value = (s64)result;

	return true;
}

static b8 parse_memory_operand(hx_parser *parser, hx_operand *operand)
{
	if (!parser_match(parser, TOKEN_LBRACKET))
		return false;

	hx_token *token = parser_current(parser);

	if (token == NULL) {
		parser_error(parser, "expected register after '['");
		return false;
	}

	u32 reg;

	if (!parse_register(*token, &reg)) {
		parser_error(parser, "expected register in memory operand");
		return false;
	}

	parser_advance(parser);

	s64 offset = 0;

	if (parser_match(parser, TOKEN_PLUS)) {
		token = parser_current(parser);

		if (token == NULL || token->type != TOKEN_NUMBER) {
			parser_error(parser, "expected number after '+'");
			return false;
		}

		if (!parse_integer(*token, &offset)) {
			parser_error(parser, "invalid memory offset");
			return false;
		}

		parser_advance(parser);
	} else if (parser_match(parser, TOKEN_MINUS)) {
		token = parser_current(parser);

		if (token == NULL || token->type != TOKEN_NUMBER) {
			parser_error(parser, "expected number after '-'");
			return false;
		}

		if (!parse_integer(*token, &offset)) {
			parser_error(parser, "invalid memory offset");
			return false;
		}

		offset = -offset;

		parser_advance(parser);
	}

	if (!parser_match(parser, TOKEN_RBRACKET)) {
		parser_error(parser, "expected ']'");
		return false;
	}

	operand->type = HX_OPERAND_MEMORY;
	operand->value.memory.reg = reg;
	operand->value.memory.offset = offset;

	return true;
}

static b8 parse_operand(hx_parser *parser, hx_operand *operand)
{
	hx_token *token = parser_current(parser);

	u32 reg;

	if (parse_register(*token, &reg)) {
		operand->type = HX_OPERAND_REGISTER;
		operand->value.reg = reg;

		parser_advance(parser);
		return true;
	}

	if (token->type == TOKEN_NUMBER) {
		s64 value;

		if (!parse_integer(*token, &value))
			return false;

		operand->type = HX_OPERAND_IMMEDIATE;
		operand->value.imm = value;

		parser_advance(parser);
		return true;
	}

	if (token->type == TOKEN_IDENTIFIER) {
		operand->type = HX_OPERAND_LABEL;
		operand->value.label.text = token->text;
		operand->value.label.length = token->text_length;

		parser_advance(parser);
		return true;
	}

	if (token->type == TOKEN_LBRACKET) {
		return parse_memory_operand(parser, operand);
	}

	return false;
}

static b8 parser_is_label(hx_parser *parser)
{
	if (parser->position + 1 >= parser->token_count)
		return false;

	hx_token *current = &parser->tokens[parser->position];
	hx_token *next = &parser->tokens[parser->position + 1];

	return current->type == TOKEN_IDENTIFIER && next->type == TOKEN_COLON;
}

static b8 parse_label(hx_parser *parser, hx_program *program)
{
	hx_token *token = parser_current(parser);

	if (token == NULL)
		return false;

	hx_label label;

	label.name = token->text;
	label.name_length = token->text_length;

	parser_advance(parser);

	if (!parser_match(parser, TOKEN_COLON)) {
		parser_error(parser, "expected ':' after label");
		return false;
	}

	hx_node node;

	node.type = HX_NODE_LABEL;
	node.value.label = label;

	if (!program_add_node(program, node)) {
		parser_error(parser, "failed to add label");
		return false;
	}

	parser_match(parser, TOKEN_NEWLINE);

	return true;
}

static b8 parse_instruction(hx_parser *parser, hx_program *program)
{
	hx_token *token = parser_current(parser);

	if (token == NULL || token->type != TOKEN_IDENTIFIER) {
		parser_error(parser, "expected instruction");
		return false;
	}

	hx_mnemonic mnemonic;

	if (!mnemonic_from_token(*token, &mnemonic)) {
		parser_error(parser, "unknown instruction");
		return false;
	}

	parser_advance(parser);

	hx_width width = HX_WIDTH_64;

	if (parser_check(parser, TOKEN_DOT)) {
		if (!parse_width(parser, &width)) {
			parser_error(parser, "invalid operand width");
			return false;
		}
	}

	hx_instruction instruction;

	instruction.mnemonic = mnemonic;
	instruction.width = width;
	instruction.operand_count = 0;
	instruction.line = token->line;

	if (!parser_check(parser, TOKEN_NEWLINE) && !parser_check(parser, TOKEN_EOF)) {
		for (;;) {
			if (instruction.operand_count >= 3) {
				parser_error(parser, "too many operands");
				return false;
			}

			if (!parse_operand(parser, &instruction.operands[instruction.operand_count])) {
				parser_error(parser, "invalid operand");
				return false;
			}

			instruction.operand_count ++;

			if (!parser_match(parser, TOKEN_COMMA))
				break;
		}
	}

	if (!parser_check(parser, TOKEN_NEWLINE) &&
			!parser_check(parser, TOKEN_EOF)) {
		parser_error(parser, "expected newline after instruction");
		return false;
	}

	parser_match(parser, TOKEN_NEWLINE);

	hx_node node;

	node.type = HX_NODE_INSTRUCTION;
	node.value.instruction = instruction;

	if (!program_add_node(program, node)) {
		parser_error(parser, "failed to add instruction");
		return false;
	}

	return true;
}

b8 parser_init(hx_parser *parser, hx_token *tokens, u32 token_count)
{
	parser->tokens = tokens;
	parser->token_count = token_count;

	parser->position = 0;

	return success;
}

b8 parser_parse(hx_parser *parser, hx_program *program)
{
	while (!parser_check(parser, TOKEN_EOF)) {
		if (parser_match(parser, TOKEN_NEWLINE))
			continue;

		if (parser_is_label(parser)) {
			if (!parse_label(parser, program))
				return false;

			continue;
		}

		if (!parse_instruction(parser, program))
			return false;
	}
	
	return success;
}
