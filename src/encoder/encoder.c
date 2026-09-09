#include "encoder/encoder.h"
#include "cli/cl_parser.h"
#include "encoder/symbol.h"
#include "isa/instruction.h"
#include "isa/label.h"
#include "isa/node.h"
#include "isa/program.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>

b8 encoder_init(hx_program *program)
{
	(void)program;

	return success;
}

b8 encoder_free(hx_program *program)
{
	(void)program;

	return success;
}

b8 binary_free(hx_binary *binary)
{
	free(binary->data);

	binary->data = NULL;
	binary->size = 0;
	binary->capacity = 0;

	return success;
}

/*
static b8 binary_write_byte(hx_binary *binary, u8 value)
{
	if (binary->size >= binary->capacity) {
		u32 new_capacity = binary->capacity == 0 ? 64 : binary->capacity * 2;

		u8 *new_data = realloc(binary->data, new_capacity);

		if (new_data == NULL)
			return failure;

		binary->data = new_data;
		binary->capacity = new_capacity;
	}

	binary->data[binary->size++] = value;

	return success;
}*/

static b8 encode_instruction(const hx_instruction *instruction, hx_symbol_table *symbols, hx_binary *binary)
{
	(void)instruction;
	(void)symbols;
	(void)binary;

	/*
	 * TODO: find the correct candidate instruction to write, then format
	 * in u16 or u32, then send to binary_write_byte to write to hx_binary
	 * then sent to writer what writes binary to file
	 */

	return success;
}

static void symbol_debug(const hx_symbol_table *sym_table, b8 pic)
{
	printf("Symbol Table (%s)\n", pic ? "PIC" : "non-PIC");
	printf("--------------------------------\n");

	for (u32 i = 0; i < sym_table->count; i++) {
		const hx_symbol *symbol =
			&sym_table->symbols[i];

		printf(
			"%.*s:  0x%08X\n",
			(int)symbol->label.name_length,
			symbol->label.name,
			symbol->address
		);
	}

	printf("\n");
}

b8 encoder_encode(const hx_program *program, hx_binary *binary, hx_cli *cli)
{
	hx_symbol_table sym_table;
	symbol_table_init(&sym_table);
	if (!collect_symbols(program, &sym_table))
		return failure;

	if (cli->symbol_debug) {
		symbol_debug(&sym_table, cli->pic);
		return failure;
	}


	for (u32 i = 0; i < program->node_count; i++) {
		switch (program->nodes[i].type) {
			case HX_NODE_LABEL: /* NOTHING */
				break;
			case HX_NODE_INSTRUCTION:
				if (!encode_instruction(&program->nodes[i].value.instruction, &sym_table, binary))
					return failure;
				break;
			case HX_NODE_ASSEMBLER_DIRECTIVE: /* TODO: origin, fill, etc... */
				break;
		}
	}

	(void)program;
	(void)binary;

	return success;
}
