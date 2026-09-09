#include "encoder/symbol.h"
#include "isa/instruction.h"
#include "isa/program.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

b8 symbol_table_init(hx_symbol_table *table)
{
	table->symbols = NULL;
	table->capacity = 0;
	table->count = 0;

	return success;
}

b8 symbol_table_free(hx_symbol_table *table)
{
	free(table->symbols);
	table->capacity = 0;
	table->count = 0;

	return success;
}

b8 symbol_add(hx_symbol_table *table, hx_label label, u32 address)
{
	u32 existing_address;

	if (symbol_find(table, label, &existing_address)) {
		return failure;
	}

	if (table->symbols == NULL) {
		table->capacity = 64;
		
		table->symbols = malloc(sizeof(hx_symbol) * table->capacity);

		if (table->symbols == NULL) {
			fprintf(stderr, "out of memory\n");
			return failure;
		}
	}

	if (table->count >= table->capacity) {
		u32 new_capacity = table->capacity * 2;

		hx_symbol *new_symbols = realloc(table->symbols, new_capacity);

		if (new_symbols == NULL)
			return failure;

		table->symbols = new_symbols;
		table->capacity = new_capacity;
	}

	hx_symbol *symbol = &table->symbols[table->count++];

	symbol->label = label;

	symbol->address = address;

	return success;
}

/* TODO: Actually verify */
static u32 instruction_size(const hx_instruction *instruction)
{
	(void)instruction;
	return 4;
}

b8 collect_symbols(const hx_program *program, hx_symbol_table *symbols)
{
	/* TODO: Option for PIC assembling */
	u32 address = 0;

	for (u32 i = 0; i < program->node_count; i++) {
		const hx_node *node = &program->nodes[i];

		switch (node->type) {
			case HX_NODE_LABEL: {
				const hx_label *label = &node->value.label;

				if (!symbol_add(symbols, *label, address)) {
					fprintf(
						stderr,
						"duplicate label: %.*s\n",
						(int)label->name_length,
						label->name
					);

					return false;
				}

				break;
			}
			case HX_NODE_INSTRUCTION:
				address += instruction_size(
					&node->value.instruction
				);
				break;

			case HX_NODE_ASSEMBLER_DIRECTIVE:
				break;
		}
	}

	return true;
}
b8 symbol_find(const hx_symbol_table *table, hx_label label, u32 *address)
{
	for (u32 i = 0; i < table->count; i++) {
		const hx_symbol *symbol = &table->symbols[i];

		if (symbol->label.name_length != label.name_length)
			continue;

		if (memcmp(symbol->label.name, label.name, label.name_length) != 0)
			continue;

		if (address != NULL)
			*address = symbol->address;

		return success;
	}

	return failure;
}
