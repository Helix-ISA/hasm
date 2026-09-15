#ifndef HX_OPERAND_H
#define HX_OPERAND_H

#include "isa/mnemonic.h"
#include "types.h"

#define MAX_OPERANDS 4

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

b8 large_operand_instruction(hx_mnemonic mnenmonic);

#endif
