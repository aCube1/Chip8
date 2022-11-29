#include "cpu.h"

#include "utils.h"

#include <string.h>

#define MEMORY_SIZE     4096
#define STACK_SIZE      16
#define REGISTERS_COUNT 16
#define KEYS_COUNT      16

const uint8_t CPU_font[80] = {
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

static struct {
	uint16_t opcode; /* Current Opcode */
	uint8_t memory[MEMORY_SIZE];
	uint8_t v_register[REGISTERS_COUNT];

	uint16_t stack[STACK_SIZE];
	uint16_t stack_pointer; /* Points to the next empty spot in stack */

	uint16_t index_register; /* Index register */
	/* Points to the next instruction in memory to execute*/
	uint16_t program_counter;

	/* Screen has a total of 2048 pixels, and hold state of 0 or 1 */
	uint8_t gfx[GFX_WIDTH * GFX_HEIGHT];

	/* Timers count at 60Hz. When set above 0, they will count down to 0 */
	uint8_t delay_timer;
	uint8_t sound_timer;

	uint8_t key_state[KEYS_COUNT]; /* HEX based keymap (0x0-0xF) */
} CPU;

static void reset_cpu(void);
static const char *read_rom(const char *filename);

int8_t cpu_init(const char *filename) {
	reset_cpu();

	/* Load the built-in fontset int 0x50-0x0A0 */
	memcpy(CPU.memory + 0x50, CPU_font, 80);

	/* TODO: Read File */
	read_rom(filename);

	return STATUS_OK;
}

static void reset_cpu(void) {
	/* System expects the application to be loaded at memory location 0x200 */
	CPU.program_counter = 0x200;
	CPU.opcode = 0;         /* Reset current opcode */
	CPU.index_register = 0; /* Reset index register */
	CPU.stack_pointer = 0;  /* Reset stack pointer */

	/* Reset timers */
	CPU.delay_timer = 0;
	CPU.sound_timer = 0;

	memset(CPU.memory, 0, MEMORY_SIZE * sizeof(uint8_t)); /* Reset memory */
	memset(CPU.stack, 0, STACK_SIZE * sizeof(uint16_t));  /* Reset stack */
	/* Reset registers */
	memset(CPU.v_register, 0, REGISTERS_COUNT * sizeof(uint8_t));
	/* Reset display */
	memset(CPU.gfx, 0, GFX_WIDTH * GFX_HEIGHT * sizeof(uint8_t));
	/* Reset key states */
	memset(CPU.key_state, 0, KEYS_COUNT * sizeof(uint8_t));
}

static const char *read_rom(const char *filename) {
	(void)filename;
	char *data = NULL;

	/* TODO: Read data from the file and return it. */

	return data;
}
