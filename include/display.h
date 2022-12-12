#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
} display_t;

int8_t create_display(display_t *display, int16_t width, int16_t height);
void destroy_display(display_t *display);

int8_t surface_set_pixel(SDL_Surface *surface, uint32_t x, uint32_t y, uint32_t color);

/* SDL_RenderCopy wrapper. Return STATUS_ERROR if error. */
int8_t display_render(
	display_t *display, SDL_Texture *texture, SDL_Rect *srcrect, SDL_Rect *dstrect
);
/* SDL_RenderClear wrapper. Set is_running to false if error.*/
void display_clear(display_t *display, bool *is_running);
/* SDL_RenderPresent wrapper. */
void display_update(display_t *display);

#endif /* _DISPLAY_H_ */
