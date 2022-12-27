#ifndef _INPUT_H_
#define _INPUT_H_

#include "cpu.h"

#include <SDL2/SDL_keyboard.h>
#include <SDL_events.h>
#include <stdint.h>

/* Chip8 Key layout.
 * 1 | 2 | 3 | C
 * 4 | 5 | 6 | D
 * 7 | 8 | 9 | E
 * A | 0 | B | F
 */

typedef struct {
	SDL_Scancode cpu_keys[KEYS_COUNT]; /* CPU keymap. */
} input_t;

/* Load default key mapping. */
void input_init(input_t *input);

/* Read scancode from SDL_Event and update keystate. */
void input_update_keystate(SDL_Event *event, input_t input, uint8_t *keystate);

#endif /* _INPUT_H_ */
