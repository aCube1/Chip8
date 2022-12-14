#ifndef _CPU_H_
#define _CPU_H_

#include <SDL_render.h>
#include <stdbool.h>
#include <stdint.h>

/* CPU settings */
#define GFX_WIDTH		  64
#define GFX_HEIGHT		  32
#define RAM_SIZE		  0x0FFF /* 4096 bytes */
#define STACK_SIZE		  16
#define V_REGISTERS_COUNT 16
#define KEYS_COUNT		  16

#define FONT_ADDRESS	  0x50 /* Address to load font. */
#define FONT_CHAR_COUNT	  16
#define FONT_CHAR_SIZE	  5

#define TIMER_CLOCK_SPEED 60 /* Time clock speed in Hz. */

typedef struct {
	uint16_t opcode; /* Current Opcode. */
	uint8_t memory[RAM_SIZE];
	uint16_t stack[STACK_SIZE];

	/* Registers. */
	uint8_t V[V_REGISTERS_COUNT];
	uint16_t I;	 /* Index register. */
	uint16_t PC; /* Points to the next instruction in memory to execute. */
	uint16_t SP; /* Points to the next empty spot in stack. */

	/* Screen has a total of 2048 pixels, and hold state of 0 or 1. */
	uint8_t gfx[GFX_WIDTH * GFX_HEIGHT];
	bool has_gfx_changed;

	/* Timers count at 60Hz. When set above 0, they will count down to 0. */
	uint8_t delay_timer;
	uint8_t sound_timer;

	uint8_t key_state[KEYS_COUNT]; /* HEX based keymap (0x0-0xF) */

	uint16_t clock_speed; /* CPU clock speed for executing code. */

	/* Data */
	uint16_t addr;	/* 0nnn */
	uint8_t byte;	/* 00kk */
	uint8_t nibble; /* 000n */
	uint8_t x;		/* 0X00 */
	uint8_t y;		/* 00Y0 */
} cpu_t;

/* Reset CPU and load font to the memory. */
int8_t cpu_init(cpu_t *cpu, uint16_t clock_speed);

/* Do cpu cycles and update timers. */
int8_t cpu_update(cpu_t *cpu);
void cpu_reset(cpu_t *cpu); /* Set CPU to a initial state. */

/* Read rom from filepath and load it to the memory. */
int8_t cpu_loadrom(cpu_t *cpu, const char *filepath);

int8_t cpu_decode_opcode(cpu_t *cpu); /* Decode OPCODE and execute instruction. */

#endif /* _CPU_H_ */
