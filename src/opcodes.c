#include "opcodes.h"

#include "cpu.h"
#include "log.h"
#include "utils.h"

int8_t decode_opcode(cpu_t *cpu) {
	switch (cpu->opcode & 0xF000) {
	case 0x0000:
		/* TODO: Manage 0x0FFF instructions */
		break;
	case 0xA000: /* ANNN: Set index register to address NNN. */
		cpu->index_register = cpu->opcode & 0x0FFF;
		cpu->pc += 2;
		break;
	default:
		log_error("Unknown opcode: 0x%X", cpu->opcode);
	}

	return STATUS_OK;
}
