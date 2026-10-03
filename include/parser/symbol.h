#ifndef HX_SYMBOL_H
#define HX_SYMBOL_H

#include "types.h"
#include <isac/instruction.h>

typedef struct {
	const char *name;
	u32 name_length;

	u64 address;
} hx_symbol;

typedef struct {
	hx_symbol *symbols;
	u32 symbol_count;
	u32 capacity;

	u32 *hashes;
	u32 hash_capacity;
} hx_symbol_table;

b8 symbol_table_init(hx_symbol_table *symbol_table);
b8 symbol_table_free(hx_symbol_table *symbol_table);

b8 symbol_table_add(hx_symbol_table *symbol_table, const hx_symbol symbol);

hx_symbol *symbol_table_find(
	hx_symbol_table *table,
	const char *name,
	u32 name_length
);

#endif
