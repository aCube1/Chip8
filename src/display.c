#include "display.h"

#include "cpu.h"
#include "log.h"
#include "utils.h"

#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_video.h>

static SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL;
static SDL_RendererFlags renderer_flags = SDL_RENDERER_ACCELERATED |
										  SDL_RENDERER_PRESENTVSYNC;

int8_t create_display(display_t *display, int16_t width, int16_t height) {
	display->window = NULL;
	display->renderer = NULL;

	display->window = SDL_CreateWindow(
		TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags
	);
	if (display->window == NULL) {
		log_fatal("Unable to create window: %s", SDL_GetError());
		return STATUS_ERROR;
	}

	display->renderer = SDL_CreateRenderer(display->window, -1, renderer_flags);
	if (display->renderer == NULL) {
		log_fatal("Unable to create renderer: %s", SDL_GetError());
		destroy_display(display);
		return STATUS_ERROR;
	}

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

int8_t surface_set_pixel(SDL_Surface *surface, uint32_t x, uint32_t y, uint32_t color) {
	bool must_lock = SDL_MUSTLOCK(surface) == SDL_TRUE;
	if (must_lock && SDL_LockSurface(surface) < 0) {
		log_error("Unable to lock surface: %s", SDL_GetError());
		return STATUS_ERROR;
	}

	uint32_t offset = y * surface->pitch + x * surface->format->BytesPerPixel;
	uint32_t *pixel = (uint32_t *)((uint8_t *)surface->pixels + offset);
	*pixel = color;

	if (must_lock) {
		SDL_UnlockSurface(surface);
	}

	return STATUS_OK;
}

int8_t display_render(
	display_t *display, SDL_Texture *texture, SDL_Rect *srcrect, SDL_Rect *dstrect
) {
	if (SDL_RenderCopy(display->renderer, texture, srcrect, dstrect) < 0) {
		log_error("Unable to render texture: %s", SDL_GetError());
		return STATUS_ERROR;
	}
	return STATUS_OK;
}

void display_clear(display_t *display, bool *is_running) {
	if (SDL_RenderClear(display->renderer) < 0) {
		log_error("Unable to clear renderer: %s", SDL_GetError());
		*is_running = false;
	}
}

void display_update(display_t *display) {
	SDL_RenderPresent(display->renderer);
}
