#include "isa/mnemonic.h"

#include "lexer/token.h"
#include "types.h"

const char *mnemonic_name(hx_mnemonic mnemonic)
{
	switch (mnemonic) {
		case HX_MN_ADD:    	return "add";
		case HX_MN_ADDI:    	return "addi";
		case HX_MN_SUB:		return "sub";
		case HX_MN_MUL:		return "mul";
		case HX_MN_DIV:		return "div";
		case HX_MN_REM:		return "rem";

		case HX_MN_AND:		return "and";
		case HX_MN_ANDI:	return "andi";
		case HX_MN_OR:		return "or";
		case HX_MN_ORI:		return "ori";
		case HX_MN_XOR:		return "xor";
		case HX_MN_XORI:	return "xori";

		case HX_MN_SLL:		return "sll";
		case HX_MN_SLLI:	return "slli";
		case HX_MN_SLR:		return "slr";
		case HX_MN_SLRI:	return "slri";
		case HX_MN_SAR:		return "sar";
		case HX_MN_SARI:	return "sari";
		case HX_MN_SLT:		return "slt";
		case HX_MN_SLTI:	return "slti";
		case HX_MN_SLTU:	return "sltu";
		case HX_MN_SLTUI:	return "sltui";

		case HX_MN_LB:		return "lb";
		case HX_MN_LQ:		return "lq";
		case HX_MN_LH:		return "lh";
		case HX_MN_LW:		return "lw";
		case HX_MN_LBU:		return "lbu";
		case HX_MN_LQU:		return "lqu";
		case HX_MN_LHU:		return "lhu";

		case HX_MN_SB:		return "sb";
		case HX_MN_SQ:		return "sq";
		case HX_MN_SH:		return "sh";
		case HX_MN_SW:		return "sw";

		case HX_MN_BEQ:		return "beq";
		case HX_MN_BNE:		return "bne";
		case HX_MN_BLT:		return "blt";
		case HX_MN_BGE:		return "bge";
		case HX_MN_BLTU:	return "bltu";
		case HX_MN_BGEU:	return "bgeu";

		case HX_MN_JAL:		return "jal";
		case HX_MN_JALR:	return "jalr";

		case HX_MN_SCALL:	return "scall";
		case HX_MN_STRAP:	return "strap";
		case HX_MN_SRET:	return "sret";
		case HX_MN_WFI:		return "wfi";

		case HX_MN_MOVZ:	return "movz";
		case HX_MN_MOVP:	return "movp";
		case HX_MN_MOVN:	return "movn";

		case HX_MN_CS:		return "cs";
		case HX_MN_CSINC:	return "csinc";
		case HX_MN_CSNEG:	return "csneg";

		default:
			return "unknown";
	}
}
u8 mnemonic_opcode(hx_mnemonic mnemonic)
{
	switch (mnemonic) {
		case HX_MN_ADD:    	return 0x27;
		case HX_MN_ADDI:    	return 0x07;
		case HX_MN_SUB:		return 0x27;
		case HX_MN_MUL:		return 0x27;
		case HX_MN_DIV:		return 0x27;
		case HX_MN_REM:		return 0x27;

		case HX_MN_AND:		return 0x27;
		case HX_MN_ANDI:	return 0x07;
		case HX_MN_OR:		return 0x27;
		case HX_MN_ORI:		return 0x07;
		case HX_MN_XOR:		return 0x27;
		case HX_MN_XORI:	return 0x07;

		case HX_MN_SLL:		return 0x27;
		case HX_MN_SLLI:	return 0x07;
		case HX_MN_SLR:		return 0x27;
		case HX_MN_SLRI:	return 0x07;
		case HX_MN_SAR:		return 0x27;
		case HX_MN_SARI:	return 0x07;
		case HX_MN_SLT:		return 0x27;
		case HX_MN_SLTI:	return 0x07;
		case HX_MN_SLTU:	return 0x27;
		case HX_MN_SLTUI:	return 0x07;

		case HX_MN_LB:		return 0x03;
		case HX_MN_LQ:		return 0x03;
		case HX_MN_LH:		return 0x03;
		case HX_MN_LW:		return 0x03;
		case HX_MN_LBU:		return 0x03;
		case HX_MN_LQU:		return 0x03;
		case HX_MN_LHU:		return 0x03;

		case HX_MN_SB:		return 0x23;
		case HX_MN_SQ:		return 0x23;
		case HX_MN_SH:		return 0x23;
		case HX_MN_SW:		return 0x23;

		case HX_MN_BEQ:		return 0x4F;
		case HX_MN_BNE:		return 0x4F;
		case HX_MN_BLT:		return 0x4F;
		case HX_MN_BGE:		return 0x4F;
		case HX_MN_BLTU:	return 0x4F;
		case HX_MN_BGEU:	return 0x4F;

		case HX_MN_JAL:		return 0x6F;
		case HX_MN_JALR:	return 0x73;

		case HX_MN_SCALL:	return 0x63;
		case HX_MN_STRAP:	return 0x63;
		case HX_MN_SRET:	return 0x63;
		case HX_MN_WFI:		return 0x63;

		case HX_MN_MOVZ:	return 0x43;
		case HX_MN_MOVP:	return 0x43;
		case HX_MN_MOVN:	return 0x43;

		case HX_MN_CS:		return 0x77;
		case HX_MN_CSINC:	return 0x77;
		case HX_MN_CSNEG:	return 0x77;

		default:
			return 0xFF; /* Unknown error code */

	}
}

