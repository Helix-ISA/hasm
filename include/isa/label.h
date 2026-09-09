#ifndef HX_LABEL_H
#define HX_LABEL_H

#include "types.h"
typedef struct {
	const char *name;
	u32 name_length;
	u32 instruction_index;
} hx_label;

#endif
