#ifndef _OPCODES_H_
#define _OPCODES_H_

#include "cpu.h"

#include <stdint.h>

#define MAX_OPCODES 36

typedef uint16_t (*opcode_handler_t)(cpu_t *cpu);

typedef struct {
	opcode_handler_t handler;
	uint16_t opcode;
	uint16_t mask;
} opcode_t;

/* List of opcodes initialized in "opcodes.c" */
extern const opcode_t OPCODES[MAX_OPCODES];

int8_t opcode_decode(cpu_t *cpu);

#endif /* _OPCODES_H_ */
