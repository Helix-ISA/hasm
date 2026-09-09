#include "isa/program.h"

#include "isa/node.h"
#include "types.h"

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

b8 program_init(hx_program *program)
{
	program->nodes = NULL;
	program->node_count = 0;
	program->node_capacity = 0;

	return success;
}

b8 program_free(hx_program *program)
{
	free(program->nodes);

	program->nodes = NULL;
	program->node_count = 0;
	program->node_capacity = 0;

	return success;
}

b8 program_add_node(hx_program *program, hx_node node)
{
	if (program->nodes == NULL) {
		program->node_capacity = HX_INITIAL_NODE_CAPCITY;

		program->nodes = malloc(sizeof(hx_node) * program->node_capacity);

		if (program->nodes == NULL) {
			fprintf(stderr, "out of memory\n");
			return false;
		}
	}

	if (program->node_count >= program->node_capacity) {
		u32 new_capacity = program->node_capacity * 2;
		hx_node *new_nodes = realloc(program->nodes, sizeof(hx_node) * new_capacity);

		if (new_nodes == NULL) {
			fprintf(stderr, "out of memory\n");
			return false;
		}

		program->nodes = new_nodes;
		program->node_capacity = new_capacity;
	}

	program->nodes[program->node_count++] = node;

	return true;
}
