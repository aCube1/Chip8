#include "cpu.h"

#include "display.h"
#include "log.h"
#include "opcodes.h"
#include "utils.h"

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>

#define FONT_OFFSET 0x50
#define ROM_OFFSET	0x200 /* 512 */

static uint64_t last_time = 0;
static double pending_cpu_cycles = 0;
static double pending_timer_cycles = 0;

static int8_t do_cpu_cycles(cpu_t *cpu, uint32_t amount); /* Fetch and decode opcodes. */
static void do_timers_cycles(cpu_t *cpu, uint32_t amount);

static const SDL_PixelFormatEnum texture_pixel_format = SDL_PIXELFORMAT_ABGR32;
static const SDL_TextureAccess texture_access = SDL_TEXTUREACCESS_STREAMING;

static const uint8_t cpu_font[] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
	0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
	0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
	0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
	0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
	0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
	0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
	0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
	0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
	0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
	0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
	0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
	0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
	0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
	0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
	0xF0, 0x80, 0xF0, 0x80, 0x80, /* F */
};

int8_t cpu_init(cpu_t *cpu, uint16_t clock_speed, SDL_Renderer *renderer) {
	cpu_reset(cpu); /* Reset CPU to a initial state. */

	cpu->screen = SDL_CreateTexture(
		renderer, texture_pixel_format, texture_access, GFX_WIDTH, GFX_HEIGHT
	);
	if (cpu->screen == NULL) {
		log_error("Unable to create Chip8 render screen: %s", SDL_GetError());
		return STATUS_ERROR;
	}

	cpu->clock_speed = clock_speed;
	return STATUS_OK;
}

int8_t cpu_update(cpu_t *cpu) {
	if (last_time == 0.0f) { /* Running for the first time. */
		last_time = SDL_GetTicks64();
	}

	/* Figure out how long it has been since the previous cpu_update call.*/
	const uint64_t time_passed = SDL_GetTicks64() - last_time;
	last_time = SDL_GetTicks64();

	/* Get pending cycles for cpu and timer. */
	pending_cpu_cycles += time_passed * cpu->clock_speed / 1000.0f;
	pending_timer_cycles += time_passed * TIMER_CLOCK_SPEED / 1000.0f;

	/* Perform for the number of whole cycles accumulated. */
	do_timers_cycles(cpu, (uint32_t)pending_timer_cycles);
	if (do_cpu_cycles(cpu, (uint32_t)pending_cpu_cycles) != STATUS_OK) {
		log_error("Unable to do cpu cycles!");
		return STATUS_ERROR;
	}

	/* Update cpu screen if gfx array has changed */
	if (cpu->has_gfx_changed) {
		cpu_update_screen(cpu);
		cpu->has_gfx_changed = false;
	}

	/* Keep the pending cycles remainder. */
	pending_cpu_cycles = fmodf(pending_cpu_cycles, 1.0);
	pending_timer_cycles = fmodf(pending_timer_cycles, 1.0);
	return STATUS_OK;
}

void cpu_reset(cpu_t *cpu) {
	cpu->PC = ROM_OFFSET; /* ROM is loaded at memory location 0x200 */
	cpu->opcode = 0;	  /* Reset current opcode. */
	cpu->I = 0;			  /* Reset index register. */
	cpu->SP = 0;		  /* Reset stack pointer. */

	/* Reset timers */
	cpu->delay_timer = 0;
	cpu->sound_timer = 0;

	memset(cpu->memory, 0, sizeof(uint8_t) * RAM_SIZE);			   /* Reset memory */
	memset(cpu->stack, 0, sizeof(uint16_t) * STACK_SIZE);		   /* Reset stack */
	memset(cpu->V, 0, sizeof(uint8_t) * V_REGISTERS_COUNT);		   /* Reset registers */
	memset(cpu->gfx, 0, sizeof(uint8_t) * GFX_WIDTH * GFX_HEIGHT); /* Reset display */
	memset(cpu->key_state, 0, sizeof(uint8_t) * KEYS_COUNT);	   /* Reset key states */

	/* Load the built-in fontset in 0x50-0x0A0 */
	memcpy(
		cpu->memory + FONT_ADDRESS, cpu_font,
		FONT_CHAR_COUNT * FONT_CHAR_SIZE * sizeof(uint8_t)
	);
}

void cpu_quit(cpu_t *cpu) {
	if (cpu->screen != NULL) {
		SDL_DestroyTexture(cpu->screen);
		log_info("Chip8 render screen destroyed!");
	}
}

int8_t cpu_loadrom(cpu_t *cpu, const char *filepath) {
	const uint32_t max_rom_size = RAM_SIZE - ROM_OFFSET; /* 0xDFF = 3584 bytes */
	FILE *rom = fopen(filepath, "rb");
	file_t file;

	if (rom == NULL || get_file_content(&file, rom) != STATUS_OK) {
		log_error("Unable to read rom!");
		return STATUS_ERROR;
	}

	if (file.lenght > max_rom_size) {
		log_error("Rom file size is bigger than max rom size!");
		file_free(&file);
		return STATUS_ERROR;
	}

	/* Load ROM to memory */
	memcpy(cpu->memory + 0x200, file.content, file.lenght * sizeof(uint8_t));
	log_info("Loaded %s with %d bytes to memory.", filepath, file.lenght);

	file_free(&file); /* We don't need the file anymore. */
	return STATUS_OK;
}

void cpu_update_screen(cpu_t *cpu) {
	SDL_Surface *tex_surface = NULL;

	SDL_LockTextureToSurface(cpu->screen, NULL, &tex_surface);
	for (size_t x = 0; x < GFX_WIDTH; x += 1) {
		for (size_t y = 0; y < GFX_HEIGHT; y += 1) {
			uint8_t pixel = cpu->gfx[y * GFX_WIDTH + x];
			uint32_t target_color =
				pixel == 0x1
					? SDL_MapRGBA(tex_surface->format, FORE_R, FORE_G, FORE_B, 0xFF)
					: SDL_MapRGBA(tex_surface->format, BACK_R, BACK_G, BACK_B, 0xFF);

			surface_set_pixel(tex_surface, x, y, target_color);
		}
	}
	SDL_UnlockTexture(cpu->screen);
}

static int8_t do_cpu_cycles(cpu_t *cpu, uint32_t amount) {
	for (uint32_t i = 0; i < amount; i += 1) {
		if (cpu->PC > RAM_SIZE) {
			log_error("CPU program counter is greater than RAM size!");
			return STATUS_ERROR;
		}

		/* Fetch opcode */
		cpu->opcode = cpu->memory[cpu->PC] << 8 | cpu->memory[cpu->PC + 1];
		cpu->addr = cpu->opcode & 0x0FFF;
		cpu->byte = cpu->opcode & 0x00FF;
		cpu->nibble = cpu->opcode & 0x000F;
		cpu->x = (cpu->opcode & 0x0F00) >> 8;
		cpu->y = (cpu->opcode & 0x00F0) >> 4;

		if (opcode_decode(cpu) != STATUS_OK) {
			log_error("Unable to decode opcode: %X", cpu->opcode);
			return STATUS_ERROR;
		}
	}

	return STATUS_OK;
}

static void do_timers_cycles(cpu_t *cpu, uint32_t amount) {
	for (uint32_t i = 0; i < amount; i += 1) {
		if (cpu->delay_timer > 0) {
			cpu->delay_timer -= 1;
		}
		if (cpu->sound_timer > 0) {
			cpu->sound_timer -= 1;
		}
	}
}
