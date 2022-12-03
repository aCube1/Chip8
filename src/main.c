#include "configs.h"
#include "core.h"
#include "log.h"
#include "utils.h"

#include <stdint.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	configs_t configs = {
		.rom_filepath = "",
		.width = DEFAULT_WIDTH,
		.height = DEFAULT_HEIGHT,
	};

	if (cfg_parse_options(&configs, argc, argv) != STATUS_OK) {
		log_fatal("Unable to read command-line arguments!");
		return EXIT_FAILURE;
	}

	if (core_init(configs) != STATUS_OK) {
		log_fatal("Unable to initialize Engine Core!");
		return EXIT_FAILURE;
	}

	if (core_run() != STATUS_OK) {
		log_fatal("An error occurried running Engine Core!");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
