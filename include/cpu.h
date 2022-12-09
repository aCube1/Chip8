#ifndef _CPU_H_
#define _CPU_H_

#include <stdint.h>

/* CPU settings */
#define GFX_WIDTH		64
#define GFX_HEIGHT		32
#define MEMORY_SIZE		0x0FFF /* 4096 bytes */
#define STACK_SIZE		16
#define REGISTERS_COUNT 16
#define KEYS_COUNT		16

typedef struct {
	uint16_t opcode; /* Current Opcode */
	uint8_t memory[MEMORY_SIZE];
	uint8_t v_register[REGISTERS_COUNT];

	uint16_t stack[STACK_SIZE];
	uint16_t sp; /* Points to the next empty spot in stack */

	uint16_t index_register; /* Index register */
	uint16_t pc;			 /* Points to the next instruction in memory to execute */

	/* Screen has a total of 2048 pixels, and hold state of 0 or 1 */
	uint8_t gfx[GFX_WIDTH * GFX_HEIGHT];

	/* Timers count at 60Hz. When set above 0, they will count down to 0 */
	uint8_t delay_timer;
	uint8_t sound_timer;

	uint8_t key_state[KEYS_COUNT]; /* HEX based keymap (0x0-0xF) */
} cpu_t;

int8_t cpu_init(cpu_t *cpu);
int8_t cpu_emulate_cycle(cpu_t *cpu);

/* Read rom from filepath and load it to the memory. */
int8_t cpu_loadrom(cpu_t *cpu, const char *filepath);
/* Decode OPCODE and execute instruction. */
int8_t cpu_decode_opcode(cpu_t *cpu);

#endif /* _CPU_H_ */
