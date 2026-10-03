#include "parser/parser.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser/ast.h"
#include "types.h"
#include <ctype.h>
#include <isac/mnemonic.h>
#include <isac/operand.h>
#include <stdio.h>

#include <isac/instruction.h>
#include <stdlib.h>
#include <string.h>

static void parser_advance(hx_parser *parser)
{
	parser->previous = parser->current;
	parser->current = parser->peek;
	parser->peek = lexer_next(parser->lexer);

	parser->has_current = 1;
}

static inline b8 parser_check(hx_parser *parser, hx_token_type type)
{
	return parser->current.type == type;
}

static inline b8 parser_peek(hx_parser *parser, hx_token_type type)
{
	return parser->peek.type == type;
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
	fprintf(
			stderr, 
			"error:%u:%u: %s\n",
			parser->current.location.line,
			parser->current.location.column,
			message
	);

	parser->had_error = true;
}

static b8 parser_expect(hx_parser *parser, hx_token_type type, const char *message)
{
	if (!parser_match(parser, type)) {
		parser_error(parser, message);
		return false;
	}

	return true;
}

static u8 lex_register(const char *lex, u32 lex_length)
{
	if (lex_length > 3)
		return -1;

	if (lex[0] != 'h')
		return -1;

	lex++;

	char buffer[2];
	strncpy(buffer, lex, 2);

	u8 number = atoi(buffer);

	return number;
}

static u64 lex_number(const char *lex, u32 lex_length)
{
	u64 value = 0;
	u32 i = 0;
	u32 base = 10;

	if (lex_length >= 2 &&
			lex[0] == '0' &&
			(lex[1] == 'x' || lex[1] == 'X')) {
		base = 16;
		i = 2;
	}

	for (; i < lex_length; i++) {
		char c = lex[i];
		u32 digit;

		if (c >= '0' && c <= '9')
			digit = (u32)(c - '0');
		else if (c >= 'a' && c <= 'f')
			digit = (u32)(c - 'a') + 10;
		else if (c >= 'A' && c <= 'F')
			digit = (u32)(c - 'A') + 10;
		else
			break;

		if (digit >= base)
			break;

		value = value * base + digit;
	}

	return value;
}

/**
 * out_offset can return NULL
 */
static b8 parser_parse_memory(hx_parser *parser, hx_operand **out_reg, hx_operand **out_offset)
{
	if (parser_expect(parser, TOKEN_LBRACKET, "expected '['"))
		return false;

	if (parser_expect(parser, TOKEN_IDENTIFIER, "expected register"))
		return false;

	*out_reg = operand_create_register(lex_register(parser->current.lexme, parser->current.lexme_length));

	parser_advance(parser);

	/* No offset */
	if (!parser_check(parser, TOKEN_NUMBER)) {
		parser_expect(parser, TOKEN_RBRACKET, "expected ']'");
		return true;
	}

	if (parser_expect(parser, TOKEN_NUMBER, "expected offset"))
		return false;

	*out_offset = operand_create_immediate(lex_number(parser->current.lexme, parser->current.lexme_length));

	if (parser_expect(parser, TOKEN_RBRACKET, "expected ']'"))
		return false;

	return true;
}

static hx_operand *parser_parse_operand(hx_parser *parser)
{
	switch (parser->current.type) {
		case TOKEN_IDENTIFIER: {
			if (parser->current.lexme[0] == 'h' && isdigit(parser->current.lexme[1])) {
				hx_operand *reg = operand_create_register(lex_register(parser->current.lexme, parser->current.lexme_length));
				parser_advance(parser);
				return reg;
			} else {
				hx_operand *symbol = operand_create_symbol(parser->current.lexme, parser->current.lexme_length);
				parser_advance(parser);
				return symbol;
			}
		}

		case TOKEN_NUMBER: {
			hx_operand *imm = operand_create_immediate(lex_number(parser->current.lexme, parser->current.lexme_length));
			parser_advance(parser);
			return imm;
		}

		case TOKEN_LBRACKET: {
			parser_advance(parser);
			if (!parser_check(parser, TOKEN_IDENTIFIER)) {
				parser_error(parser, "expected register");
				parser_advance(parser);
				return NULL;
			}
	
			/* Copy reg operand */
			const char *reg = parser->current.lexme;
			u32 reg_length = parser->current.lexme_length;
			parser_advance(parser);

			if (!parser_check(parser, TOKEN_NUMBER)) {
				parser_error(parser, "expected offset");
				parser_advance(parser);
				return NULL;
			}

			hx_operand *mem = operand_create_memory(lex_register(reg, reg_length), lex_number(parser->current.lexme, parser->current.lexme_length));

			parser_advance(parser);

			return mem;
		}

		default:
			return NULL;

	}

	return NULL;
}

