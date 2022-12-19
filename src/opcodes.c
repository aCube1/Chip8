#include "opcodes.h"

#include "cpu.h"
#include "log.h"
#include "utils.h"

#include <string.h>

#define NEXT_PC cpu->PC + 2 /* Go to next instruction. */
#define SKIP_PC cpu->PC + 4 /* Skip next instruction. */

/* Set true if error occurried. */
static bool has_error = false;

static uint16_t opcode_CLS(cpu_t *cpu);		 /* 0x00E0 */
static uint16_t opcode_RET(cpu_t *cpu);		 /* 0x00EE */
static uint16_t opcode_JMP(cpu_t *cpu);		 /* 0x1nnn */
static uint16_t opcode_CALL(cpu_t *cpu);	 /* 0x2nnn */
static uint16_t opcode_SE(cpu_t *cpu);		 /* 0x3xkk */
static uint16_t opcode_SNE(cpu_t *cpu);		 /* 0x4xkk */
static uint16_t opcode_SEREG(cpu_t *cpu);	 /* 0x5xy0 */
static uint16_t opcode_LDIMM(cpu_t *cpu);	 /* 0x6xkk */
static uint16_t opcode_ADDIMM(cpu_t *cpu);	 /* 0x7xkk */
static uint16_t opcode_LDV(cpu_t *cpu);		 /* 0x8xy0 */
static uint16_t opcode_OR(cpu_t *cpu);		 /* 0x8xy1 */
static uint16_t opcode_AND(cpu_t *cpu);		 /* 0x8xy2 */
static uint16_t opcode_XOR(cpu_t *cpu);		 /* 0x8xy3 */
static uint16_t opcode_ADD(cpu_t *cpu);		 /* 0x8xy4 */
static uint16_t opcode_SUB(cpu_t *cpu);		 /* 0x8xy5 */
static uint16_t opcode_SHR(cpu_t *cpu);		 /* 0x8xy6 */
static uint16_t opcode_SUBN(cpu_t *cpu);	 /* 0x8xy7 */
static uint16_t opcode_SHL(cpu_t *cpu);		 /* 0x8xyE */
static uint16_t opcode_SNEREG(cpu_t *cpu);	 /* 0x9xy0 */
static uint16_t opcode_LDI(cpu_t *cpu);		 /* 0xAnnn */
static uint16_t opcode_JMPREG(cpu_t *cpu);	 /* 0xBnnn */
static uint16_t opcode_RAND(cpu_t *cpu);	 /* 0xCxkk */
static uint16_t opcode_DRAW(cpu_t *cpu);	 /* 0xDxyn */
static uint16_t opcode_SKEY(cpu_t *cpu);	 /* 0xEx9E */
static uint16_t opcode_SNKEY(cpu_t *cpu);	 /* 0xExA1 */
static uint16_t opcode_RDELAY(cpu_t *cpu);	 /* 0xFx07 */
static uint16_t opcode_WAITKEY(cpu_t *cpu);	 /* 0xFx0A */
static uint16_t opcode_WDELAY(cpu_t *cpu);	 /* 0xFx15 */
static uint16_t opcode_WSOUND(cpu_t *cpu);	 /* 0xFx18 */
static uint16_t opcode_ADDI(cpu_t *cpu);	 /* 0xFx1E */
static uint16_t opcode_LDSPRITE(cpu_t *cpu); /* 0xFx29 */
static uint16_t opcode_STBCD(cpu_t *cpu);	 /* 0xFx33 */
static uint16_t opcode_STREG(cpu_t *cpu);	 /* 0xFx55 */
static uint16_t opcode_LDREG(cpu_t *cpu);	 /* 0xFx65 */

