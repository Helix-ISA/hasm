#ifndef HX_AST_H
#define HX_AST_H

#include <isac/instruction.h>

#include "parser/symbol.h"
#include "types.h"

typedef enum {
	HX_NODE_INSTRUCTION,
	HX_NODE_LABEL,
	HX_NODE_DIRECTIVE
} hx_node_type;

typedef struct {
	hx_node_type type;

	union {
		struct {
			const char *name;
			u32 name_length;
			u32 address;
		} label;

		hx_instruction *instruction;

		struct {
			const char *name;
		} directive;
	} value;
} hx_node;

typedef struct {
	hx_node *nodes;
	u32 node_count;
	u32 capacity;

	hx_symbol_table *symbol_table;
} hx_ast;

b8 ast_init(hx_ast *ast);
b8 ast_free(hx_ast *ast);

b8 ast_add_node(hx_ast *ast, const hx_node node);

b8 ast_resolve_symbols(hx_ast *ast);
b8 ast_optimize(hx_ast *ast);

#endif
