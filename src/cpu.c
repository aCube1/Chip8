#include "cpu.h"

#include "log.h"
#include "opcodes.h"
#include "utils.h"

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

int8_t cpu_init(cpu_t *cpu, const char *rom_content) {
	(void)rom_content; /* TODO: Read ROM and load it to memory */
	reset_cpu(cpu);

	/* Load the built-in fontset int 0x50-0x0A0 */
	memcpy(cpu->memory + 0x50, CPU_font, 80 * sizeof(uint8_t));

	return STATUS_OK;
}

int8_t cpu_emulate_cycle(cpu_t *cpu) {
	/* Fetch opcode */
	cpu->opcode = cpu->memory[cpu->pc] << 8 | cpu->memory[cpu->pc + 1];

	decode_opcode(cpu);

	update_timers(cpu);

	return STATUS_OK;
}

static void reset_cpu(cpu_t *cpu) {
	/* System expects the application to be loaded at memory location 0x200 */
	cpu->pc = 0x200;
	cpu->opcode = 0;         /* Reset current opcode */
	cpu->index_register = 0; /* Reset index register */
	cpu->sp = 0;             /* Reset stack pointer */

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
