#include "cpu.h"

#include "log.h"
#include "opcodes.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

static void reset_cpu(cpu_t *cpu);
static void update_timers(cpu_t *cpu);

static const uint8_t CPU_font[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
	0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
	0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
	0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
	0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
	0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
	0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
	0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
	0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
	0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
	0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
	0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
	0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
	0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
	0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
	0xF0, 0x80, 0xF0, 0x80, 0x80, /* F */
};

int8_t cpu_init(cpu_t *cpu) {
	reset_cpu(cpu);

	/* Load the built-in fontset in 0x50-0x0A0 */
	memcpy(cpu->memory + 0x50, CPU_font, 80 * sizeof(uint8_t));

	return STATUS_OK;
}

int8_t cpu_emulate_cycle(cpu_t *cpu) {
	update_timers(cpu);
	/* Fetch opcode */
	cpu->opcode = cpu->memory[cpu->PC] << 8 | cpu->memory[cpu->PC + 1];

	return cpu_decode_opcode(cpu);
}

int8_t cpu_loadrom(cpu_t *cpu, const char *filepath) {
	const uint32_t max_rom_size = MEMORY_SIZE - 0x200; /* 3584 bytes */
	FILE *rom = fopen(filepath, "rb");
	file_t file;

	if (rom == NULL || get_file_content(&file, rom) != STATUS_OK) {
		log_error("Unable to read rom!");
		return STATUS_ERROR;
	}

	if (file.lenght > max_rom_size) {
		log_error("Rom file size is bigger than max rom size!");
		return STATUS_ERROR;
	}

	/* Load ROM to memory */
	memcpy(cpu->memory + 0x200, file.content, file.lenght * sizeof(uint8_t));
	log_info("Loaded %s with %d bytes to memory.", filepath, file.lenght);

	file_free(&file); /* Always free allocated memory. */
	return STATUS_OK;
}

int8_t cpu_decode_opcode(cpu_t *cpu) {
	cpu->addr = cpu->opcode & 0x0FFF;
	cpu->byte = cpu->opcode & 0x00FF;
	cpu->nibble = cpu->opcode & 0x000F;
	cpu->x = (cpu->opcode & 0x0F00) >> 8;
	cpu->y = (cpu->opcode & 0x00F0) >> 4;

	for (size_t i = 0; i < MAX_OPCODES; i += 1) {
		opcode_t opcode = opcodes[i];

		if (opcode.handler == NULL) {
			return STATUS_OK; /* If handler not exists, stop function. */
		} else if ((cpu->opcode & opcode.mask) != opcode.opcode) {
			continue; /* Go to next opcode if current opcode doesn't match. */
		}

		/* Execute handler if current opcode match. */
		opcode.handler(cpu);
		log_debug("OPCODE: %04X", opcode.opcode);
		return STATUS_OK;
	}

	log_error("Unknown opcode: 0x%X", cpu->opcode);
	return STATUS_ERROR;
}

static void reset_cpu(cpu_t *cpu) {
	/* System expects the application to be loaded at memory location 0x200 */
	cpu->PC = 0x200;
	cpu->opcode = 0; /* Reset current opcode */
	cpu->I = 0;		 /* Reset index register */
	cpu->SP = 0;	 /* Reset stack pointer */

	/* Reset timers */
	cpu->delay_timer = 0;
	cpu->sound_timer = 0;

	memset(cpu->memory, 0, MEMORY_SIZE * sizeof(uint8_t)); /* Reset memory */
	memset(cpu->stack, 0, STACK_SIZE * sizeof(uint16_t));  /* Reset stack */
	/* Reset registers */
	memset(cpu->v_register, 0, REGISTERS_COUNT * sizeof(uint8_t));
	/* Reset display */
	memset(cpu->gfx, 0, GFX_WIDTH * GFX_HEIGHT * sizeof(uint8_t));
	/* Reset key states */
	memset(cpu->key_state, 0, KEYS_COUNT * sizeof(uint8_t));
}

static void update_timers(cpu_t *cpu) {
	if (cpu->delay_timer > 0) {
		cpu->delay_timer -= 1;
	}
	if (cpu->sound_timer > 0) {
		cpu->sound_timer -= 1;
	}
}
