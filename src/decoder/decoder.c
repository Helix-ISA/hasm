#include "decoder/decoder.h"
#include "isa/mnemonic.h"
#include "types.h"
#include <stdio.h>

static void decode_r_type(u32 instruction)
{
	u8 opcode = (instruction & 0x7F);
	u8 funct3 = (instruction >> 12) & 0x7;
	u8 rd = (instruction >> 7) & 0x1F;
	u8 rs1 = (instruction >> 15) & 0x1F;
	u8 rs2 = (instruction >> 20) & 0x1F;
	u8 funct7 = (instruction >> 25) & 0x7F;

	hx_mnemonic mnemonic = mnemonic_from_encoding(opcode, funct3, funct7);

	if (opcode == 0x77) {
		switch (funct7) {
			case 0x00:
				printf("%s r%u, r%u, r%u, %s\n", mnemonic_name(mnemonic), rd, rs1, rs2, "eq");
				break;
			case 0x01:
				printf("%s r%u, r%u, r%u, %s\n", mnemonic_name(mnemonic), rd, rs1, rs2, "ne");
				break;
			case 0x02:
				printf("%s r%u, r%u, r%u, %s\n", mnemonic_name(mnemonic), rd, rs1, rs2, "lt");
				break;
			case 0x03:
				printf("%s r%u, r%u, r%u, %s\n", mnemonic_name(mnemonic), rd, rs1, rs2, "ge");
				break;
			case 0x04:
				printf("%s r%u, r%u, r%u, %s\n", mnemonic_name(mnemonic), rd, rs1, rs2, "ltu");
				break;
			case 0x05:
				printf("%s r%u, r%u, r%u, %s\n", mnemonic_name(mnemonic), rd, rs1, rs2, "geu");
				break;
		}
	} else {
		printf("%s r%u, r%u, r%u\n", mnemonic_name(mnemonic), rd, rs1, rs2);
	}
}

static void decode_i_type(u32 instruction)
{
	u8 opcode = (instruction & 0x7F);
	u8 funct3 = (instruction >> 12) & 0x7;
	u8 rd = (instruction >> 7) & 0x1F;
	u8 rs1 = (instruction >> 15) & 0x1F;
	s16 imm = (instruction >> 20) & 0xFFF;

	hx_mnemonic mnemonic = mnemonic_from_encoding(opcode, funct3, 0);
	
	if (opcode == 0x03) {
		printf("%s r%u, [r%u + %d]\n", mnemonic_name(mnemonic), rd, rs1, imm);
	} else if(opcode == 0x63) {
		printf("%s\n", mnemonic_name(mnemonic));
	} else {
		printf("%s r%u, r%u, %d\n", mnemonic_name(mnemonic), rd, rs1, imm);
	}
}

static void decode_s_type(u32 instruction)
{
	u8 opcode = (instruction & 0x7F);
	u8 funct3 = (instruction >> 7) & 0x7;
	u8 rs1 = (instruction >> 10) & 0x1F;
	u8 rs2 = (instruction >> 15) & 0x1F;
	s16 imm = (instruction >> 20) & 0xFFF;

	hx_mnemonic mnemonic = mnemonic_from_encoding(opcode, funct3, 0);

	printf("%s [r%u + %d], r%u\n", mnemonic_name(mnemonic), rs1, imm, rs2);
}

static void decode_b_type(u32 instruction)
{
	u8 opcode = (instruction & 0x7F);
	u8 funct3 = (instruction >> 7) & 0x7;
	u8 rs1 = (instruction >> 10) & 0x1F;
	u8 rs2 = (instruction >> 15) & 0x1F;
	s16 imm = (instruction >> 20) & 0xFFF;

	hx_mnemonic mnemonic = mnemonic_from_encoding(opcode, funct3, 0);

	printf("%s r%u, r%u, %d\n", mnemonic_name(mnemonic), rs1, rs2, imm << 1);
}

static void decode_j_type(u32 instruction)
{
	u8 rd = (instruction >> 7) & 0x1F;
	s16 imm = (instruction >> 12) & 0x7FFFF;

	printf("jal r%u, %d\n", rd, imm << 1);
}

static void decode_m_type(u32 instruction)
{
	u8 rd = (instruction >> 7) & 0x1F;
	u8 mod = (instruction >> 12) & 0x3;
	u8 shift = (instruction >> 14) & 0x3;
	u16 imm = (instruction >> 16) & 0xFFFF;

	switch (mod) {
		case 0x0:
			printf("%s r%u, %d, %d\n", "movz", rd, imm, shift);
			break;
		case 0x1:
			printf("%s r%u, %d, %d\n", "movp", rd, imm, shift);
			break;
		case 0x2:
			printf("%s r%u, %d, %d\n", "movn", rd, imm, shift);
		case 0x3:
			printf("%s r%u, %d\n", "mov", rd, imm);
			break;
	}

}

b8 decoded_disassemble(const char *filename)
{
	u32 instruction;

	FILE *file = fopen(filename, "rb");
	while (fread(&instruction, sizeof(u32), 1, file) == 1) {
		u8 opcode = instruction & 0x7F;

		switch (opcode) {
			case 0x27:
			case 0x77:
				decode_r_type(instruction);
				break;

			case 0x07:
			case 0x03:
			case 0x63:
			case 0x73:
				decode_i_type(instruction);
				break;

			case 0x23:
				decode_s_type(instruction);
				break;

			case 0x4F:
				decode_b_type(instruction);
				break;

			case 0x6F:
				decode_j_type(instruction);
				break;

			case 0x43:
				decode_m_type(instruction);
				break;

			default:
				printf("unknown opcode: %i\n", opcode);
				return failure;
		}
	}

	return success;
}