/* Generate OPCODES. */
/* clang-format off */
const opcode_t OPCODES[MAX_OPCODES] = {
	{ opcode_CLS,      0x00E0, 0xF0FF },
	{ opcode_RET,      0x00EE, 0xF0FF },
	{ opcode_JMP,      0x1000, 0xF000 },
	{ opcode_CALL,     0x2000, 0xF000 },
	{ opcode_SE,       0x3000, 0xF000 },
	{ opcode_SNE,      0x4000, 0xF000 },
	{ opcode_SEREG,    0x5000, 0xF00F },
	{ opcode_LDIMM,    0x6000, 0xF000 },
	{ opcode_ADDIMM,   0x7000, 0xF000 },
	{ opcode_LDV,      0x8000, 0xF00F },
	{ opcode_OR,       0x8001, 0xF00F },
	{ opcode_AND,      0x8002, 0xF00F },
	{ opcode_XOR,      0x8003, 0xF00F },
	{ opcode_ADD,      0x8004, 0xF00F },
	{ opcode_SUB,      0x8005, 0xF00F },
	{ opcode_SHR,      0x8006, 0xF00F },
	{ opcode_SUBN,     0x8007, 0xF00F },
	{ opcode_SHL,      0x800E, 0xF00F },
	{ opcode_SNEREG,   0x9000, 0xF000 },
	{ opcode_LDI,      0xA000, 0xF000 },
	{ opcode_JMPREG,   0xB000, 0xF000 },
	{ opcode_RAND,     0xC000, 0xF000 },
	{ opcode_DRAW,     0xD000, 0xF000 },
	{ opcode_SKEY,     0xE09E, 0xF0FF },
	{ opcode_SNKEY,    0xE0A1, 0xF0FF },
	{ opcode_RDELAY,   0xF007, 0xF0FF },
	{ opcode_WAITKEY,  0xF00A, 0xF0FF },
	{ opcode_WDELAY,   0xF015, 0xF0FF },
	{ opcode_WSOUND,   0xF018, 0xF0FF },
	{ opcode_ADDI,     0xF01E, 0xF0FF },
	{ opcode_LDSPRITE, 0xF029, 0xF0FF },
	{ opcode_STBCD,    0xF033, 0xF0FF },
	{ opcode_STREG,    0xF055, 0xF0FF },
	{ opcode_LDREG,    0xF065, 0xF0FF },
};
/* clang-format on */

/* Decode current opcode using the mask and execute handler if match. */
int8_t opcode_decode(cpu_t *cpu) {
	has_error = false; /* Reset error. */

	for (size_t i = 0; i < MAX_OPCODES; i += 1) {
		opcode_t opcode = OPCODES[i];

		if ((cpu->opcode & opcode.mask) == opcode.opcode) {
			/* Execute handler if current opcode match. */
			if (opcode.handler != NULL) {
				cpu->PC = opcode.handler(cpu);
			}

			/* Verify if the opcode handler setted the error flag to true. */
			if (has_error) {
				log_error("An error occurried while execute opcode: %X", cpu->opcode);
				return STATUS_ERROR;
			}

			return STATUS_OK;
		}
	}

	log_error("Unknown opcode: 0x%X", cpu->opcode);
	return STATUS_ERROR;
}

/* 0x00E0 - CLS: Clear display. */
static uint16_t opcode_CLS(cpu_t *cpu) {
	memset(cpu->gfx, 0, GFX_WIDTH * GFX_HEIGHT * sizeof(uint8_t));
	cpu->gfx_changed = true;
	return NEXT_PC;
}

/* 0x00EE - RET: Return from a subroutine.
 * The interpreter sets the program counter to the address at the top of the stack,
 * then subtracts 1 from the stack pointer.
 * WARN: If stack pointer is <= 0, the instruction is skipped.
 */
static uint16_t opcode_RET(cpu_t *cpu) {
	if (cpu->SP <= 0) {
		/* Skip instruction and set has_error to true.*/
		has_error = true;
		return NEXT_PC;
	}

	cpu->SP -= 1;
	return cpu->stack[cpu->SP] + 2;
}

/* 0x1nnn - JMP: Jump to location nnn.
 * The interpreter sets the program counter to nnn.
 */
static uint16_t opcode_JMP(cpu_t *cpu) {
	return cpu->addr;
}

/* 0x2nnn - CALL: Call subroutine at nnn.
 * The interpreter increments the stack pointer,
 * then puts the current PC on the top of the stack.
 * The PC is then set to nnn.
 * WARN: If stack pointer is <= 0, the instruction is skipped.
 */
static uint16_t opcode_CALL(cpu_t *cpu) {
	if (cpu->SP >= STACK_SIZE) {
		/* Skip instruction and set has_error to true. */
		has_error = true;
		return NEXT_PC;
	}

	cpu->stack[cpu->SP] = cpu->PC;
	cpu->SP += 1;
	return cpu->addr;
}

/* 0x3xkk - SE: Skip next instruction if Vx == kk.
 * The interpreter compares register Vx to kk, and if they are equal,
 * increments the program counter by 2, otherwise, increments the program counter by 4.
 */
static uint16_t opcode_SE(cpu_t *cpu) {
	const uint8_t val = cpu->byte;
	const uint8_t reg = cpu->v_register[cpu->x];

	return reg == val ? SKIP_PC : NEXT_PC;
}

/* 0x4xkk - SNE: Skip next instruction is Vx != kk.
 * The interpreter compares register Vx to kk, and if they are NOT equal,
 * increments the program counter by 2.
 */
