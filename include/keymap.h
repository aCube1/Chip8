#ifndef _KEYMAP_H_
#define _KEYMAP_H_

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
	SDL_Scancode chip8_keys[KEYS_COUNT];
} keymap_t;

/* Load default key mapping. */
void keymap_load_default(keymap_t *map);

/* Read scancode from SDL_Event and update keystate. */
void keymap_get_keystate(SDL_Event *event, keymap_t map, uint8_t keystate[KEYS_COUNT]);

#endif /* _KEYMAP_H_ */
