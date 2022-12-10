#include "opcodes.h"

#include "log.h"
#include "utils.h"

#include <string.h>

#define NEXT_PC 2 /* Go to next instruction. */
#define SKIP_PC 4 /* Skip next instruction. */

/* 0x00E0 - CLS: Clear display. */
static void opcode_CLS(cpu_t *cpu) {
	memset(cpu->gfx, 0, GFX_WIDTH * GFX_HEIGHT * sizeof(uint8_t));
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
	uint8_t reg = cpu->x;
	uint8_t val = cpu->byte;

	cpu->PC = cpu->v_register[reg] == val ? SKIP_PC : NEXT_PC;
}

/* 0x4xkk - SNE: Skip next instruction is Vx != kk.
 * The interpreter compares register Vx to kk, and if they are NOT equal,
 * increments the program counter by 2.
 */
static void opcode_SNE(cpu_t *cpu) {
	uint8_t reg = cpu->x;
	uint8_t val = cpu->byte;

	cpu->PC = cpu->v_register[reg] != val ? SKIP_PC : NEXT_PC;
}

/* 0x5xy0 - SEREG: Skip next instruction if Vx == Vy.
 * The interpreter compares register Vx to register Vy, and if they are equal,
 * increments the program counter by 2.
 */
static void opcode_SEREG(cpu_t *cpu) {
	uint8_t reg_x = cpu->x;
	uint8_t reg_y = cpu->y;

	cpu->PC = cpu->v_register[reg_x] == cpu->v_register[reg_y] ? SKIP_PC : NEXT_PC;
}

/* 0x6xkk - LDIMM: Set Vx = kk.
 * The interpreter puts the value kk into register Vx.
 */
static void opcode_LDIMM(cpu_t *cpu) {
	uint8_t reg = cpu->x;
	uint8_t val = cpu->byte;

	cpu->v_register[reg] = val;
	cpu->PC += NEXT_PC;
}

/* 0xAnnn - LDI: Set I = nnn.
 * The value of register I is set to nnn.
 */
static void opcode_LDI(cpu_t *cpu) {
	cpu->I = cpu->addr;
	cpu->PC += NEXT_PC;
}

/* Generate OPCODES. */
/* clang-format off */
const opcode_t opcodes[MAX_OPCODES] = {
	{ opcode_CLS,   0x00E0, 0xF0FF },
	{ opcode_RET,   0x00EE, 0xF0FF },
	{ opcode_JMP,   0x1000, 0xF000 },
	{ opcode_CALL,  0x2000, 0xF000 },
	{ opcode_SE,    0x3000, 0xF000 },
	{ opcode_SNE,   0x4000, 0xF000 },
	{ opcode_SEREG, 0x5000, 0xF00F },
	{ opcode_LDIMM, 0x6000, 0xF000 },
	{ opcode_LDI,   0xA000, 0xF000 },
	{ NULL,         0x0000, 0x0000 },
};
/* clang-format on */
