#include "disassembler/disassembler.h"
#include "types.h"

#include <isac/instruction.h>
#include <isac/format.h>
#include <isac/operand.h>
#include <isac/mnemonic.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_MEMORY (1024 * 1024 * 50)

b8 disassembler_disassemble(FILE *in)
{
	u32 program_size;
	u64 pc = 0;
	u8 *memory = malloc(MAX_MEMORY);
	if (memory == NULL)
		return failure;

	if (in == NULL) {
		return failure;
	}

	program_size = fread(memory, 1, MAX_MEMORY, in);

	while (pc < program_size) {
		u32 encoded;
		hx_instruction *instruction;

		if (pc > program_size - 4) {
			fprintf(stderr, "Instruction out of bounds: 0x%08lx\n", pc);
			return failure;
		}

		encoded = ((u32)memory[pc]) |
		          ((u32)memory[pc + 1] << 8) |
		          ((u32)memory[pc + 2] << 16) |
		          ((u32)memory[pc + 3] << 24);

		switch ((encoded >> 2) & 0x1F) {
			case 0x00:
				instruction = format_r_decode(encoded);
				printf("0x%08lx: %s", pc, mnemonic_string(instruction_mnemonic(instruction)));
				printf(" h%d, h%d, h%d\n", 
						operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER)),
						operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER)),
						operand_get_register(instruction_get_operand(instruction, 2, HX_REGISTER))
				);
				break;

			case 0x08:
				instruction = format_i_decode(encoded);
				printf("0x%08lx: %s", pc, mnemonic_string(instruction_mnemonic(instruction)));
				printf(" h%d, h%d, %ld\n", 
						operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER)),
						operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER)),
						operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE))
				);
				break;
			case 0x09:
				instruction = format_i_decode(encoded);
				printf("0x%08lx: %s", pc, mnemonic_string(instruction_mnemonic(instruction)));
				printf(" h%d, [h%d %ld\n", 
						operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER)),
						operand_get_memory_register(instruction_get_operand(instruction, 1, HX_MEMORY)),
						operand_get_memory_offset(instruction_get_operand(instruction, 1, HX_MEMORY))
				);
				break;
			case 0x0A:
				instruction = format_i_decode(encoded);
				printf("0x%08lx: %s", pc, mnemonic_string(instruction_mnemonic(instruction)));
				printf(" h%d, h%d, %ld\n", 
						operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER)),
						operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER)),
						operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE))
				);
				break;

			case 0x01:
				instruction = format_s_decode(encoded);
				printf("0x%08lx: %s", pc, mnemonic_string(instruction_mnemonic(instruction)));
				printf(" [h%d %ld], h%d\n", 
						operand_get_memory_register(instruction_get_operand(instruction, 0, HX_MEMORY)),
						operand_get_memory_offset(instruction_get_operand(instruction, 0, HX_MEMORY)),
						operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER))
				);
				break;

			case 0x02:
				instruction = format_j_decode(encoded);
				printf("0x%08lx: %s", pc, mnemonic_string(instruction_mnemonic(instruction)));
				printf(" h%d, %ld\n", 
						operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER)),
						operand_get_immediate(instruction_get_operand(instruction, 1, HX_IMMEDIATE))
				);
				break;

			case 0x10:
				instruction = format_b_decode(encoded);
				printf("0x%08lx: %s", pc, mnemonic_string(instruction_mnemonic(instruction)));
				printf(" h%d, h%d, %ld\n", 
						operand_get_register(instruction_get_operand(instruction, 0, HX_REGISTER)),
						operand_get_register(instruction_get_operand(instruction, 1, HX_REGISTER)),
						operand_get_immediate(instruction_get_operand(instruction, 2, HX_IMMEDIATE))
				);
				break;

			default:
				fprintf(stderr, "Unknown instruction at 0x%08lx\n", pc);
				return failure;
		}

		/* Print/disassemble instruction here. */
		/* e.g. instruction_print(instruction); */


		pc += 4;
	}

	return success;
}
