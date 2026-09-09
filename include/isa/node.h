#ifndef HX_NODE_H
#define HX_NODE_H

#include "isa/instruction.h"
#include "isa/label.h"

#define HX_INITIAL_NODE_CAPCITY 8

typedef enum {
	HX_NODE_INSTRUCTION,
	HX_NODE_LABEL,
	HX_NODE_ASSEMBLER_DIRECTIVE
} hx_node_type;

typedef struct {
	hx_node_type type;

	union {
		hx_instruction instruction;
		
		hx_label label;
	} value;
} hx_node;

#endif
