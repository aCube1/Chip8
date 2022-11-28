#include "core.h"

#include "display.h"
#include "log.h"
#include "utils.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

static struct {
	bool running;
	Display display;
} Core;

int8_t core_init(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	uint32_t init_flags = SDL_INIT_EVERYTHING;
	if (SDL_Init(init_flags) < 0) {
		log_error("Unable to init SDL2: %s", SDL_GetError());
		return STATUS_ERROR;
	}

	if (create_display(&Core.display) != STATUS_OK) {
		log_error("Unable to create display!");
	}

	return STATUS_OK;
}

int8_t core_run(void) { return -1; }
