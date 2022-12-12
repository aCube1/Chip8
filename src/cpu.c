#include "cpu.h"

#include "display.h"
#include "log.h"
#include "opcodes.h"
#include "utils.h"

#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <stdlib.h>
#include <string.h>

static void update_timers(cpu_t *cpu);

static const SDL_PixelFormatEnum texture_pixel_format = SDL_PIXELFORMAT_ABGR32;
static const SDL_TextureAccess texture_access = SDL_TEXTUREACCESS_STREAMING;

static const uint8_t CPU_font[80] = {
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

int8_t cpu_init(cpu_t *cpu, SDL_Renderer *renderer) {
	cpu_reset(cpu); /* Reset CPU to a initial state. */

	cpu->screen = SDL_CreateTexture(
		renderer, texture_pixel_format, texture_access, GFX_WIDTH, GFX_HEIGHT
	);
	if (cpu->screen == NULL) {
		log_error("Unable to create Chip8 render screen: %s", SDL_GetError());
		return STATUS_ERROR;
	}

	return STATUS_OK;
}

int8_t cpu_emulate_cycle(cpu_t *cpu) {
	update_timers(cpu);
	/* Fetch opcode */
	cpu->opcode = cpu->memory[cpu->PC] << 8 | cpu->memory[cpu->PC + 1];
	cpu->addr = cpu->opcode & 0x0FFF;
	cpu->byte = cpu->opcode & 0x00FF;
	cpu->nibble = cpu->opcode & 0x000F;
	cpu->x = (cpu->opcode & 0x0F00) >> 8;
	cpu->y = (cpu->opcode & 0x00F0) >> 4;

	if (opcode_decode(cpu)) {
		log_error("Unable to decode OPCODE.");
		return STATUS_ERROR;
	}

	if (cpu->gfx_changed) {
		cpu_update_screen(cpu);
	}

	return STATUS_OK;
}

void cpu_reset(cpu_t *cpu) {
	/* System expects the application to be loaded at memory location 0x200 */
	cpu->PC = 0x200;
	cpu->opcode = 0; /* Reset current opcode */
	cpu->I = 0;		 /* Reset index register */
	cpu->SP = 0;	 /* Reset stack pointer */

	/* Reset timers */
	cpu->delay_timer = 0;
	cpu->sound_timer = 0;

	memset(cpu->memory, 0, MEMORY_SIZE * sizeof(uint8_t)); /* Reset memory */
	memset(cpu->stack, 0, STACK_SIZE * sizeof(uint16_t));  /* Reset stack */
	/* Reset registers */
	memset(cpu->v_register, 0, REGISTERS_COUNT * sizeof(uint8_t));
	/* Reset display */
	memset(cpu->gfx, 0, GFX_WIDTH * GFX_HEIGHT * sizeof(uint8_t));
	/* Reset key states */
	memset(cpu->key_state, 0, KEYS_COUNT * sizeof(uint8_t));

	/* Load the built-in fontset in 0x50-0x0A0 */
	memcpy(cpu->memory + 0x50, CPU_font, 80 * sizeof(uint8_t));
}

void cpu_quit(cpu_t *cpu) {
	if (cpu->screen != NULL) {
		SDL_DestroyTexture(cpu->screen);
		log_info("Chip8 render screen destroyed!");
	}
}

int8_t cpu_loadrom(cpu_t *cpu, const char *filepath) {
	const uint32_t max_rom_size = MEMORY_SIZE - 0x200; /* 0xDFF = 3584 bytes */
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

static void update_timers(cpu_t *cpu) {
	if (cpu->delay_timer > 0) {
		cpu->delay_timer -= 1;
	}
	if (cpu->sound_timer > 0) {
		cpu->sound_timer -= 1;
	}
}
