#ifndef HX_SYMBOL_H
#define HX_SYMBOL_H

#include "isa/label.h"
#include "isa/program.h"
#include "types.h"

typedef struct {
	hx_label label;
	u32 address;
} hx_symbol;

typedef struct {
	hx_symbol *symbols;
	u32 count;
	u32 capacity;
} hx_symbol_table;

b8 symbol_table_init(hx_symbol_table *table);
b8 symbol_table_free(hx_symbol_table *table);

b8 collect_symbols(const hx_program *program, hx_symbol_table *symbols);

b8 symbol_add(hx_symbol_table *table, hx_label label, u32 address);
b8 symbol_find(const hx_symbol_table *table, hx_label label, u32 *address);

#endif
