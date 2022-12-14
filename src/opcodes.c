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
static void opcode_OR(cpu_t *cpu);	   /* 0x8xy1 */
static void opcode_AND(cpu_t *cpu);	   /* 0x8xy2 */
static void opcode_XOR(cpu_t *cpu);	   /* 0x8xy3 */
static void opcode_ADD(cpu_t *cpu);	   /* 0x8xy4 */
static void opcode_SUB(cpu_t *cpu);	   /* 0x8xy5 */
static void opcode_SHR(cpu_t *cpu);	   /* 0x8xy6 */
static void opcode_SUBN(cpu_t *cpu);   /* 0x8xy7 */
static void opcode_SHL(cpu_t *cpu);	   /* 0x8xyE */
static void opcode_SNEREG(cpu_t *cpu); /* 0x9xy0 */
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
	{ opcode_OR,     0x8001, 0xF00F },
	{ opcode_AND,    0x8002, 0xF00F },
	{ opcode_XOR,    0x8003, 0xF00F },
	{ opcode_ADD,    0x8004, 0xF00F },
	{ opcode_SUB,    0x8005, 0xF00F },
	{ opcode_SHR,    0x8006, 0xF00F },
	{ opcode_SUBN,   0x8007, 0xF00F },
	{ opcode_SHL,    0x800E, 0xF00F },
	{ opcode_SNEREG, 0x9000, 0xF000 },
	{ opcode_LDI,    0xA000, 0xF000 },
	{ NULL,          0xB000, 0xF000 },
	{ NULL,          0xC000, 0xF000 },
	{ opcode_DRAW,   0xD000, 0xF000 },
	{ NULL,          0xE09E, 0xF0FF },
	{ NULL,          0xE0A1, 0xF0FF },
	{ NULL,          0xF007, 0xF0FF },
	{ NULL,          0xF00A, 0xF0FF },
	{ NULL,          0xF015, 0xF0FF },
	{ NULL,          0xF018, 0xF0FF },
	{ NULL,          0xF01E, 0xF0FF },
	{ NULL,          0xF029, 0xF0FF },
	{ NULL,          0xF033, 0xF0FF },
	{ NULL,          0xF055, 0xF0FF },
	{ NULL,          0xF065, 0xF0FF },
};
/* clang-format on */

/* Decode current opcode using the mask and execute handler if match. */
int8_t opcode_decode(cpu_t *cpu) {
	for (size_t i = 0; i < MAX_OPCODES; i += 1) {
		opcode_t opcode = opcodes[i];

		if ((cpu->opcode & opcode.mask) == opcode.opcode) {
			/* Execute handler if current opcode match. */
			if (opcode.handler != NULL) {
				opcode.handler(cpu);
			}
			return STATUS_OK;
		}
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
 * increments the program counter by 2, otherwise, increments the program counter by 4.
 */
static void opcode_SE(cpu_t *cpu) {
	const uint8_t val = cpu->byte;
	const uint8_t reg = cpu->v_register[cpu->x];

	cpu->PC = reg == val ? SKIP_PC : NEXT_PC;
}

/* 0x4xkk - SNE: Skip next instruction is Vx != kk.
 * The interpreter compares register Vx to kk, and if they are NOT equal,
 * increments the program counter by 2.
 */
static void opcode_SNE(cpu_t *cpu) {
	const uint8_t val = cpu->byte;
	const uint8_t reg = cpu->v_register[cpu->x];

	cpu->PC = reg != val ? SKIP_PC : NEXT_PC;
}

/* 0x5xy0 - SEREG: Skip next instruction if Vx == Vy.
 * The interpreter compares register Vx to register Vy, and if they are equal,
 * increments the program counter by 2.
 */
static void opcode_SEREG(cpu_t *cpu) {
	const uint8_t reg_x = cpu->v_register[cpu->x];
	const uint8_t reg_y = cpu->v_register[cpu->y];

	cpu->PC = reg_x == reg_y ? SKIP_PC : NEXT_PC;
}

/* 0x6xkk - LDIMM: Set Vx = kk.
 * The interpreter puts the value kk into register Vx.
 */
static void opcode_LDIMM(cpu_t *cpu) {
	const uint8_t val = cpu->byte;
	uint8_t *reg = &cpu->v_register[cpu->x];

	*reg = val;
	cpu->PC += NEXT_PC;
}

/* 0x7xkk - ADDIMM: Set Vx = Vx + kk.
 * The interpreter adds the value kk to the value of register Vx,
 * then stores the result in Vx.
 */
static void opcode_ADDIMM(cpu_t *cpu) {
	const uint8_t val = cpu->byte;
	uint8_t *reg = &cpu->v_register[cpu->x];

	*reg += val;
	cpu->PC += NEXT_PC;
}

/* 0x8xy0 - LDV: Set Vx = Vy.
 * The interpreter stores the value of register Vy in register Vx.
 */
static void opcode_LDV(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];

	*reg_x = reg_y;
	cpu->PC += NEXT_PC;
}

/* 0x8xy1 - OR: Set Vx = Vx OR Vy.
 * The interpreter performs a bitwise OR on the values of Vx and Vy, then stores the
 * result in Vx. A bitwise OR compares the corresponding bits from two values, and if
 * either bit is 1, then the same bit in the result is also 1. Otherwise, it is 0.
 */
static void opcode_OR(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];

	*reg_x |= reg_y;
	cpu->PC += NEXT_PC;
}

