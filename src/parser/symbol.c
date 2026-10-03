#include "parser/symbol.h"
#include "types.h"
#include <isac/instruction.h>
#include <isac/operand.h>
#include <isac/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

b8 symbol_table_init(hx_symbol_table *symbol_table)
{
	if (symbol_table == NULL)
		return failure;

	symbol_table->symbol_count = 0;
	symbol_table->capacity = 128;

	symbol_table->symbols = malloc(sizeof(hx_symbol) * symbol_table->capacity);
	if (symbol_table->symbols == NULL)
		return failure;

	symbol_table->hash_capacity = 256;
	symbol_table->hashes = calloc(symbol_table->hash_capacity, sizeof(u32));

	if (symbol_table->hashes == NULL) {
		free(symbol_table->symbols);
		symbol_table->symbols = NULL;
		return failure;
	}

	return success;
}

b8 symbol_table_free(hx_symbol_table *symbol_table)
{
	if (symbol_table == NULL)
		return failure;

	free(symbol_table->symbols);

	return success;
}

static u64 symbol_hash(const char *name, u32 length)
{
	u64 hash = 14695981039346656037ULL;

	for (u32 i = 0; i < length; i++) {
		hash ^= (u8)name[i];
		hash *= 1099511628211ULL;
	}

	return hash;
}

b8 symbol_table_add(hx_symbol_table *table, const hx_symbol symbol)
{
	if (table == NULL)
		return failure;

	if (table->symbol_count + 1 >= table->capacity) {
		u32 new_capacity = table->capacity * 2;

		hx_symbol *new_symbols = realloc(
			table->symbols,
			sizeof(hx_symbol) * new_capacity
		);

		if (new_symbols == NULL) {
			fprintf(stderr, "out of memory\n");
			return failure;
		}

		table->symbols = new_symbols;
		table->capacity = new_capacity;
	}

	u32 symbol_index = table->symbol_count;

	table->symbols[symbol_index] = symbol;
	table->symbol_count++;

	u64 hash = symbol_hash(symbol.name, symbol.name_length);
	u32 mask = table->hash_capacity - 1;
	u32 index = hash & mask;

	while (table->hashes[index] != 0)
		index = (index + 1) & mask;

	table->hashes[index] = symbol_index + 1;

	return success;
}

hx_symbol *symbol_table_find(
	hx_symbol_table *table,
	const char *name,
	u32 name_length)
{
	if (table->hash_capacity == 0)
		return NULL;

	u64 hash = symbol_hash(name, name_length);
	u32 mask = table->hash_capacity - 1;
	u32 index = hash & mask;

	for (;;) {
		u32 entry = table->hashes[index];
		if (entry == 0)
			return NULL;

		hx_symbol *symbol = &table->symbols[entry - 1];

		if (symbol->name_length == name_length &&
		    memcmp(symbol->name, name, name_length) == 0) {
			return symbol;
		}

		index = (index + 1) & mask;
	}
}
