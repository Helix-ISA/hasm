#include "isa/register.h"

const char *width_name(hx_width width)
{
	switch (width) {
		case HX_WIDTH_8: return "8";
		case HX_WIDTH_16: return "16";
		case HX_WIDTH_32: return "32";
		case HX_WIDTH_64: return "64";
		default: return "64";
	}
}
