#include "parser/ast.h"
#include "parser/symbol.h"
#include "types.h"
#include <isac/instruction.h>
#include <isac/operand.h>
#include <stdio.h>
#include <stdlib.h>

b8 ast_init(hx_ast *ast)
{
	if (ast == NULL)
		return failure;

	ast->capacity = 128;
	ast->nodes = malloc(sizeof(hx_ast) * ast->capacity);
	ast->node_count = 0;

	ast->symbol_table = malloc(sizeof(hx_symbol_table));
	if (ast->symbol_table == NULL)
		return failure;

	symbol_table_init(ast->symbol_table);

	return success;
}

b8 ast_free(hx_ast *ast)
{
	if (ast == NULL)
		return failure;

	ast->capacity = 0;
	ast->node_count = 0;

	free(ast->nodes);

	symbol_table_free(ast->symbol_table);

	ast->nodes = NULL;

	return success;
}

b8 ast_add_node(hx_ast *ast, const hx_node node)
{
	if (ast == NULL)
		return failure;

	if (ast->node_count + 1 >= ast->capacity) {
		u32 new_capacity = ast->capacity * 2;
		hx_node *new_nodes = realloc(ast->nodes, sizeof(hx_ast) * new_capacity);
		if (new_nodes == NULL) {
			fprintf(stderr, "out of memory\n");
			return failure;
		}

		ast->nodes = new_nodes;
		ast->capacity = new_capacity;
	}

	ast->nodes[ast->node_count++] = node;

	return success;
}

b8 ast_resolve_symbols(hx_ast *ast)
{
	b8 status = success;

	/* Pass 1: collect all labels */
	for (u64 i = 0; i < ast->node_count; i++) {
		if (ast->nodes[i].type != HX_NODE_LABEL)
			continue;

		hx_symbol symbol;

		symbol.name = ast->nodes[i].value.label.name;
		symbol.name_length = ast->nodes[i].value.label.name_length;
		symbol.address = ast->nodes[i].value.label.address;

		if (!symbol_table_add(ast->symbol_table, symbol))
			status = failure;
	}

	/* Pass 2: resolve symbol operands */
	for (u64 i = 0; i < ast->node_count; i++) {
		if (ast->nodes[i].type != HX_NODE_INSTRUCTION)
			continue;

		hx_instruction *inst = ast->nodes[i].value.instruction;

		for (u8 j = 0; j < instruction_operand_count(inst); j++) {
			if (!instruction_operand_match_type(inst, j, HX_SYMBOL))
				continue;

			const char *name;
			u32 name_length;

			instruction_operand_get_symbol(
				inst,
				j,
				&name,
				&name_length
			);

			hx_symbol *symbol = symbol_table_find(
				ast->symbol_table,
				name,
				name_length
			);

			if (symbol == NULL) {
				fprintf(
					stderr,
					"resolver: failed to resolve symbol: %.*s\n",
					name_length,
					name
				);

				status = failure;
				continue;
			}

			u64 calculated_address =
				symbol->address - get_instruction_address(inst);

			instruction_operand_resolve_symbol(
				inst,
				j,
				calculated_address
			);
		}
	}

	return status;
}

b8 ast_optimize(hx_ast *ast)
{
	(void)ast;
	return success;
}