/**
 * TODO: Rework function to not be a massive if else
 */
b8 mnemonic_from_token(hx_token token, hx_mnemonic *mnemonic)
{
	if (token_equals(&token, "add")) {
		*mnemonic = HX_MN_ADD;
		return success;
	} else if (token_equals(&token, "addi")) {
		*mnemonic = HX_MN_ADDI;
		return success;
	} else if (token_equals(&token, "sub")) {
		*mnemonic = HX_MN_SUB;
		return success;
	} else if (token_equals(&token, "mul")) {
		*mnemonic = HX_MN_MUL;
		return success;
	} else if (token_equals(&token, "div")) {
		*mnemonic = HX_MN_DIV;
		return success;
	} else if (token_equals(&token, "rem")) {
		*mnemonic = HX_MN_REM;
		return success;
	} else if (token_equals(&token, "and")) {
		*mnemonic = HX_MN_AND;
		return success;
	} else if (token_equals(&token, "andi")) {
		*mnemonic = HX_MN_ANDI;
		return success;
	} else if (token_equals(&token, "or")) {
		*mnemonic = HX_MN_OR;
		return success;
	} else if (token_equals(&token, "ori")) {
		*mnemonic = HX_MN_ORI;
		return success;
	} else if (token_equals(&token, "xor")) {
		*mnemonic = HX_MN_XOR;
		return success;
	} else if (token_equals(&token, "xori")) {
		*mnemonic = HX_MN_XORI;
		return success;
	} else if (token_equals(&token, "sll")) {
		*mnemonic = HX_MN_SLL;
		return success;
	} else if (token_equals(&token, "slli")) {
		*mnemonic = HX_MN_SLLI;
		return success;
	} else if (token_equals(&token, "slr")) {
		*mnemonic = HX_MN_SLR;
		return success;
	} else if (token_equals(&token, "slri")) {
		*mnemonic = HX_MN_SLRI;
		return success;
	} else if (token_equals(&token, "sar")) {
		*mnemonic = HX_MN_SAR;
		return success;
	} else if (token_equals(&token, "sari")) {
		*mnemonic = HX_MN_SARI;
		return success;
	} else if (token_equals(&token, "slt")) {
		*mnemonic = HX_MN_SLT;
		return success;
	} else if (token_equals(&token, "slti")) {
		*mnemonic = HX_MN_SLTI;
		return success;
	} else if (token_equals(&token, "sltu")) {
		*mnemonic = HX_MN_SLTU;
		return success;
	} else if (token_equals(&token, "sltui")) {
		*mnemonic = HX_MN_SLTUI;
		return success;

	} else if (token_equals(&token, "lb")) {
		*mnemonic = HX_MN_LB;
		return success;
	} else if (token_equals(&token, "lq")) {
		*mnemonic = HX_MN_LQ;
		return success;
	} else if (token_equals(&token, "lh")) {
		*mnemonic = HX_MN_LH;
		return success;
	} else if (token_equals(&token, "lw")) {
		*mnemonic = HX_MN_LW;
		return success;
	} else if (token_equals(&token, "lbu")) {
		*mnemonic = HX_MN_LBU;
		return success;
	} else if (token_equals(&token, "lqu")) {
		*mnemonic = HX_MN_LQU;
		return success;
	} else if (token_equals(&token, "lhu")) {
		*mnemonic = HX_MN_LHU;
		return success;

	} else if (token_equals(&token, "sb")) {
		*mnemonic = HX_MN_SB;
		return success;
	} else if (token_equals(&token, "sq")) {
		*mnemonic = HX_MN_SQ;
		return success;
	} else if (token_equals(&token, "sh")) {
		*mnemonic = HX_MN_SH;
		return success;
	} else if (token_equals(&token, "sw")) {
		*mnemonic = HX_MN_SW;
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
	} else if (token_equals(&token, "bltu")) {
		*mnemonic = HX_MN_BLTU;
		return success;
	} else if (token_equals(&token, "bgeu")) {
		*mnemonic = HX_MN_BGEU;
		return success;
	} else if (token_equals(&token, "jal")) {
		*mnemonic = HX_MN_JAL;
		return success;
	} else if (token_equals(&token, "jalr")) {
		*mnemonic = HX_MN_JALR;
		return success;
	} else if (token_equals(&token, "scall")) {
		*mnemonic = HX_MN_SCALL;
		return success;
	} else if (token_equals(&token, "strap")) {
		*mnemonic = HX_MN_STRAP;
		return success;
	} else if (token_equals(&token, "sret")) {
		*mnemonic = HX_MN_SRET;
		return success;
	} else if (token_equals(&token, "wfi")) {
		*mnemonic = HX_MN_WFI;
		return success;
	} else if (token_equals(&token, "movz")) {
		*mnemonic = HX_MN_MOVZ;
		return success;
	} else if (token_equals(&token, "movp")) {
		*mnemonic = HX_MN_MOVP;
		return success;
	} else if (token_equals(&token, "movn")) {
		*mnemonic = HX_MN_MOVN;
		return success;
	} else if (token_equals(&token, "cs")) {
		*mnemonic = HX_MN_CS;
		return success;
	} else if (token_equals(&token, "csinc")) {
		*mnemonic = HX_MN_CSINC;
		return success;
	}
	return failure;
}

