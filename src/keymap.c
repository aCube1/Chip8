#include "keymap.h"

#include "cpu.h"
#include "log.h"
#include "utils.h"

#include <SDL_scancode.h>

/* Default scancode layout.
	1 | 2 | 3 | 4
	Q | W | E | R
	A | S | D | F
	Z | X | C | V
*/

void keymap_load_default(keymap_t *map) {
	map->chip8_keys[0] = SDL_SCANCODE_1;
	map->chip8_keys[1] = SDL_SCANCODE_2;
	map->chip8_keys[2] = SDL_SCANCODE_3;
	map->chip8_keys[3] = SDL_SCANCODE_C;

	map->chip8_keys[4] = SDL_SCANCODE_4;
	map->chip8_keys[5] = SDL_SCANCODE_5;
	map->chip8_keys[6] = SDL_SCANCODE_6;
	map->chip8_keys[7] = SDL_SCANCODE_D;

	map->chip8_keys[8] = SDL_SCANCODE_7;
	map->chip8_keys[9] = SDL_SCANCODE_8;
	map->chip8_keys[10] = SDL_SCANCODE_9;
	map->chip8_keys[11] = SDL_SCANCODE_E;

	map->chip8_keys[12] = SDL_SCANCODE_A;
	map->chip8_keys[13] = SDL_SCANCODE_0;
	map->chip8_keys[14] = SDL_SCANCODE_B;
	map->chip8_keys[15] = SDL_SCANCODE_F;
}

void keymap_get_keystate(SDL_Event *event, keymap_t map, uint8_t keystate[KEYS_COUNT]) {
	SDL_Scancode scancode = event->key.keysym.scancode;

	for (uint8_t i = 0; i < KEYS_COUNT; i += 1) {
		if (scancode == map.chip8_keys[i]) {
			/* If key is pressed, set keystate to 1, if not pressed, set keystate to 0. */
			keystate[i] = event->key.state == SDL_PRESSED ? 1 : 0;
			break;
		}
	}
}