static uint16_t opcode_SNE(cpu_t *cpu) {
	const uint8_t val = cpu->byte;
	const uint8_t reg = cpu->v_register[cpu->x];

	return reg != val ? SKIP_PC : NEXT_PC;
}

/* 0x5xy0 - SEREG: Skip next instruction if Vx == Vy.
 * The interpreter compares register Vx to register Vy, and if they are equal,
 * increments the program counter by 2.
 */
static uint16_t opcode_SEREG(cpu_t *cpu) {
	const uint8_t reg_x = cpu->v_register[cpu->x];
	const uint8_t reg_y = cpu->v_register[cpu->y];

	return reg_x == reg_y ? SKIP_PC : NEXT_PC;
}

/* 0x6xkk - LDIMM: Set Vx = kk.
 * The interpreter puts the value kk into register Vx.
 */
static uint16_t opcode_LDIMM(cpu_t *cpu) {
	const uint8_t val = cpu->byte;
	uint8_t *reg = &cpu->v_register[cpu->x];

	*reg = val;
	return NEXT_PC;
}

/* 0x7xkk - ADDIMM: Set Vx = Vx + kk.
 * The interpreter adds the value kk to the value of register Vx,
 * then stores the result in Vx.
 */
static uint16_t opcode_ADDIMM(cpu_t *cpu) {
	const uint8_t val = cpu->byte;
	uint8_t *reg = &cpu->v_register[cpu->x];

	*reg += val;
	return NEXT_PC;
}

/* 0x8xy0 - LDV: Set Vx = Vy.
 * The interpreter stores the value of register Vy in register Vx.
 */
static uint16_t opcode_LDV(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];

	*reg_x = reg_y;
	return NEXT_PC;
}

/* 0x8xy1 - OR: Set Vx = Vx OR Vy.
 * The interpreter performs a bitwise OR on the values of Vx and Vy, then stores the
 * result in Vx. A bitwise OR compares the corresponding bits from two values, and if
 * either bit is 1, then the same bit in the result is also 1. Otherwise, it is 0.
 */
static uint16_t opcode_OR(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];

	*reg_x |= reg_y;
	return NEXT_PC;
}

/* 0x8xy2 - AND: Set Vx = Vx AND Vy.
 * The interpreter performs a bitwise AND on the values of Vx and Vy, then stores the
 * result in Vx. A bitwise AND compares the corresponding bits from two values, and if
 * both bits are 1, then the same bit in the result is also 1. Otherwise, it is 0.
 */
static uint16_t opcode_AND(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];

	*reg_x &= reg_y;
	return NEXT_PC;
}

/* 0x8xy3 - XOR: Set Vx = Vx XOR Vy.
 * The interpreter erforms a bitwise exclusive OR on the values of Vx and Vy, then stores
 * the result in Vx. An exclusive OR compares the corresponding bits from two values, and
 * if the bits are not both the same, then the corresponding bit in the result is set
 * to 1. Otherwise, it is 0.
 */
static uint16_t opcode_XOR(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];

	*reg_x ^= reg_y;
	return NEXT_PC;
}

/* 0x8xy4 - ADD: Set Vx = Vx + Vy, set VF = carry.
 * The interpreter adds Vy to the value of Vx, then store result in Vx
 * If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0.
 * Only the lowest 8 bits of the result are kept, and stored in Vx.
 */
static uint16_t opcode_ADD(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];
	uint16_t val = *reg_x + reg_y;

	cpu->v_register[0xF] = val > UINT8_MAX; /* Set carry flag. */
	*reg_x += reg_y;
	return NEXT_PC;
}

/* 0x8xy5 - SUB: Set Vx = Vx - Vy, set VF = NOT borrow.
 * If Vx > Vy, then VF is set to 1, otherwise 0.
 * Then Vy is subtracted from Vx, and the results stored in Vx.
 */
static uint16_t opcode_SUB(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];

	cpu->v_register[0xF] = *reg_x > reg_y; /* Set not borrow. */
	*reg_x -= reg_y;
	return NEXT_PC;
}

/* 0x8xy6 - SHR: Set Vx = Vx SHR 1.
 * Interpreter set VF to the least-significant bit of Vx. Then Vx is divided by 2.
 * Vy is ignored.
 * WARN: Ambiguous opcode.
 */
static uint16_t opcode_SHR(cpu_t *cpu) {
	uint8_t *reg = &cpu->v_register[cpu->x];

	cpu->v_register[0xF] = *reg & 0x1;
	*reg >>= 1;
	return NEXT_PC;
}

