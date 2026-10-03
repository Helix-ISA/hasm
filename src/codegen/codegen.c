#include "codegen/codegen.h"
#include "parser/ast.h"
#include "types.h"
#include <isac/instruction.h>
#include <isac/format.h>
#include <isac/mnemonic.h>
#include <stdio.h>
#include <stdlib.h>

static b8 codegen_append(hx_code *code, u32 encoded)
{
	if (code->code_count + 1 >= code->capacity) {
		u32 new_capacity = code->capacity * 2;
		u32 *new_codes = realloc(code->codes, sizeof(u32) * new_capacity);
		if (new_codes == NULL) {
			fprintf(stderr, "out of memory\n");
			return failure;
		}

		code->codes = new_codes;
		code->capacity = new_capacity;
	}

	if (encoded == 0)
		return failure;

	code->codes[code->code_count++] = encoded;

	return success;
}

b8 codegen_init(hx_codegen *codegen, const hx_ast *ast)
{
	if (codegen == NULL)
		return failure;

	codegen->code = malloc(sizeof(hx_code));

	codegen->ast = ast;

	codegen->code->capacity = 128;
	codegen->code->codes = malloc(sizeof(u32) * codegen->code->capacity);

	return success;
}

b8 codegen_free(hx_codegen *codegen)
{
	if (codegen == NULL)
		return failure;

	codegen->ast = NULL;

	free(codegen->code);

	return success;
}

b8 codegen_instruction(hx_codegen *codegen, hx_instruction *instruction)
{
	hx_mnemonic mnemonic = instruction_mnemonic(instruction);

	switch (mnemonic) {
		case HX_ADD:
		case HX_SUB:
		case HX_AND:
		case HX_OR:
		case HX_XOR:
		case HX_SLL:
		case HX_SLR:
		case HX_SAR:
		case HX_SLT:
		case HX_SLTU:
			if(!codegen_append(codegen->code, format_r_encode(instruction))) {
				return failure;
			} break;

		case HX_ADDI:
		case HX_ANDI:
		case HX_ORI:
		case HX_XORI:
		case HX_SLLI:
		case HX_SLRI:
		case HX_SARI:
		case HX_SLTI:
		case HX_SLTUI:
		case HX_JRAL:

		/* Pseudo */
		case HX_MOV:
		case HX_NOP:
			if(!codegen_append(codegen->code, format_i_encode(instruction))) {
				return failure;
			} break;

		case HX_SB:
		case HX_SQ:
		case HX_SH:
		case HX_SW:
			if(!codegen_append(codegen->code, format_s_encode(instruction))) {
				return failure;
			} break;

		case HX_LB:
		case HX_LQ:
		case HX_LH:
		case HX_LW:
		case HX_LBU:
		case HX_LQU:
		case HX_LHU:
			if(!codegen_append(codegen->code, format_i_encode(instruction))) {
				return failure;
			} break;

		/* Pseudo */
		case HX_JMP:
		case HX_JAL:
			if(!codegen_append(codegen->code, format_j_encode(instruction))) {
				return failure;
			} break;

		case HX_BEQ:
		case HX_BNE:
		case HX_BLT:
		case HX_BGE:
			if(!codegen_append(codegen->code, format_b_encode(instruction))) {
				return failure;
			} break;

		default:
			return failure;
		
	}

	return success;
}

b8 codegen_generate(hx_codegen *codegen)
{
	const hx_ast *ast = codegen->ast;

	for (u64 i = 0; i < ast->node_count; i++) {
		const hx_node *node = &ast->nodes[i];
		switch (node->type) {
			case HX_NODE_INSTRUCTION:
				if (!codegen_instruction(codegen, node->value.instruction))
					return failure;
				break;
			case HX_NODE_LABEL:
				break;
			case HX_NODE_DIRECTIVE:
				break;
			default:
				break;
		}
	}

	return success;
}

b8 codegen_write(hx_codegen *codegen, FILE *out)
{
	for (u64 i = 0; i < codegen->code->code_count; i++) {
		u32 code = codegen->code->codes[i];

		if (fwrite(&code, sizeof(u32), 1, out) != 1) {
			return failure;
		}
	}

	return success;
}