u8 mnemonic_funct3(hx_mnemonic mnemonic)
{
	switch (mnemonic) {
		case HX_MN_ADD:    	return 0x0;
		case HX_MN_ADDI:    	return 0x0;
		case HX_MN_SUB:		return 0x0;
		case HX_MN_MUL:		return 0x0;
		case HX_MN_DIV:		return 0x1;
		case HX_MN_REM:		return 0x2;

		case HX_MN_AND:		return 0x1;
		case HX_MN_ANDI:	return 0x1;
		case HX_MN_OR:		return 0x2;
		case HX_MN_ORI:		return 0x2;
		case HX_MN_XOR:		return 0x3;
		case HX_MN_XORI:	return 0x3;

		case HX_MN_SLL:		return 0x4;
		case HX_MN_SLLI:	return 0x4;
		case HX_MN_SLR:		return 0x5;
		case HX_MN_SLRI:	return 0x5;
		case HX_MN_SAR:		return 0x5;
		case HX_MN_SARI:	return 0x5;
		case HX_MN_SLT:		return 0x6;
		case HX_MN_SLTI:	return 0x6;
		case HX_MN_SLTU:	return 0x7;
		case HX_MN_SLTUI:	return 0x7;

		case HX_MN_LB:		return 0x0;
		case HX_MN_LQ:		return 0x1;
		case HX_MN_LH:		return 0x2;
		case HX_MN_LW:		return 0x3;
		case HX_MN_LBU:		return 0x4;
		case HX_MN_LQU:		return 0x5;
		case HX_MN_LHU:		return 0x6;

		case HX_MN_SB:		return 0x0;
		case HX_MN_SQ:		return 0x1;
		case HX_MN_SH:		return 0x2;
		case HX_MN_SW:		return 0x3;

		case HX_MN_BEQ:		return 0x0;
		case HX_MN_BNE:		return 0x1;
		case HX_MN_BLT:		return 0x2;
		case HX_MN_BGE:		return 0x3;
		case HX_MN_BLTU:	return 0x4;
		case HX_MN_BGEU:	return 0x5;

		case HX_MN_SCALL:	return 0x0;
		case HX_MN_STRAP:	return 0x1;
		case HX_MN_SRET:	return 0x2;
		case HX_MN_WFI:		return 0x3;

		case HX_MN_CS:		return 0x0;
		case HX_MN_CSINC:	return 0x1;
		case HX_MN_CSNEG:	return 0x2;

		default:
			return 0xFF; /* Unknown error code */

	}
}

u8 mnemonic_funct7(hx_mnemonic mnemonic)
{
	switch (mnemonic) {
		case HX_MN_ADD:    	return 0x0;
		case HX_MN_SUB:		return 0x40;
		case HX_MN_MUL:		return 0x1;
		case HX_MN_DIV:		return 0x1;
		case HX_MN_REM:		return 0x1;

		case HX_MN_AND:		return 0x0;
		case HX_MN_OR:		return 0x0;
		case HX_MN_XOR:		return 0x0;

		case HX_MN_SLL:		return 0x0;
		case HX_MN_SLR:		return 0x0;
		case HX_MN_SAR:		return 0x40;
		case HX_MN_SLT:		return 0x0;
		case HX_MN_SLTU:	return 0x0;

		default:
			return 0xFF; /* Unknown error code */
	}
}
