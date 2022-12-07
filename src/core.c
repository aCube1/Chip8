#include "core.h"

#include "cpu.h"
#include "display.h"
#include "log.h"
#include "utils.h"

#include <SDL2/SDL.h>

static void update_fps(void);
static void core_exit(void);

static struct {
	bool running;

	display_t display;
	cpu_t cpu;

	uint16_t current_fps;
	double delta_time;
} Core;

static uint64_t last_ticks = 0;
static uint64_t fps_count = 0;
static double fps_timer = 0.0;

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

	if (cpu_init(&Core.cpu) != STATUS_OK) {
		log_fatal("Unable to init Chip-8 CPU!");
		core_exit();
		return STATUS_ERROR;
	}

	if (cpu_loadrom(&Core.cpu, configs.rom_filepath) != STATUS_OK) {
		log_error("Unable to load rom to memory!");
		core_exit();
		return STATUS_ERROR;
	}

	Core.running = true;
	return STATUS_OK;
}

int8_t core_run(void) {
	SDL_Event event;

	while (Core.running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				Core.running = false;
				break;
			}
		}

		cpu_emulate_cycle(&Core.cpu);

		display_clear(&Core.display);
		/* TODO: Render things here */
		display_update(&Core.display);

		update_fps();
	}

	core_exit();
	return STATUS_OK;
}

static void update_fps(void) {
	Core.delta_time = (SDL_GetTicks64() - last_ticks) / 1000.0;
	fps_timer += Core.delta_time;

	if (fps_timer >= 1.0) {
		Core.current_fps = fps_count;
		fps_count = 0;
		fps_timer = 0;
	}

	fps_count += 1;
	last_ticks = SDL_GetTicks64();
}

static void core_exit(void) {
	destroy_display(&Core.display);
	log_info("Core exitted!");
}
