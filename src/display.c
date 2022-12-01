#include "display.h"

#include "log.h"
#include "utils.h"

#include <SDL_render.h>
#include <SDL_video.h>

int8_t create_display(display_t *display) {
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL;
	SDL_RendererFlags renderer_flags =
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

	window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
		window_flags
	);
	if (window == NULL) {
		log_fatal("Unable to create window: %s", SDL_GetError());
		return STATUS_ERROR;
	}

	renderer = SDL_CreateRenderer(window, -1, renderer_flags);
	if (renderer == NULL) {
		log_fatal("Unable to create renderer: %s", SDL_GetError());
		return STATUS_ERROR;
	}

	display->window = window;
	display->renderer = renderer;
	log_info("Display created!");
	return STATUS_OK;
}

void destroy_display(display_t *display) {
	if (display->renderer != NULL) {
		SDL_DestroyRenderer(display->renderer);
		log_info("Renderer destroyed!");
	}

	if (display->window != NULL) {
		SDL_DestroyWindow(display->window);
		log_info("Window destroyed!");
	}

	log_info("Display destroyed!");
}

void display_clear(display_t *display) {
	if (SDL_RenderClear(display->renderer) < 0) {
		log_error("Unable to clear renderer: %s", SDL_GetError());
	}
}

void display_update(display_t *display) {
	SDL_RenderPresent(display->renderer);
}
