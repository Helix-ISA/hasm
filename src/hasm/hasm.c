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
			printf("memory: [r%u %+lld]", operand->value.memory.reg,
					(long long)operand->value.memory.offset);
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

void parser_debug(const hx_program *program)
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
	hx_cli cli;

	if (parse_args(argc, argv, &cli) == 0)
		return 1;

	u32 source_length = 0;

	char *source = read_file(cli.input_file, &source_length);

	fclose(cli.input_file);

	if (source == NULL)
		return 1;

	u32 token_count;
	hx_token *tokens = lexer_tokenize(source, source_length, &token_count);
	if (tokens == NULL) {
		fprintf(stderr, "lexer_tokenize failed\n");
		free(source);
		return 1;
	}

	if (cli.lexer_debug) {
		for (u32 i = 0; i < token_count; i++) {
			print_token(tokens[i]);
		}
		return 0;
	}

	hx_parser parser;
	parser_init(&parser, tokens, token_count);

	hx_program program;

	if (!parser_parse(&parser, &program)) {
		free(tokens);
		free(source);
		return 1;
	}

	if (cli.parser_debug) {
		parser_debug(&program);
	}

	free(tokens);
	free(source);
	return 0;
}
