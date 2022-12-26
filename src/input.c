#include "input.h"

#include "cpu.h"
#include "log.h"
#include "utils.h"

#include <SDL_keyboard.h>
#include <string.h>

/* Default scancode layout.
	1 | 2 | 3 | 4
	Q | W | E | R
	A | S | D | F
	Z | X | C | V
*/
static const SDL_Scancode default_keymap[] = {
	SDL_SCANCODE_X, /* 0 */
	SDL_SCANCODE_1, /* 1 */
	SDL_SCANCODE_2, /* 2 */
	SDL_SCANCODE_3, /* 3 */
	SDL_SCANCODE_Q, /* 4 */
	SDL_SCANCODE_W, /* 5 */
	SDL_SCANCODE_E, /* 6 */
	SDL_SCANCODE_A, /* 7 */
	SDL_SCANCODE_S, /* 8 */
	SDL_SCANCODE_D, /* 9 */
	SDL_SCANCODE_Z, /* A */
	SDL_SCANCODE_C, /* B */
	SDL_SCANCODE_4, /* C */
	SDL_SCANCODE_R, /* D */
	SDL_SCANCODE_F, /* E */
	SDL_SCANCODE_V, /* F */
};

void input_init(input_t *input) {
	/* Load default keymap */
	memcpy(input->cpu_keys, default_keymap, KEYS_COUNT * sizeof(SDL_Scancode));
}

void input_update_keystate(SDL_Event *event, input_t map, uint8_t *keystate) {
	SDL_Scancode scancode = event->key.keysym.scancode;
	bool is_pressed = event->key.state == SDL_PRESSED;

	for (uint8_t i = 0; i < KEYS_COUNT; i += 1) {
		if (map.cpu_keys[i] == scancode) {
			/* If key is pressed, set keystate to 1, if not, set it to 0. */
			keystate[i] = is_pressed ? 1 : 0;
			break;
		}
	}
}
