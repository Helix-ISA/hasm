#ifndef HX_CODEGEN_H
#define HX_CODEGEN_H

#include "parser/ast.h"
#include <isac/instruction.h>
#include <stdio.h>

typedef struct {
	u32 *codes;
	u32 code_count;
	u32 capacity;
} hx_code;

typedef struct {
	const hx_ast *ast;
	hx_code *code;
} hx_codegen;

b8 codegen_init(hx_codegen *codegen, const hx_ast *ast);
b8 codegen_free(hx_codegen *codegen);

b8 codegen_generate(hx_codegen *codegen);
b8 codegen_write(hx_codegen *codegen, FILE *out);

#endif
