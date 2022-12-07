#include "opcodes.h"

#include "log.h"
#include "utils.h"

#define NEXT 2 /* Go to next instruction. */
#define SKIP 4 /* Skin next instruction. */

uint8_t opcode_LDI(uint16_t *index, uint16_t addr) {
	*index = addr;
	return NEXT;
}