/* 0x8xy2 - AND: Set Vx = Vx AND Vy.
 * The interpreter performs a bitwise AND on the values of Vx and Vy, then stores the
 * result in Vx. A bitwise AND compares the corresponding bits from two values, and if
 * both bits are 1, then the same bit in the result is also 1. Otherwise, it is 0.
 */
static void opcode_AND(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];

	*reg_x &= reg_y;
	cpu->PC += NEXT_PC;
}

/* 0x8xy3 - XOR: Set Vx = Vx XOR Vy.
 * The interpreter erforms a bitwise exclusive OR on the values of Vx and Vy, then stores
 * the result in Vx. An exclusive OR compares the corresponding bits from two values, and
 * if the bits are not both the same, then the corresponding bit in the result is set
 * to 1. Otherwise, it is 0.
 */
static void opcode_XOR(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];

	*reg_x ^= reg_y;
	cpu->PC += NEXT_PC;
}

/* 0x8xy4 - ADD: Set Vx = Vx + Vy, set VF = carry.
 * The interpreter adds Vy to the value of Vx, then store result in Vx
 * If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0.
 * Only the lowest 8 bits of the result are kept, and stored in Vx.
 */
static void opcode_ADD(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];
	uint16_t val = *reg_x + reg_y;

	cpu->v_register[0xF] = val > UINT8_MAX; /* Set carry flag. */
	*reg_x += reg_y;
	cpu->PC += NEXT_PC;
}

/* 0x8xy5 - SUB: Set Vx = Vx - Vy, set VF = NOT borrow.
 * If Vx > Vy, then VF is set to 1, otherwise 0.
 * Then Vy is subtracted from Vx, and the results stored in Vx.
 */
static void opcode_SUB(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];

	cpu->v_register[0xF] = *reg_x > reg_y; /* Set not borrow. */
	*reg_x -= reg_y;
	cpu->PC += NEXT_PC;
}

/* 0x8xy6 - SHR: Set Vx = Vx SHR 1.
 * Interpreter set VF to the least-significant bit of Vx. Then Vx is divided by 2.
 * Vy is ignored.
 */
static void opcode_SHR(cpu_t *cpu) {
	uint8_t *reg = &cpu->v_register[cpu->x];

	cpu->v_register[0xF] = *reg & 1;
	*reg >>= 1;
	cpu->PC += NEXT_PC;
}

/* 0x8xy7 - SUBN: Set Vx = Vy - Vx, set VF = NOT borrow.
 * If Vy > Vx, then VF is set to 1, otherwise 0.
 * Then Vx is subtracted from Vy, and the results stored in Vx.
 */
static void opcode_SUBN(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];

	cpu->v_register[0xF] = *reg_x < reg_y; /* Set not borrow. */
	*reg_x = reg_y - (*reg_x);
	cpu->PC += NEXT_PC;
}

/* 0x8xyE - SHL: Set Vx = Vx SHL 1.
 * Interpreter set VF to the most-significant bit of Vx. Then Vx is multiplied by 2.
 * Vy is ignored.
 */
static void opcode_SHL(cpu_t *cpu) {
	uint8_t *reg = &cpu->v_register[cpu->x];

	cpu->v_register[0xF] = (*reg >> 7) & 1;
	*reg <<= 1;
	cpu->PC += NEXT_PC;
}

/* 0x9xy0 - SNEREG: Skip next instruction if Vx != Vy.
 * Interpreter compare Vx and Vy, if they are not equal,
 * the program counter is increased by 2,
 * otherwise, the program counter is increased by 4.
 */
static void opcode_SNEREG(cpu_t *cpu) {
	const uint8_t reg_x = cpu->v_register[cpu->x];
	const uint8_t reg_y = cpu->v_register[cpu->y];

	cpu->PC = reg_x != reg_y ? SKIP_PC : NEXT_PC;
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
