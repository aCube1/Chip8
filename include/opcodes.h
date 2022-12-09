#ifndef _OPCODES_H_
#define _OPCODES_H_

#include "cpu.h"

#include <stdint.h>

#define NEXT_PC 2 /* Go to next instruction. */
#define SKIP_PC 4 /* Skip next instruction. */

void opcode_CLS(cpu_t *cpu);   /* 0x00E0 */
void opcode_RET(cpu_t *cpu);   /* 0x00EE */
void opcode_JMP(cpu_t *cpu);   /* 0x1nnn */
void opcode_CALL(cpu_t *cpu);  /* 0x2nnn */
void opcode_SE(cpu_t *cpu);	   /* 0x3xkk */
void opcode_SNE(cpu_t *cpu);   /* 0x4xkk */
void opcode_SEREG(cpu_t *cpu); /* 0x5xy0 */
void opcode_LDIMM(cpu_t *cpu); /* 0x6xkk */
void opcode_LDI(cpu_t *cpu);   /* 0xAnnn */

#endif /* _OPCODES_H_ */