static void parser_parse_instruction(hx_parser *parser)
{
	if (!parser_check(parser, TOKEN_IDENTIFIER)) {
		parser_error(parser, "expected instruction");
		return;
	}

	hx_token mnemonic_token = parser->current;
	hx_mnemonic mnemonic = get_mnemonic(mnemonic_token.lexme, mnemonic_token.lexme_length);

	u32 address = parser->instruction_address;
	parser->instruction_address += 4;
	parser_advance(parser);

	hx_operand *operands[3];
	u32 operand_count = 0;

	for (;;) {
		/* Memory operand */
		if (parser_check(parser, TOKEN_LBRACKET)) {
			hx_operand *reg;
			hx_operand *offset;

			if (!parser_parse_memory(parser, &reg, &offset))
				return;

			operands[operand_count++] = reg;
			if (offset != NULL)
				operands[operand_count++] = offset;

		}
		/* Symbol or reg */
		else if (parser_check(parser, TOKEN_IDENTIFIER) || parser_check(parser, TOKEN_NUMBER)) {

			/* Pseudo */
			if (mnemonic == HX_JMP) {
				operands[operand_count++] = operand_create_register(0);
			}
			if (mnemonic == HX_MOV) {
				if (operand_count == 1 && parser_check(parser, TOKEN_NUMBER)) {
					operands[operand_count++] = operand_create_register(0);
					operands[operand_count++] = operand_create_immediate(lex_number(parser->current.lexme, parser->current.lexme_length));
					parser_advance(parser);
					break;
				}
			}

			hx_operand *operand = parser_parse_operand(parser);

			if (operand == NULL)
				return;

			operands[operand_count++] = operand;
			if (operand_count == 2 && mnemonic == HX_MOV) {
				operands[operand_count++] = operand_create_immediate(0);
			}
		} else {
			/* Pseudo */
			if (mnemonic == HX_NOP) {
				operands[operand_count++] = operand_create_register(0);
				operands[operand_count++] = operand_create_register(0);
				operands[operand_count++] = operand_create_immediate(0);
			}
			break;
		}

		if (!parser_match(parser, TOKEN_COMMA))
			break;

		if (operand_count > 3) {
			parser_error(parser, "too many operands");
			return;
		}
	}

	hx_instruction *instruction;

	switch (operand_count) {
		case 0:
			instruction = instruction_create(
					mnemonic,
					address,
					0
			);
			break;
		case 1:
			instruction = instruction_create(
					mnemonic,
					address,
					1,
					operands[0]
			);
			break;
		case 2:
			instruction = instruction_create(
					mnemonic,
					address,
					2,
					operands[0],
					operands[1]
			);
			break;
		case 3:
			instruction = instruction_create(
					mnemonic,
					address,
					3,
					operands[0],
					operands[1],
					operands[2]
			);
			break;

		default:
			parser_error(parser, "too many operands");
			return;
	}

	hx_node node;
	node.type = HX_NODE_INSTRUCTION;
	node.value.instruction = instruction;

	ast_add_node(parser->ast, node);
}

static void parser_parse_line(hx_parser *parser)
{
	/* Labels or Instructions */
	if (parser_check(parser, TOKEN_IDENTIFIER)) {
		/* Label */
		if (parser_peek(parser, TOKEN_COLON)) {
			hx_node node;
			node.type = HX_NODE_LABEL;
			node.value.label.name = parser->current.lexme;
			node.value.label.name_length = parser->current.lexme_length;
			node.value.label.address = parser->current.location.line << 2;
			ast_add_node(parser->ast, node);

			parser_advance(parser); /* Identifier */
			parser_advance(parser); /* Colon */


			/* Same line instruction */
			if (parser_check(parser, TOKEN_IDENTIFIER))
				parser_parse_instruction(parser);
			
		}
		/* Instruction */
		else {
			parser_parse_instruction(parser);
		}

		parser_match(parser, TOKEN_NEWLINE);
		return;
	}
	/* Directives */
	else if (parser_check(parser, TOKEN_DOT)) {
		// TODO: Directives
	}

	parser_error(parser, "expected label, instruction, or directive");
	parser_advance(parser);
}

b8 parser_init(hx_parser *parser, hx_lexer *lexer, hx_ast *ast)
{
	parser->lexer = lexer;
	parser->ast = ast;

	parser->current = (hx_token){0};
	parser->previous = (hx_token){0};
	parser->peek = (hx_token){0};

	parser->has_current = 0;
	parser->had_error = 0;
	parser->instruction_address = 0;

	parser_advance(parser);
	parser_advance(parser);

	return success;
}

b8 parser_free(hx_parser *parser)
{
	parser->lexer = NULL;
	parser->ast = NULL;

	return success;
}

b8 parser_parse(hx_parser *parser)
{
	while (parser->current.type != TOKEN_EOF) {
		parser_parse_line(parser);
	}

	return parser->had_error ? failure : success;
}