/* 0x8xy7 - SUBN: Set Vx = Vy - Vx, set VF = NOT borrow.
 * If Vy > Vx, then VF is set to 1, otherwise 0.
 * Then Vx is subtracted from Vy, and the results stored in Vx.
 */
static uint16_t opcode_SUBN(cpu_t *cpu) {
	const uint8_t reg_y = cpu->v_register[cpu->y];
	uint8_t *reg_x = &cpu->v_register[cpu->x];

	cpu->v_register[0xF] = *reg_x < reg_y; /* Set not borrow. */
	*reg_x = reg_y - (*reg_x);
	return NEXT_PC;
}

/* 0x8xyE - SHL: Set Vx = Vx SHL 1.
 * Interpreter set VF to the most-significant bit of Vx. Then Vx is multiplied by 2.
 * Vy is ignored.
 * WARN: Ambiguous opcode.
 */
static uint16_t opcode_SHL(cpu_t *cpu) {
	uint8_t *reg = &cpu->v_register[cpu->x];

	cpu->v_register[0xF] = (*reg >> 7) & 1;
	*reg <<= 1;
	return NEXT_PC;
}

/* 0x9xy0 - SNEREG: Skip next instruction if Vx != Vy.
 * Interpreter compare Vx and Vy, if they are not equal,
 * the program counter is increased by 2,
 * otherwise, the program counter is increased by 4.
 */
static uint16_t opcode_SNEREG(cpu_t *cpu) {
	const uint8_t reg_x = cpu->v_register[cpu->x];
	const uint8_t reg_y = cpu->v_register[cpu->y];

	return reg_x != reg_y ? SKIP_PC : NEXT_PC;
}

/* 0xAnnn - LDI: Set I = nnn.
 * The value of register I is set to nnn.
 */
static uint16_t opcode_LDI(cpu_t *cpu) {
	cpu->I = cpu->addr;
	return NEXT_PC;
}

/* 0xBnnn - JMPREG: Jump to location nnn + V0.
 * Interpreter set PC to nnn plus the value of V0.
 * WARN: Ambiguous opcode.
 * TODO: This opcode also can be read as 0xBxnn, make this configurable.
 */
static uint16_t opcode_JMPREG(cpu_t *cpu) {
	return cpu->addr + cpu->v_register[0];
}

/* 0xCxkk - RAND: Set Vx = random_byte AND kk;
 * The interpreter generate random number from 0 to 255,
 * which is then ANDed with the value of kk. The results are stored in Vx.
 */
