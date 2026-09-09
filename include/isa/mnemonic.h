#ifndef HX_MNEMONIC_H
#define HX_MNEMONIC_H

#include "lexer/token.h"
typedef enum {
	HX_MN_NOP,
	
	HX_MN_HLT,
	HX_MN_SYSCALL,
	HX_MN_TRAP,
	HX_MN_IRET,
	HX_MN_EI,
	HX_MN_DI,

	HX_MN_ADD,
	HX_MN_SUB,

	HX_MN_AND,
	HX_MN_OR,
	HX_MN_XOR,
	HX_MN_NOT,

	HX_MN_MOV,
	HX_MN_ST,
	HX_MN_LD,
	HX_MN_LEA,

	HX_MN_SHL,
	HX_MN_SHR,
	HX_MN_SAR,

	HX_MN_JMP,
	HX_MN_LJMP,

	HX_MN_BEQ,
	HX_MN_BNE,
	HX_MN_BLT,
	HX_MN_BGE,
	
	HX_MN_CALL,
	HX_MN_RET
} hx_mnemonic;

const char *mnemonic_name(hx_mnemonic mnemonic);
b8 mnemonic_from_token(hx_token token, hx_mnemonic *mnemonic);

#endif
