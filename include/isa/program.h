#ifndef HX_PROGRAM_H
#define HX_PROGRAM_H

#include "isa/node.h"

typedef struct {
	hx_node *nodes;
	u32 node_count;
	u32 node_capacity;
} hx_program;

b8 program_init(hx_program *program);
b8 program_free(hx_program *program);

b8 program_add_node(hx_program *program, hx_node node);

#endif
