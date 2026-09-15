#include "isa/operand.h"
#include "isa/mnemonic.h"

b8 large_operand_instruction(hx_mnemonic mnenmonic)
{
	switch (mnenmonic) {
		case HX_MN_CSL: return true;
		case HX_MN_CSINC: return true;
		case HX_MN_CSNEG: return true;

		default: return false;
	}
}
