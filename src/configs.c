#include "configs.h"

#include "argparse.h"
#include "log.h"
#include "utils.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

enum {
	LOG_ALL = -1,
	LOG_QUIET = 0,
};

static const char *const usages[] = {
	"Chip8 [options] <path-to-rom>",
	NULL,
};

#ifdef USE_DEBUG
static int32_t log_mode = LOG_ALL;
#else
static int32_t log_mode = LOG_QUIET;
#endif

void cfg_parse_options(configs_t *config, int argc, const char *argv[]) {
	struct argparse argparse;
	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_GROUP("Basic arguments."),
		OPT_BOOLEAN(0, "quiet", &log_mode, "Disable terminal output", NULL, LOG_QUIET, 0),
		OPT_BOOLEAN(0, "verbose", &log_mode, "Enable terminal output", NULL, LOG_ALL, 0),
		OPT_GROUP("Window arguments."),
		OPT_INTEGER('w', "width", &config->width, "Set window width.", NULL, 0, 0),
		OPT_INTEGER('h', "height", &config->height, "Set window height", NULL, 0, 0),
		OPT_END(),
	};

	argparse_init(&argparse, options, usages, ARGPARSE_STOP_AT_NON_OPTION);
	argc = argparse_parse(&argparse, argc, argv);

	/* Set logging mode. */
	log_set_quiet(log_mode == LOG_QUIET);

	/* Show usage if no arguments. */
	if (argc == 0) {
		argparse_usage(&argparse);
		return;
	}

	/* Copy filepath to config struct. */
	memcpy(config->rom_filepath, argv[0], MAX_FILEPATH_SIZE - 1);
}
