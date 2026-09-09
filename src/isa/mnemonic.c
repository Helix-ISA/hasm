#include "isa/mnemonic.h"

#include "lexer/token.h"
#include "types.h"

const char *mnemonic_name(hx_mnemonic mnemonic)
{
	switch (mnemonic) {
		case HX_MN_NOP:    return "nop";
		case HX_MN_HLT:    return "hlt";
		case HX_MN_SYSCALL:return "syscall";
		case HX_MN_TRAP:   return "trap";
		case HX_MN_IRET:   return "iret";
		case HX_MN_EI:     return "ei";
		case HX_MN_DI:     return "di";

		case HX_MN_ADD:    return "add";
		case HX_MN_SUB:    return "sub";
		case HX_MN_AND:    return "and";
		case HX_MN_OR:     return "or";
		case HX_MN_XOR:    return "xor";
		case HX_MN_NOT:    return "not";
		case HX_MN_MOV:    return "mov";

		case HX_MN_ST:     return "st";
		case HX_MN_LD:     return "ld";
		case HX_MN_LEA:    return "lea";

		case HX_MN_SHL:    return "shl";
		case HX_MN_SHR:    return "shr";
		case HX_MN_SAR:    return "sar";

		case HX_MN_JMP:    return "jmp";
		case HX_MN_LJMP:   return "ljmp";

		case HX_MN_BEQ:    return "beq";
		case HX_MN_BNE:    return "bne";
		case HX_MN_BLT:    return "blt";
		case HX_MN_BGE:    return "bge";

		case HX_MN_CALL:   return "call";
		case HX_MN_RET:    return "ret";

		default:
			return "unknown";
	}
}

b8 mnemonic_from_token(hx_token token, hx_mnemonic *mnemonic)
{
	if (token_equals(&token, "nop")) {
		*mnemonic = HX_MN_NOP;
		return success;
	} else if (token_equals(&token, "hlt")) {
		*mnemonic = HX_MN_HLT;
		return success;
	} else if (token_equals(&token, "syscall")) {
		*mnemonic = HX_MN_SYSCALL;
		return success;
	} else if (token_equals(&token, "trap")) {
		*mnemonic = HX_MN_TRAP;
		return success;
	} else if (token_equals(&token, "iret")) {
		*mnemonic = HX_MN_IRET;
		return success;
	} else if (token_equals(&token, "ei")) {
		*mnemonic = HX_MN_EI;
		return success;
	} else if (token_equals(&token, "di")) {
		*mnemonic = HX_MN_DI;
		return success;
	} else if (token_equals(&token, "add")) {
		*mnemonic = HX_MN_ADD;
		return success;
	} else if (token_equals(&token, "sub")) {
		*mnemonic = HX_MN_SUB;
		return success;
	} else if (token_equals(&token, "and")) {
		*mnemonic = HX_MN_AND;
		return success;
	} else if (token_equals(&token, "or")) {
		*mnemonic = HX_MN_OR;
		return success;
	} else if (token_equals(&token, "xor")) {
		*mnemonic = HX_MN_XOR;
		return success;
	} else if (token_equals(&token, "not")) {
		*mnemonic = HX_MN_NOT;
		return success;
	} else if (token_equals(&token, "mov")) {
		*mnemonic = HX_MN_MOV;
		return success;
	} else if (token_equals(&token, "st")) {
		*mnemonic = HX_MN_ST;
		return success;
	} else if (token_equals(&token, "ld")) {
		*mnemonic = HX_MN_LD;
		return success;
	} else if (token_equals(&token, "lea")) {
		*mnemonic = HX_MN_LEA;
		return success;
	} else if (token_equals(&token, "shl")) {
		*mnemonic = HX_MN_SHL;
		return success;
	} else if (token_equals(&token, "shr")) {
		*mnemonic = HX_MN_SHR;
		return success;
	} else if (token_equals(&token, "sar")) {
		*mnemonic = HX_MN_SAR;
		return success;
	} else if (token_equals(&token, "jmp")) {
		*mnemonic = HX_MN_JMP;
		return success;
	} else if (token_equals(&token, "ljmp")) {
		*mnemonic = HX_MN_LJMP;
		return success;
	} else if (token_equals(&token, "beq")) {
		*mnemonic = HX_MN_BEQ;
		return success;
	} else if (token_equals(&token, "bne")) {
		*mnemonic = HX_MN_BNE;
		return success;
	} else if (token_equals(&token, "blt")) {
		*mnemonic = HX_MN_BLT;
		return success;
	} else if (token_equals(&token, "bge")) {
		*mnemonic = HX_MN_BGE;
		return success;
	} else if (token_equals(&token, "call")) {
		*mnemonic = HX_MN_CALL;
		return success;
	} else if (token_equals(&token, "ret")) {
		*mnemonic = HX_MN_RET;
		return success;
	}

	return failure;
}
