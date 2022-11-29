#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <SDL2/SDL_render.h>
#include <stdint.h>

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
} display_t;

int8_t create_display(display_t *display);
void destroy_display(display_t *display);

/* Wrappers to SDL_RenderClear and SDL_RenderPresent respectively. */
void display_clear(display_t *display);
void display_update(display_t *display);

#endif /* _DISPLAY_H_ */
