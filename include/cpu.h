#ifndef _CPU_H_
#define _CPU_H_

#include <SDL_render.h>
#include <stdbool.h>
#include <stdint.h>

/* CPU settings */
#define GFX_WIDTH		64
#define GFX_HEIGHT		32
#define MEMORY_SIZE		0x0FFF /* 4096 bytes */
#define STACK_SIZE		16
#define REGISTERS_COUNT 16
#define KEYS_COUNT		16

typedef struct {
	uint16_t opcode; /* Current Opcode. */
	uint8_t memory[MEMORY_SIZE];
	uint16_t stack[STACK_SIZE];

	/* Registers. */
	uint8_t v_register[REGISTERS_COUNT];
	uint16_t I;	 /* Index register. */
	uint16_t PC; /* Points to the next instruction in memory to execute. */
	uint16_t SP; /* Points to the next empty spot in stack. */

	/* Screen has a total of 2048 pixels, and hold state of 0 or 1. */
	uint8_t gfx[GFX_WIDTH * GFX_HEIGHT];
	SDL_Texture *screen;
	bool gfx_changed;

	/* Timers count at 60Hz. When set above 0, they will count down to 0. */
	uint8_t delay_timer;
	uint8_t sound_timer;

	uint8_t key_state[KEYS_COUNT]; /* HEX based keymap (0x0-0xF) */

	/* Data */
	uint16_t addr;	/* 0nnn */
	uint8_t byte;	/* 0kk0 */
	uint8_t nibble; /* 000n */
	uint8_t x;		/* 0X00 */
	uint8_t y;		/* 00Y0 */
} cpu_t;

/* Reset CPU, create screen and load font to the memory. */
int8_t cpu_init(cpu_t *cpu, SDL_Renderer *renderer);

int8_t cpu_emulate_cycle(cpu_t *cpu); /* Emulate CPU cycle. */
void cpu_reset(cpu_t *cpu);			  /* Set CPU to a initial state. */
void cpu_quit(cpu_t *cpu);			  /* Destroy CPU. */

/* Read rom from filepath and load it to the memory. */
int8_t cpu_loadrom(cpu_t *cpu, const char *filepath);

int8_t cpu_decode_opcode(cpu_t *cpu); /* Decode OPCODE and execute instruction. */

void cpu_update_screen(cpu_t *cpu);

#endif /* _CPU_H_ */
