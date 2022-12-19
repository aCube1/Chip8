#include "keymap.h"

#include "cpu.h"
#include "log.h"
#include "utils.h"

#include <SDL_keyboard.h>
#include <SDL_scancode.h>

/* Default scancode layout.
	1 | 2 | 3 | 4
	Q | W | E | R
	A | S | D | F
	Z | X | C | V
*/

void keymap_load_default(keymap_t *map) {
	map->chip8_keys[0x1] = SDL_SCANCODE_1;
	map->chip8_keys[0x2] = SDL_SCANCODE_2;
	map->chip8_keys[0x3] = SDL_SCANCODE_3;
	map->chip8_keys[0xC] = SDL_SCANCODE_4;

	map->chip8_keys[0x4] = SDL_SCANCODE_Q;
	map->chip8_keys[0x5] = SDL_SCANCODE_W;
	map->chip8_keys[0x6] = SDL_SCANCODE_E;
	map->chip8_keys[0xD] = SDL_SCANCODE_R;

	map->chip8_keys[0x7] = SDL_SCANCODE_A;
	map->chip8_keys[0x8] = SDL_SCANCODE_S;
	map->chip8_keys[0x9] = SDL_SCANCODE_D;
	map->chip8_keys[0xE] = SDL_SCANCODE_F;

	map->chip8_keys[0xA] = SDL_SCANCODE_Z;
	map->chip8_keys[0x0] = SDL_SCANCODE_X;
	map->chip8_keys[0xB] = SDL_SCANCODE_C;
	map->chip8_keys[0xF] = SDL_SCANCODE_V;
}

void keymap_get_keystate(SDL_Event *event, keymap_t map, uint8_t keystate[KEYS_COUNT]) {
	SDL_Scancode scancode = event->key.keysym.scancode;

	for (uint8_t i = 0; i < KEYS_COUNT; i += 1) {
		if (scancode == map.chip8_keys[i]) {
			/* If key is pressed, set keystate to 1, if not, set keystate to 0. */
			keystate[i] = event->key.state == SDL_PRESSED ? 1 : 0;
			break;
		}
	}
}
