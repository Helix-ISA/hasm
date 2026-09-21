#ifndef HX_MNEMONIC_H
#define HX_MNEMONIC_H

#include "lexer/token.h"

/**
 * TODO: Pseudo mnemonics, such as nop is actually like addi r0, 0
 */
typedef enum {
	HX_MN_ADD,
	HX_MN_ADDI,
	HX_MN_SUB,

	HX_MN_MUL,
	HX_MN_DIV,
	HX_MN_REM,

	HX_MN_AND,
	HX_MN_ANDI,
	HX_MN_OR,
	HX_MN_ORI,
	HX_MN_XOR,
	HX_MN_XORI,

	HX_MN_SLL,
	HX_MN_SLLI,
	HX_MN_SLR,
	HX_MN_SLRI,
	HX_MN_SAR,
	HX_MN_SARI,
	HX_MN_SLT,
	HX_MN_SLTI,
	HX_MN_SLTU,
	HX_MN_SLTUI,

	HX_MN_LB,
	HX_MN_LQ,
	HX_MN_LH,
	HX_MN_LW,
	HX_MN_LBU,
	HX_MN_LQU,
	HX_MN_LHU,

	HX_MN_SB,
	HX_MN_SQ,
	HX_MN_SH,
	HX_MN_SW,

	HX_MN_BEQ,
	HX_MN_BNE,
	HX_MN_BLT,
	HX_MN_BGE,
	HX_MN_BLTU,
	HX_MN_BGEU,

	HX_MN_JAL,
	HX_MN_JALR,

	HX_MN_SCALL,
	HX_MN_STRAP,
	HX_MN_SRET,
	HX_MN_WFI,

	HX_MN_MOVZ,
	HX_MN_MOVP,
	HX_MN_MOVN,
	HX_MN_MOV,

	HX_MN_CSL,
	HX_MN_CSINC,
	HX_MN_CSNEG,

	HX_MN_UNKNOWN

} hx_mnemonic;

const char *mnemonic_name(hx_mnemonic mnemonic);
u8 mnemonic_opcode(hx_mnemonic mnemonic);
u8 mnemonic_funct3(hx_mnemonic mnemonic);
u8 mnemonic_funct7(hx_mnemonic mnemonic);
b8 mnemonic_from_token(hx_token token, hx_mnemonic *mnemonic);

hx_mnemonic mnemonic_from_encoding(u8 opcode, u8 funct3, u8 funct7);

#endif
