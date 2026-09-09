#include "hasm/hasm.h"

#include <stdio.h>
#include <stdlib.h>

#include "cli/cl_parser.h"
#include "isa/instruction.h"
#include "isa/mnemonic.h"
#include "isa/node.h"
#include "isa/operand.h"
#include "isa/program.h"
#include "isa/register.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "io/file.h"
#include "parser/parser.h"
#include "types.h"

static void print_token(hx_token token)
{
	printf(
			"%-10s  line=%u  column=%u  text=\"%.*s\"\n",
			token_type_name(token.type),
			token.line,
			token.column,
			(int)token.text_length,
			token.text
	      );
}

static void print_operand(const hx_operand *operand)
{
	switch (operand->type) {
		case HX_OPERAND_REGISTER:
			printf("register: r%u", operand->value.reg);
			break;
		case HX_OPERAND_IMMEDIATE:
			printf("immediate: %lld", (long long)operand->value.imm);
			break;
		case HX_OPERAND_MEMORY:
			if (operand->value.memory.offset > 0) {
				printf(
						"memory: [r%u + %lld]",
						operand->value.memory.reg,
						(long long)operand->value.memory.offset
				      );
			} else if (operand->value.memory.offset < 0) {
				printf(
						"memory: [r%u - %lld]",
						operand->value.memory.reg,
						(long long)-operand->value.memory.offset
				      );
			} else {
				printf(
						"memory: [r%u]",
						operand->value.memory.reg
				      );
			}
			break;
		case HX_OPERAND_LABEL:
			printf("label: %.*s", (int)operand->value.label.length,
					operand->value.label.text);
			break;

		default:
			printf("unknown");
			break;
	}
}

static void parser_debug(const hx_program *program)
{
	for (u32 i = 0; i < program->node_count; i++) {
		const hx_node *node = &program->nodes[i];

		switch (node->type) {
			case HX_NODE_LABEL:
				printf("label: %.*s\n", (int)node->value.label.name_length,
						node->value.label.name);
				break;
			case HX_NODE_INSTRUCTION: {
				const hx_instruction *instruction =
					&node->value.instruction;

				printf(
					"instruction:\n"
					"  mnemonic: %s\n"
					"  width:    %s\n"
					"  line:     %u\n"
					"  operands: %u\n",
					mnemonic_name(instruction->mnemonic),
					width_name(instruction->width),
					instruction->line,
					instruction->operand_count
				);

				for (u32 j = 0; j < instruction->operand_count; j++) {
					printf("    [%u] ", j);

					print_operand(&instruction->operands[j]);

					putchar('\n');
				}
				break;
			}

			default:
				printf("unknown node\n");
				break;
		}
	}
}

int hasm(int argc, char** argv)
{
	/* CLI Args */
	hx_cli cli;
	cl_parse_init(&cli);
	if (!cl_parse_args(argc, argv, &cli))
		return failure;

	/* Source bytes */
	u32 source_length = 0;
	char *source = read_file(cli.input_file, &source_length);
	if (source == NULL)
		return 1;

	/* Tokenize the source */
	u32 token_count;
	hx_token *tokens = lexer_tokenize(source, source_length, &token_count);
	if (tokens == NULL) {
		fprintf(stderr, "lexer_tokenize failed\n");

		free(source);
		cl_parse_free(&cli);
		return 1;
	}
	
	/* DEBUG: Remove in release, prints tokens and exits early */
	if (cli.lexer_debug) {
		for (u32 i = 0; i < token_count; i++) {
			print_token(tokens[i]);
		}
		return 0;
	}

	/* Prepare parser and nodes */
	hx_parser parser;
	parser_init(&parser, tokens, token_count);

	hx_program program;
	program_init(&program);

	/* Parse the tokens */
	if (!parser_parse(&parser, &program)) {
		free(tokens);
		free(source);
		cl_parse_free(&cli);
		return 1;
	}

	/* DEBUG: Remove in release, prints nodes and exits early */
	if (cli.parser_debug) {
		parser_debug(&program);
		return 0;
	}

	/* Start shutdown process */
	if (!program_free(&program)) {
		fprintf(stderr, "failed to free program\n");
		return 1;
	}
	
	cl_parse_free(&cli);
	free(tokens);
	free(source);
	return 0;
}
