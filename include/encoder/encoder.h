#ifndef HX_ENCODER_H
#define HX_ENCODER_H

#include "cli/cl_parser.h"
#include "isa/program.h"

typedef struct {
	u8 *data;
	u32 size;
	u32 capacity;
} hx_binary;

b8 encoder_init(hx_program *program);
b8 encoder_free(hx_program *program);

b8 binary_free(hx_binary *binary);

b8 encoder_encode(const hx_program *program, hx_binary *binary, hx_cli *cli);

#endif
