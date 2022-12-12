#include "opcodes.h"

#include "log.h"
#include "utils.h"

#include <string.h>

#define NEXT_PC 2 /* Go to next instruction. */
#define SKIP_PC 4 /* Skip next instruction. */

static void opcode_CLS(cpu_t *cpu);	   /* 0x00E0 */
static void opcode_RET(cpu_t *cpu);	   /* 0x00EE */
static void opcode_JMP(cpu_t *cpu);	   /* 0x1nnn */
static void opcode_CALL(cpu_t *cpu);   /* 0x2nnn */
static void opcode_SE(cpu_t *cpu);	   /* 0x3xkk */
static void opcode_SNE(cpu_t *cpu);	   /* 0x4xkk */
static void opcode_SEREG(cpu_t *cpu);  /* 0x5xy0 */
static void opcode_LDIMM(cpu_t *cpu);  /* 0x6xkk */
static void opcode_ADDIMM(cpu_t *cpu); /* 0x7xkk */
static void opcode_LDV(cpu_t *cpu);	   /* 0x8xy0 */
static void opcode_LDI(cpu_t *cpu);	   /* 0xAnnn */
static void opcode_DRAW(cpu_t *cpu);   /* 0xDxyn */

/* Generate OPCODES. */
/* clang-format off */
const opcode_t opcodes[MAX_OPCODES] = {
	{ opcode_CLS,    0x00E0, 0xF0FF },
	{ opcode_RET,    0x00EE, 0xF0FF },
	{ opcode_JMP,    0x1000, 0xF000 },
	{ opcode_CALL,   0x2000, 0xF000 },
	{ opcode_SE,     0x3000, 0xF000 },
	{ opcode_SNE,    0x4000, 0xF000 },
	{ opcode_SEREG,  0x5000, 0xF00F },
	{ opcode_LDIMM,  0x6000, 0xF000 },
	{ opcode_ADDIMM, 0x7000, 0xF000 },
	{ opcode_LDV,    0x8000, 0xF00F },
	{ opcode_LDI,    0xA000, 0xF000 },
	{ opcode_DRAW,   0xD000, 0xF000 },
	{ NULL,          0x0000, 0x0000 },
};
/* clang-format on */

/* Decode current opcode using the mask and execute handler if match. */
int8_t opcode_decode(cpu_t *cpu) {
	for (size_t i = 0; i < MAX_OPCODES; i += 1) {
		opcode_t opcode = opcodes[i];

		if (opcode.handler == NULL) {
			return STATUS_OK; /* If handler not exists, stop function. */
		} else if ((cpu->opcode & opcode.mask) != opcode.opcode) {
			continue; /* Go to next opcode if current opcode doesn't match. */
		}

		/* Execute handler if current opcode match. */
		opcode.handler(cpu);
		return STATUS_OK;
	}

	log_error("Unknown opcode: 0x%X", cpu->opcode);
	return STATUS_ERROR;
}

/* 0x00E0 - CLS: Clear display. */
static void opcode_CLS(cpu_t *cpu) {
	memset(cpu->gfx, 0, GFX_WIDTH * GFX_HEIGHT * sizeof(uint8_t));
	cpu->gfx_changed = true;
	cpu->PC += NEXT_PC;
}

/* 0x00EE - RET: Return from a subroutine.
 * The interpreter sets the program counter to the address at the top of the stack,
 * then subtracts 1 from the stack pointer.
 */
static void opcode_RET(cpu_t *cpu) {
	cpu->SP -= 1;
	cpu->PC = cpu->stack[cpu->SP] + 2;
}

/* 0x1nnn - JMP: Jump to location nnn.
 * The interpreter sets the program counter to nnn.
 */
static void opcode_JMP(cpu_t *cpu) {
	cpu->PC = cpu->addr;
}

/* 0x2nnn - CALL: Call subroutine at nnn.
 * The interpreter increments the stack pointer,
 * then puts the current PC on the top of the stack.
 * The PC is then set to nnn.
 */
static void opcode_CALL(cpu_t *cpu) {
	cpu->stack[cpu->SP] = cpu->PC;
	cpu->SP += 1;
	cpu->PC = cpu->addr;
}

