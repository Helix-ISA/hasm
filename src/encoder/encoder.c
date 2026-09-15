#include "encoder/encoder.h"
#include "cli/cl_parser.h"
#include "encoder/symbol.h"
#include "isa/instruction.h"
#include "isa/label.h"
#include "isa/mnemonic.h"
#include "isa/node.h"
#include "isa/operand.h"
#include "isa/program.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

b8 encoder_init(hx_program *program)
{
	(void)program;

	return success;
}

b8 encoder_free(hx_program *program)
{
	(void)program;

	return success;
}

b8 binary_free(hx_binary *binary)
{
	free(binary->data);

	binary->data = NULL;
	binary->size = 0;
	binary->capacity = 0;

	return success;
}

static b8 binary_write_byte(hx_binary *binary, u8 value)
{
	if (binary->size >= binary->capacity) {
		u32 new_capacity = binary->capacity == 0 ? 64 : binary->capacity * 2;

		u8 *new_data = realloc(binary->data, new_capacity);

		if (new_data == NULL)
			return failure;

		binary->data = new_data;
		binary->capacity = new_capacity;
	}

	binary->data[binary->size++] = value;

	return success;
}

static u8 reg_operands(const hx_instruction *instruction)
{
	u8 count = 0;
	for (u8 i = 0; i < instruction->operand_count; i++) {
		if (instruction->operands[i].type == HX_OPERAND_REGISTER)
			count++;
	}

	return count;
}

static u32 encode_r_type(const hx_instruction *instruction) {
	u32 encoded = 0;
	u8 opcode = mnemonic_opcode(instruction->mnemonic);

	if (instruction->operands[3].type == HX_OPERAND_LABEL) {
		hx_label condition = {
			.name = instruction->operands[3].value.label.text,
			.name_length = instruction->operands[3].value.label.length
		};

		char buf[5];
		snprintf(buf, condition.name_length, "%s", condition.name);

		b8 funct7 = 0;

		if (strcmp(buf, "eq") == 0) {
			funct7 = 0x00;
		} else if (strcmp(buf, "ne") == 0) {
			funct7 = 0x01;
		} else if (strcmp(buf, "lt") == 0) {
			funct7 = 0x02;
		} else if (strcmp(buf, "ge") == 0) {
			funct7 = 0x03;
		} else if (strcmp(buf, "ltu") == 0) {
			funct7 = 0x04;
		} else if (strcmp(buf, "geu") == 0) {
			funct7 = 0x05;
		}

		encoded = (opcode) |
		((instruction->operands[0].value.reg & 0x1F) << 7) |
		((mnemonic_funct3(instruction->mnemonic) & 0x07) << 12) |
		((instruction->operands[1].value.reg & 0x1F) << 15) |
		((instruction->operands[2].value.reg & 0x1F) << 20) |
		((funct7 & 0x7F) << 25);

		return encoded;
	}

	switch (reg_operands(instruction)) {
		case 3: /* rd, rs1, rs2 */
			encoded = (opcode) |
			((instruction->operands[0].value.reg & 0x1F) << 7) |
			((mnemonic_funct3(instruction->mnemonic) & 0x07) << 12) |
			((instruction->operands[1].value.reg & 0x1F) << 15) |
			((instruction->operands[2].value.reg & 0x1F) << 20) |
			((mnemonic_funct7(instruction->mnemonic) & 0x7F) << 25);
			break;
		case 2: /* rd rs */
			encoded = (opcode) |
			((instruction->operands[0].value.reg & 0x1F) << 7) |
			((mnemonic_funct3(instruction->mnemonic) & 0x07) << 12) |
			((instruction->operands[0].value.reg & 0x1F) << 15) |
			((instruction->operands[1].value.reg & 0x1F) << 20) |
			((mnemonic_funct7(instruction->mnemonic) & 0x7F) << 25);
			break;
	}

	return encoded;
}

