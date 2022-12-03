#include "configs.h"

#include "log.h"
#include "utils.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* TODO: Windows version. */
#if defined(__unix__) || defined(__unix) || defined(unix)
#	include <getopt.h>
#elif defined(_WIN32) || defined(WIN32)
#	error "Windows is not supported!"
#endif

enum {
	LOG_ALL = -1,
	LOG_QUIET = 0,
};

static int32_t log_mode = LOG_ALL;

static struct option long_options[] = {
	{"quiet", no_argument, &log_mode, LOG_QUIET},
	{"width", required_argument, NULL, 'w'},
	{"height", required_argument, NULL, 'h'},
	{NULL, 0, NULL, 0},
};

static const char short_options[] = "w:h:";

static void set_resolution(int16_t *value, int16_t default_value, int32_t option_index);

int8_t cfg_parse_options(configs_t *config, int argc, char *argv[]) {
	(void)config;
	int32_t opt = 0;
	int32_t option_index = 0;

	while (opt != -1) {
		opt = getopt_long(argc, argv, short_options, long_options, &option_index);

		switch (opt) {
		case 0:
			/* Do nothing if option set a flag. */
			if (long_options[option_index].flag != NULL) {
				break;
			}

			/* Trace option. */
			if (optarg == NULL) {
				log_trace("Unknown option %s.", long_options[option_index].name);
			} else {
				log_trace(
					"Unknown option %s with arg %s", long_options[option_index].name,
					optarg
				);
			}
			break;
		case 'w':
			set_resolution(&config->width, DEFAULT_WIDTH, option_index);
			break;
		case 'h':
			set_resolution(&config->height, DEFAULT_HEIGHT, option_index);
			break;
		case '?':
			log_fatal("Unable to recognize option: %s", long_options[option_index]);
			return STATUS_ERROR;
		}
	}

	if (log_mode == LOG_QUIET) {
		log_set_quiet(true);
	}

	/* Get rom filepath. */
	if (optind < argc) {
		log_info("Rom filepath provided: %s", argv[optind]);
		strncpy(config->rom_filepath, argv[optind], MAX_FILEPATH_SIZE);
	} else {
		log_fatal("No rom filepath provided!");
		return STATUS_ERROR;
	}

	return STATUS_OK;
}

static void set_resolution(int16_t *value, int16_t default_value, int32_t option_index) {
	int32_t new_resolution = strtol(optarg, NULL, 0);

	if (new_resolution == 0L || errno == ERANGE) {
		log_error(
			"Unable to get resolution from option %s with arg %s.",
			long_options[option_index].name, optarg
		);
		return;
	}

	if (new_resolution > DEFAULT_WIDTH) {
		*value = (int16_t)new_resolution; /* WARN: Precision loss. */
	} else {
		log_warn("Resolution is lower than %d. Using default...", default_value);
	}
}