static uint16_t opcode_RAND(cpu_t *cpu) {
	const uint8_t byte = cpu->byte;
	uint8_t *reg = &cpu->v_register[cpu->x];

	*reg = (rand() % 256) & byte;
	return NEXT_PC;
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
static uint16_t opcode_DRAW(cpu_t *cpu) {
	const uint8_t x = cpu->v_register[cpu->x];
	const uint8_t y = cpu->v_register[cpu->y];
	const uint8_t n = cpu->nibble;

	/* Copy sprite from the memory. */
	uint8_t sprite[n];
	memcpy(sprite, &cpu->memory[cpu->I], n);

	cpu->v_register[0xF] = 0; /* Set pixel erased flag to 0. */
	for (uint8_t byte_index = 0; byte_index < n; byte_index += 1) {
		for (uint8_t bit_index = 0; bit_index < 8; bit_index += 1) {
			/* Calculate pixel position. Wrap if going beyond screen boundaries. */
			const uint8_t x_pos = (x + bit_index) % GFX_WIDTH;
			const uint8_t y_pos = (y + byte_index) % GFX_HEIGHT;

			/* Calculate pixel value */
			const uint8_t bit = (sprite[byte_index] >> (7 - bit_index)) & 1;
			uint8_t *pixel = &cpu->gfx[x_pos + y_pos * GFX_WIDTH];

			/* If pixel is ereased, set flag to 1. */
			if (bit == 0x1 && *pixel == 0x1) {
				cpu->v_register[0xF] = 1;
			}

			*pixel ^= bit; /* Effectively XOR with the sprite pixel */
		}
	}

	cpu->gfx_changed = true;
	return NEXT_PC;
}

/* 0xEx9E - SKEY: Skip next instruction if key with the value of Vx is pressed.
 * Checks the keyboard state , and if the key corresponding to the value of Vx is
 * currently in the down position, PC is increased by 2, otherwise, PC is increased by 4.
 */
static uint16_t opcode_SKEY(cpu_t *cpu) {
	const uint8_t reg = cpu->v_register[cpu->x];
	const uint8_t key_state = cpu->key_state[reg];

	return key_state == 1 ? SKIP_PC : NEXT_PC;
}

/* 0xExA1 - SNKEY: Skip next instruction if key with the value of Vx is not pressed.
 * Checks the keyboard state , and if the key corresponding to the value of Vx is
 * currently in the up position, PC is increased by 2, otherwise, PC is increased by 4.
 */
static uint16_t opcode_SNKEY(cpu_t *cpu) {
	const uint8_t reg = cpu->v_register[cpu->x];
	const uint8_t key_state = cpu->key_state[reg];

	return key_state == 0 ? SKIP_PC : NEXT_PC;
}

/* 0xFx07 - RDELAY: Set Vx = delay timer value.
 * Interpreter copy the value of delay timer into Vx.
 */
static uint16_t opcode_RDELAY(cpu_t *cpu) {
	uint8_t *reg = &cpu->v_register[cpu->x];

	*reg = cpu->delay_timer;
	return NEXT_PC;
}

/* 0xFx0A - WAITKEY: Wait for a key press, store the value of the key in Vx.
 * All execution stops until a key is pressed,
 * then the value of that key is stored in Vx.
 */
static uint16_t opcode_WAITKEY(cpu_t *cpu) {
	for (uint8_t key = 0; key < KEYS_COUNT; key += 1) {
		if (cpu->key_state[key] == 1) {
			cpu->v_register[cpu->x] = key;
			return NEXT_PC;
		}
	}

	return cpu->PC; /* Reexecute this instruction. */
}

/* 0xFx15 - STDELAY: Set delay timer = Vx.
 * Interpreter copy the value of Vx into delay timer.
 */
static uint16_t opcode_WDELAY(cpu_t *cpu) {
	const uint8_t reg_val = cpu->v_register[cpu->x];
	uint8_t *delay_timer = &cpu->delay_timer;

	*delay_timer = reg_val;
	return NEXT_PC;
}

/* 0xFx18 - STSOUND: Set sound timer = Vx.
 * Interpreter copy the value of Vx into sound timer.
 */
static uint16_t opcode_WSOUND(cpu_t *cpu) {
	const uint8_t reg = cpu->v_register[cpu->x];
	uint8_t *sound_timer = &cpu->sound_timer;

	*sound_timer = reg;
	return NEXT_PC;
}

/* 0xFx1E - ADDI: Set I = I + Vx.
 * The values of I and Vx are added, and the result is stored in I.
 */
static uint16_t opcode_ADDI(cpu_t *cpu) {
	const uint8_t reg = cpu->v_register[cpu->x];

	cpu->I += reg;
	return NEXT_PC;
}

/* 0xFx29 - LDSPRITE: Set I = location of sprite for digit Vx.
 * The value of I is set to the location for the hexadecimal sprite corresponding
 * to the value of Vx.
 */
static uint16_t opcode_LDSPRITE(cpu_t *cpu) {
	const uint8_t reg = cpu->v_register[cpu->x];

	cpu->I = FONT_ADDRESS + (FONT_CHAR_SIZE * reg);
	return NEXT_PC;
}

/* 0xFx33 - STBCD: Store BCD representation of Vx in memory locations I, I+1, and I+2.
 * The interpreter takes the decimal value of Vx, and places the hundreds digit in
 * memory at location in I, the tens digit at location I+1,
 * and the ones digit at location I+2.
 */
static uint16_t opcode_STBCD(cpu_t *cpu) {
	const uint8_t value = cpu->v_register[cpu->x];
	const uint8_t ones = value % 10;
	const uint8_t tens = (value / 10) % 10;
	const uint8_t hundreds = (value / 100) % 10;

	cpu->memory[cpu->I + 0] = hundreds;
	cpu->memory[cpu->I + 1] = tens;
	cpu->memory[cpu->I + 2] = ones;
	return NEXT_PC;
}

/* 0xFx55 - STREG: Store registers V0 through Vx in memory starting at location I.
 * The interpreter copies the values of registers V0 through Vx into memory,
 * starting at the address in I.
 */
static uint16_t opcode_STREG(cpu_t *cpu) {
	const uint8_t reg = cpu->x;

	for (size_t i = 0; i <= reg; i += 1) {
		cpu->memory[cpu->I + i] = cpu->v_register[i];
	}

	return NEXT_PC;
}

/* 0xFx65 - LDREG: Read registers V0 through Vx from memory starting at location I.
 * The interpreter reads values from memory starting at location I into
 * registers V0 through Vx.
 */
static uint16_t opcode_LDREG(cpu_t *cpu) {
	const uint8_t reg = cpu->x;

	for (size_t i = 0; i <= reg; i += 1) {
		cpu->v_register[i] = cpu->memory[cpu->I + i];
	}

	return NEXT_PC;
}
