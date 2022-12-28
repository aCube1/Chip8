#include "core.h"

#include "audio.h"
#include "cpu.h"
#include "display.h"
#include "input.h"
#include "log.h"
#include "utils.h"

#include <SDL2/SDL.h>

static void update_fps(void);
static void core_exit(void);

static struct {
	bool is_running;

	display_t display;
	cpu_t cpu;
	input_t input;

	uint16_t current_fps;
} Core;

static uint64_t last_time = 0;
static uint64_t fps_count = 0;
static double fps_timer = 0.0f;

int8_t core_init(configs_t configs) {
	uint32_t init_flags = SDL_INIT_EVERYTHING;
	if (SDL_Init(init_flags) < 0) {
		log_fatal("Unable to init SDL2: %s", SDL_GetError());
		return STATUS_ERROR;
	}

	if (create_display(&Core.display, configs.width, configs.height) != STATUS_OK) {
		log_fatal("Unable to create display!");
		return STATUS_ERROR;
	}

	if (audio_init() != STATUS_OK) {
		log_fatal("Unable to initialize audio device!");
		return STATUS_ERROR;
	}

	if (cpu_init(&Core.cpu, configs.clock_speed) != STATUS_OK) {
		log_fatal("Unable to init Chip-8 CPU!");
		core_exit();
		return STATUS_ERROR;
	}

	if (cpu_loadrom(&Core.cpu, configs.rom_filepath) != STATUS_OK) {
		log_error("Unable to load rom to memory!");
		core_exit();
		return STATUS_ERROR;
	}

	input_init(&Core.input);

	Core.is_running = true;
	return STATUS_OK;
}

int8_t core_run(void) {
	int8_t status = STATUS_OK;
	SDL_Event event;

	last_time = SDL_GetTicks64();
	while (Core.is_running && status == STATUS_OK) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				Core.is_running = false;
				break;
			case SDL_KEYUP: /* FALLTHROUGH. */
			case SDL_KEYDOWN:
				/* Update cpu key state. */
				input_update_keystate(&event, Core.input, Core.cpu.key_state);
				break;
			}
		}

		if (cpu_update(&Core.cpu) != STATUS_OK) {
			log_debug("An error has been found while running CPU!");
			status = STATUS_ERROR;
		}

		/* Update cpu screen if gfx has changed. */
		if (Core.cpu.has_gfx_changed) {
			display_update_screen(&Core.display, Core.cpu.gfx);
		}

		display_clear(&Core.display, &Core.is_running);
		if (display_render_screen(&Core.display, NULL, NULL) != STATUS_OK) {
			log_error("Unable to render CPU screen.");
			status = STATUS_ERROR;
		}
		display_update(&Core.display);

		update_fps();
		last_time = SDL_GetTicks64();
	}

	core_exit();
	return status;
}

/* TODO: Render FPS on screen. */
static void update_fps(void) {
	fps_timer += (SDL_GetTicks64() - last_time) / 1000.0f;

	if (fps_timer >= 1.0f) {
		Core.current_fps = fps_count;
		fps_count = 0;
		fps_timer = 0;
	}

	fps_count += 1;
}

static void core_exit(void) {
	destroy_display(&Core.display);
	log_info("Core exitted!");
}
