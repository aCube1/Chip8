#include "opcodes.h"

#include "cpu.h"
#include "log.h"
#include "utils.h"

#include <string.h>

void opcode_CLS(cpu_t *cpu) {
	memset(cpu->gfx, 0, GFX_WIDTH * GFX_HEIGHT * sizeof(uint8_t));
	cpu->pc += NEXT_PC;
}

void opcode_RET(cpu_t *cpu) {
	cpu->pc = cpu->stack[cpu->sp] + 2;
	cpu->sp -= 1;
}

void opcode_JMP(cpu_t *cpu, uint16_t addr) {
	cpu->pc = addr;
}

void opcode_CALL(cpu_t *cpu, uint16_t addr) {
	cpu->stack[cpu->sp + 1] = cpu->pc;
	cpu->sp += 1;
	cpu->pc = addr;
}

void opcode_SE(cpu_t *cpu, uint8_t reg, uint8_t val) {
	cpu->pc = cpu->v_register[reg] == val ? SKIP_PC : NEXT_PC;
}

void opcode_SNE(cpu_t *cpu, uint8_t reg, uint8_t val) {
	cpu->pc = cpu->v_register[reg] != val ? SKIP_PC : NEXT_PC;
}

void opcode_SEREG(cpu_t *cpu, uint8_t reg_x, uint8_t reg_y) {
	cpu->pc = cpu->v_register[reg_x] == cpu->v_register[reg_y] ? SKIP_PC : NEXT_PC;
}

void opcode_LDIMM(cpu_t *cpu, uint8_t reg, uint8_t val) {
	cpu->v_register[reg] = val;
	cpu->pc += NEXT_PC;
}

void opcode_LDI(cpu_t *cpu, uint16_t addr) {
	cpu->index_register = addr;
	cpu->pc += NEXT_PC;
}
