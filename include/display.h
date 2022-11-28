#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <SDL2/SDL_render.h>
#include <stdint.h>

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
} Display;

int8_t create_display(Display *display);
void destroy_display(Display *display);

#endif /* _DISPLAY_H_ */
