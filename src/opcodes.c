#include "opcodes.h"

#include "cpu.h"
#include "log.h"
#include "utils.h"

#include <string.h>

/* 0x00E0 - CLS: Clear display. */
void opcode_CLS(cpu_t *cpu) {
	memset(cpu->gfx, 0, GFX_WIDTH * GFX_HEIGHT * sizeof(uint8_t));
	cpu->pc += NEXT_PC;
}

/* 0x00EE - RET: Return from a subroutine.
 * The interpreter sets the program counter to the address at the top of the stack,
 * then subtracts 1 from the stack pointer.
 */
void opcode_RET(cpu_t *cpu) {
	cpu->pc = cpu->stack[cpu->sp] + 2;
	cpu->sp -= 1;
}

/* 0x1nnn - JMP: Jump to location nnn.
 * The interpreter sets the program counter to nnn.
 */
void opcode_JMP(cpu_t *cpu) {
	cpu->pc = cpu->addr;
}

/* 0x2nnn - CALL: Call subroutine at nnn.
 * The interpreter increments the stack pointer,
 * then puts the current PC on the top of the stack.
 * The PC is then set to nnn.
 */
void opcode_CALL(cpu_t *cpu) {
	cpu->stack[cpu->sp + 1] = cpu->pc;
	cpu->sp += 1;
	cpu->pc = cpu->addr;
}

/* 0x3xkk - SE: Skip next instruction if Vx == kk.
 * The interpreter compares register Vx to kk, and if they are equal,
 * increments the program counter by 2.
 */
void opcode_SE(cpu_t *cpu) {
	uint8_t reg = cpu->x;
	uint8_t val = cpu->byte;

	cpu->pc = cpu->v_register[reg] == val ? SKIP_PC : NEXT_PC;
}

/* 0x4xkk - SNE: Skip next instruction is Vx != kk.
 * The interpreter compares register Vx to kk, and if they are NOT equal,
 * increments the program counter by 2.
 */
void opcode_SNE(cpu_t *cpu) {
	uint8_t reg = cpu->x;
	uint8_t val = cpu->byte;

	cpu->pc = cpu->v_register[reg] != val ? SKIP_PC : NEXT_PC;
}

/* 0x5xy0 - SEREG: Skip next instruction if Vx == Vy.
 * The interpreter compares register Vx to register Vy, and if they are equal,
 * increments the program counter by 2.
 */
void opcode_SEREG(cpu_t *cpu) {
	uint8_t reg_x = cpu->x;
	uint8_t reg_y = cpu->y;

	cpu->pc = cpu->v_register[reg_x] == cpu->v_register[reg_y] ? SKIP_PC : NEXT_PC;
}

/* 0x6xkk - LDIMM: Set Vx = kk.
 * The interpreter puts the value kk into register Vx.
 */
void opcode_LDIMM(cpu_t *cpu) {
	uint8_t reg = cpu->x;
	uint8_t val = cpu->byte;

	cpu->v_register[reg] = val;
	cpu->pc += NEXT_PC;
}

/* 0xAnnn - LDI: Set I = nnn.
 * The value of register I is set to nnn.
 */
void opcode_LDI(cpu_t *cpu) {
	cpu->index_register = cpu->addr;
	cpu->pc += NEXT_PC;
}
