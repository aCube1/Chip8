#include "core.h"

#include "display.h"
#include "log.h"
#include "utils.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

static struct {
	bool running;
	display_t display;
} Core;

int8_t core_init(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	uint32_t init_flags = SDL_INIT_EVERYTHING;
	if (SDL_Init(init_flags) < 0) {
		log_fatal("Unable to init SDL2: %s", SDL_GetError());
		return STATUS_ERROR;
	}

	if (create_display(&Core.display) != STATUS_OK) {
		log_fatal("Unable to create display!");
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
			}
		}

		display_clear(&Core.display);
		/* TODO: Render things here */
		display_update(&Core.display);
	}

	return STATUS_OK;
}
