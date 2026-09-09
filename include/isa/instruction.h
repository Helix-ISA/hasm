#ifndef HX_INSTRUCTION_H
#define HX_INSTRUCTION_H

#include "isa/mnemonic.h"
#include "isa/operand.h"
#include "isa/register.h"
#include "types.h"

typedef struct {
	hx_mnemonic mnemonic;
	hx_width width;

	hx_operand operands[MAX_OPERANDS];
	u32 operand_count;

	u32 line;
} hx_instruction;

#endif