/* 0x3xkk - SE: Skip next instruction if Vx == kk.
 * The interpreter compares register Vx to kk, and if they are equal,
 * increments the program counter by 2.
 */
static void opcode_SE(cpu_t *cpu) {
	const uint8_t reg = cpu->x;
	const uint8_t val = cpu->byte;

	cpu->PC = cpu->v_register[reg] == val ? SKIP_PC : NEXT_PC;
}

/* 0x4xkk - SNE: Skip next instruction is Vx != kk.
 * The interpreter compares register Vx to kk, and if they are NOT equal,
 * increments the program counter by 2.
 */
static void opcode_SNE(cpu_t *cpu) {
	const uint8_t reg = cpu->x;
	const uint8_t val = cpu->byte;

	cpu->PC = cpu->v_register[reg] != val ? SKIP_PC : NEXT_PC;
}

/* 0x5xy0 - SEREG: Skip next instruction if Vx == Vy.
 * The interpreter compares register Vx to register Vy, and if they are equal,
 * increments the program counter by 2.
 */
static void opcode_SEREG(cpu_t *cpu) {
	const uint8_t reg_x = cpu->x;
	const uint8_t reg_y = cpu->y;

	cpu->PC = cpu->v_register[reg_x] == cpu->v_register[reg_y] ? SKIP_PC : NEXT_PC;
}

/* 0x6xkk - LDIMM: Set Vx = kk.
 * The interpreter puts the value kk into register Vx.
 */
static void opcode_LDIMM(cpu_t *cpu) {
	const uint8_t reg = cpu->x;
	const uint8_t val = cpu->byte;

	cpu->v_register[reg] = val;
	cpu->PC += NEXT_PC;
}

/* 0x7xkk - ADDIMM: Set Vx = Vx + kk.
 * Adds the value kk to the value of register Vx, then stores the result in Vx.
 */
static void opcode_ADDIMM(cpu_t *cpu) {
	const uint8_t reg = cpu->x;
	const uint8_t val = cpu->byte;

	cpu->v_register[reg] += val;
	cpu->PC += NEXT_PC;
}

/* 0x8xy0 - LDV: Set Vx = Vy.
 * Stores the value of register Vy in register Vx.
 */
static void opcode_LDV(cpu_t *cpu) {
	const uint8_t reg_x = cpu->x;
	const uint8_t reg_y = cpu->y;

	cpu->v_register[reg_x] = cpu->v_register[reg_y];
}

/* 0xAnnn - LDI: Set I = nnn.
 * The value of register I is set to nnn.
 */
static void opcode_LDI(cpu_t *cpu) {
	cpu->I = cpu->addr;
	cpu->PC += NEXT_PC;
}

/* 0xDxyn - DRAW: Display n-byte sprite starting at memory location I at (Vx, Vy),
 * set VF = collision.
 * The interpreter reads n bytes from memory, starting at the address stored in I.
 * These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
 * Sprites are XORed onto the existing screen.
 * If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
 * If the sprite is positioned so part of it is outside the coordinates of the display,
 * it wraps around to the opposite side of the screen.
 */
static void opcode_DRAW(cpu_t *cpu) {
	const uint8_t height = cpu->nibble;

	/* Wrap if going beyond screen boundaries. */
	const uint8_t x_pos = cpu->v_register[cpu->x] % GFX_WIDTH;
	const uint8_t y_pos = cpu->v_register[cpu->y] % GFX_HEIGHT;

	cpu->v_register[0xF] = 0; /* Set pixel erased flag to 0. */
	for (uint32_t h = 0; h < height; h += 1) {
		const uint8_t sprite_byte = cpu->memory[cpu->I + h];

		for (uint32_t w = 0; w < 8; w += 1) {
			uint8_t pixel = (sprite_byte >> (7 - w)) & 1; /* Get pixel data. */
			uint8_t *gfx_pixel = &cpu->gfx[(y_pos + h) * GFX_WIDTH + (x_pos + w)];

			if (pixel == 0 && *gfx_pixel == 0) {
				cpu->v_register[0xF] = 1; /* Pixel erased. Set flag to 1. */
			}
			*gfx_pixel ^= pixel; /* Effectively XOR with the sprite pixel */
		}
	}

	cpu->gfx_changed = true;
	cpu->PC += NEXT_PC;
}
