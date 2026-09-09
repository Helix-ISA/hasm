#ifndef HX_OPERAND_H
#define HX_OPERAND_H

#include "types.h"

#define MAX_OPERANDS 3

typedef enum {
	HX_OPERAND_REGISTER,
	HX_OPERAND_IMMEDIATE,
	HX_OPERAND_MEMORY,
	HX_OPERAND_LABEL
} hx_operand_type;

typedef struct {
	hx_operand_type type;

	union {
		u32 reg;
		s64 imm;

		struct {
			u32 reg;
			s64 offset;
		} memory;
	
		struct {
			const char *text;
			u32 length;
		} label;
	} value;
} hx_operand;

#endif
