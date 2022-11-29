#include "SDL2/SDL.h"
#include "core.h"
#include "utils.h"

#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (core_init(argc, argv) != STATUS_OK) {
		return EXIT_FAILURE;
	}

	if (core_run() != STATUS_OK) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
