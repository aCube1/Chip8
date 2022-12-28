#include "display.h"

#include "cpu.h"
#include "log.h"
#include "utils.h"

#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_video.h>

#define WINDOW_TITLE "Chip8 Emulator"

static SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL;
static SDL_RendererFlags renderer_flags = SDL_RENDERER_ACCELERATED |
										  SDL_RENDERER_PRESENTVSYNC;

static const SDL_PixelFormatEnum texture_pixel_format = SDL_PIXELFORMAT_ABGR32;
static const SDL_TextureAccess texture_access = SDL_TEXTUREACCESS_STREAMING;

int8_t create_display(display_t *display, int16_t width, int16_t height) {
	display->window = SDL_CreateWindow(
		WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
		window_flags
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

	display->cpu_screen = SDL_CreateTexture(
		display->renderer, texture_pixel_format, texture_access, GFX_WIDTH, GFX_HEIGHT
	);
	if (display->cpu_screen == NULL) {
		log_error("Unable to create Chip8 render screen: %s", SDL_GetError());
		destroy_display(display);
		return STATUS_ERROR;
	}

	log_info("Display created!");
	return STATUS_OK;
}

void destroy_display(display_t *display) {
	if (display->cpu_screen != NULL) {
		SDL_DestroyTexture(display->cpu_screen);
		log_info("Chip8 render screen destroyed!");
	}

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
	if (SDL_MUSTLOCK(surface) == SDL_TRUE && SDL_LockSurface(surface) < 0) {
		log_error("Unable to lock surface: %s", SDL_GetError());
		return STATUS_ERROR;
	}

	uint32_t offset = y * surface->pitch + x * surface->format->BytesPerPixel;
	uint32_t *pixel = (uint32_t *)((uint8_t *)surface->pixels + offset);
	*pixel = color;

	SDL_UnlockSurface(surface);

	return STATUS_OK;
}

void display_update_screen(display_t *display, uint8_t *gfx) {
	uint32_t target_color = 0;
	SDL_Surface *surface = NULL; /* Screen surface */
	SDL_Texture *screen = display->cpu_screen;

	SDL_LockTextureToSurface(screen, NULL, &surface);
	for (size_t x = 0; x < GFX_WIDTH; x += 1) {
		for (size_t y = 0; y < GFX_HEIGHT; y += 1) {
			uint8_t pixel = gfx[y * GFX_WIDTH + x];

			if (pixel == 0x1) {
				target_color = SDL_MapRGBA(surface->format, FORE_R, FORE_G, FORE_B, 0xFF);
			} else {
				target_color = SDL_MapRGBA(surface->format, BACK_R, BACK_G, BACK_B, 0xFF);
			}

			surface_set_pixel(surface, x, y, target_color);
		}
	}
	SDL_UnlockTexture(screen);
}

int8_t display_render_screen(display_t *display, SDL_Rect *srcrect, SDL_Rect *dstrect) {
	return display_render(display, display->cpu_screen, srcrect, dstrect);
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