static u32 encode_i_type(const hx_instruction *instruction)
{
	u32 encoded = 0;
	u8 opcode = mnemonic_opcode(instruction->mnemonic);
	switch (reg_operands(instruction)) {
		case 2:
			encoded = (opcode) |
			((instruction->operands[0].value.reg & 0x1F) << 7) |
			((mnemonic_funct3(instruction->mnemonic) & 0x07) << 12) |
			((instruction->operands[1].value.reg & 0x1F) << 15) |
			((instruction->operands[2].value.imm & 0xFFF) << 20);
			break;
		case 1:
			if (instruction->mnemonic == HX_MN_JALR) {
				encoded = (opcode) |
				((instruction->operands[0].value.reg & 0x1F) << 7) |
				((mnemonic_funct3(instruction->mnemonic) & 0x07) << 12) |
				((instruction->operands[1].value.reg & 0x1F) << 15) |
				((instruction->operands[2].value.imm & 0xFFF) << 20);
			} else {
				if (opcode == 0x03) {
					encoded = (opcode) |
					((instruction->operands[0].value.reg & 0x1F) << 7) |
					((mnemonic_funct3(instruction->mnemonic) & 0x07) << 12) |
					((instruction->operands[1].value.memory.reg & 0x1F) << 15) |
					((instruction->operands[1].value.memory.offset & 0xFFF) << 20);
				} else {
					encoded = (opcode) |
					((instruction->operands[0].value.reg & 0x1F) << 7) |
					((mnemonic_funct3(instruction->mnemonic) & 0x07) << 12) |
					((instruction->operands[0].value.reg & 0x1F) << 15) |
					((instruction->operands[1].value.imm & 0xFFF) << 20);
				}
			}
			break;
		case 0: /* TODO: Many immediate for some system instructions */
			encoded = (opcode) |
			((mnemonic_funct3(instruction->mnemonic) &0x07) << 12);
			break;
	}

	return encoded;
}

static u32 encode_s_type(const hx_instruction *instruction)
{
	u32 encoded;
	u8 opcode = mnemonic_opcode(instruction->mnemonic);

	if (instruction->operands[1].type == HX_OPERAND_MEMORY) {
		encoded = (opcode) |
			((mnemonic_funct3(instruction->mnemonic) & 0x07) << 7) |
			((instruction->operands[0].value.reg & 0x1F) << 10) |
			((instruction->operands[1].value.memory.reg & 0x1F) << 15) |
			((instruction->operands[1].value.memory.offset & 0xFFF) << 20);

	} else if (instruction->operands[1].type == HX_OPERAND_REGISTER) {
		encoded = (opcode) |
			((mnemonic_funct3(instruction->mnemonic) & 0x07) << 7) |
			((instruction->operands[0].value.memory.reg & 0x1F) << 10) |
			((instruction->operands[1].value.memory.reg & 0x1F) << 15) |
			((instruction->operands[0].value.memory.offset & 0xFFF) << 20);
	} else {
		__builtin_unreachable();
	}
	return encoded;
}

static u32 encode_b_type(const hx_instruction *instruction, hx_symbol_table *table)
{
	u8 opcode = mnemonic_opcode(instruction->mnemonic);
	u32 address;

	hx_label label = {
		.name = instruction->operands[2].value.label.text,
		.name_length = instruction->operands[2].value.label.length
	};

	if (!symbol_find(table, label, &address)) {
		fprintf(stderr, "instruction targets a nonexisting label\n");
	}

	u32 encoded = (opcode) |
		((mnemonic_funct3(instruction->mnemonic) & 0x07) << 7) |
		((instruction->operands[0].value.reg & 0x1F) << 10) |
		((instruction->operands[1].value.reg & 0x1F) << 15) |
		(((address >> 1) & 0x7FF) << 20);
	
	return encoded;
}

static u32 encode_j_type(const hx_instruction *instruction, hx_symbol_table *table)
{
	u8 opcode = mnemonic_opcode(instruction->mnemonic);
	u32 address;

	hx_label label = {
		.name = instruction->operands[1].value.label.text,
		.name_length = instruction->operands[1].value.label.length
	};

	if (!symbol_find(table, label, &address)) {
		fprintf(stderr, "instruction targets a nonexisting label\n");
	}

	u32 encoded = (opcode) |
		((instruction->operands[0].value.reg & 0x1F) << 7) |
		(((address >> 1) & 0x7FFFF) << 12);
	
	return encoded;
}

static u32 encode_m_type(const hx_instruction *instruction)
{
	u32 encoded;
	u8 opcode = mnemonic_opcode(instruction->mnemonic);

	switch (instruction->mnemonic) {
		case HX_MN_MOVZ:
			encoded = (opcode) |
				((instruction->operands[0].value.reg & 0x1F) << 7) |
				(0x0 << 12) |
				((instruction->operands[2].value.imm & 0x3) << 14) |
				((instruction->operands[1].value.imm & 0xFFFF) << 16);
			break;

		case HX_MN_MOVP:
			encoded = (opcode) |
				((instruction->operands[0].value.reg & 0x1F) << 7) |
				(0x1 << 12) |
				((instruction->operands[2].value.imm & 0x3) << 14) |
				((instruction->operands[1].value.imm & 0xFFFF) << 16);
			break;

		case HX_MN_MOVN:
			encoded = (opcode) |
				((instruction->operands[0].value.reg & 0x1F) << 7) |
				(0x2 << 12) |
				((instruction->operands[2].value.imm & 0x3) << 14) |
				((instruction->operands[1].value.imm & 0xFFFF) << 16);
			break;

		default:
			encoded = 0x0;
	}

	return encoded;
}

static b8 encode_instruction(const hx_instruction *instruction, hx_symbol_table *symbols, hx_binary *binary)
{
	(void)binary;

	u32 inst_bytes = 0;

	/*
	 * TODO: find the correct candidate instruction to write, then format
	 * in u16 or u32, then send to binary_write_byte to write to hx_binary
	 * then sent to writer what writes binary to file
	 */
	switch (instruction->mnemonic) {

		/* R-Type format instructions */
		case HX_MN_SUB:		
		case HX_MN_MUL:		
		case HX_MN_SLT:		
		case HX_MN_DIV:		
		case HX_MN_AND:		
		case HX_MN_REM:		
		case HX_MN_SLTU:	
		case HX_MN_SLL:		
		case HX_MN_SLR:		
		case HX_MN_SAR:		
		case HX_MN_XOR:		
		case HX_MN_OR:		
		case HX_MN_ADD:
			inst_bytes = encode_r_type(instruction);
			break;

		/* I-Type format instructions */
		case HX_MN_ADDI:
		case HX_MN_ANDI:	
		case HX_MN_ORI:		
		case HX_MN_XORI:	
		case HX_MN_SLLI:	
		case HX_MN_SLRI:	
		case HX_MN_SARI:	
		case HX_MN_SLTI:	
		case HX_MN_SLTUI:
		case HX_MN_SCALL:	
		case HX_MN_STRAP:	
		case HX_MN_SRET:	
		case HX_MN_WFI:		
		case HX_MN_JALR:	
		case HX_MN_LB:		
		case HX_MN_LQ:		
		case HX_MN_LH:		
		case HX_MN_LW:		
		case HX_MN_LBU:		
		case HX_MN_LQU:		
		case HX_MN_LHU:		
			inst_bytes = encode_i_type(instruction);
			break;

		case HX_MN_SB:		
		case HX_MN_SQ:		
		case HX_MN_SH:		
		case HX_MN_SW:
			inst_bytes = encode_s_type(instruction);
			break;

		case HX_MN_BEQ:		
		case HX_MN_BNE:		
		case HX_MN_BLT:		
		case HX_MN_BGE:		
		case HX_MN_BLTU:	
		case HX_MN_BGEU:
			inst_bytes = encode_b_type(instruction, symbols);
			break;

		case HX_MN_JAL:
			inst_bytes = encode_j_type(instruction, symbols);
			break;


		case HX_MN_MOVZ:	
		case HX_MN_MOVP:	
		case HX_MN_MOVN:
			inst_bytes = encode_m_type(instruction);
			break;

		case HX_MN_CS:		
		case HX_MN_CSINC:	
		case HX_MN_CSNEG:
			inst_bytes = encode_r_type(instruction);
		
		default:
			break;
	}

	binary_write_byte(binary, (u8)inst_bytes);
	binary_write_byte(binary, (u8)(inst_bytes >> 8));
	binary_write_byte(binary, (u8)(inst_bytes >> 16));
	binary_write_byte(binary, (u8)(inst_bytes >> 24));

	return success;
}

static void symbol_debug(const hx_symbol_table *sym_table, b8 pic)
{
	printf("Symbol Table (%s)\n", pic ? "PIC" : "non-PIC");
	printf("--------------------------------\n");

	for (u32 i = 0; i < sym_table->count; i++) {
		const hx_symbol *symbol =
			&sym_table->symbols[i];

		printf(
			"%.*s:  0x%08X\n",
			(int)symbol->label.name_length,
			symbol->label.name,
			symbol->address
		);
	}

	printf("\n");
}

b8 encoder_encode(const hx_program *program, hx_binary *binary, hx_cli *cli)
{
	hx_symbol_table sym_table;
	symbol_table_init(&sym_table);
	if (!collect_symbols(program, &sym_table))
		return failure;

	if (cli->symbol_debug) {
		symbol_debug(&sym_table, cli->pic);
		return failure;
	}


	for (u32 i = 0; i < program->node_count; i++) {
		switch (program->nodes[i].type) {
			case HX_NODE_LABEL: /* NOTHING */
				break;
			case HX_NODE_INSTRUCTION:
				if (!encode_instruction(&program->nodes[i].value.instruction, &sym_table, binary))
					return failure;
				break;
			case HX_NODE_ASSEMBLER_DIRECTIVE: /* TODO: origin, fill, etc... */
				break;
		}
	}

	(void)program;
	(void)binary;

	return success;
}
