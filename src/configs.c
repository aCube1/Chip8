#include "configs.h"

#include "log.h"
#include "utils.h"

#include <cargs.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_WIDTH		800
#define DEFAULT_HEIGHT		600
#define DEFAULT_CLOCK_SPEED 400 /* Speed in Hz */
#define MAX_CLOCK_SPEED		1000

enum {
	LOG_ALL = -1,
	LOG_QUIET = 0,
};

#ifdef USE_DEBUG
static int32_t log_mode = LOG_ALL;
#else
static int32_t log_mode = LOG_QUIET;
#endif

/* This is the main configuration of options available. */
static struct cag_option options[] = {
	{
		.identifier = 'c',
		.access_letters = "c",
		.access_name = "clock",
		.value_name = "<int>",
		.description = "Set clock speed.",
	},
	{
		.identifier = 'w',
		.access_letters = NULL,
		.access_name = "width",
		.value_name = "<int>",
		.description = "Set window width.",
	},
	{
		.identifier = 'h',
		.access_letters = NULL,
		.access_name = "height",
		.value_name = "<int>",
		.description = "Set window height.",
	},
	{
		.identifier = 'v',
		.access_letters = NULL,
		.access_name = "verbose",
		.description = "Enable log output.",
	},
	{
		.identifier = 'q',
		.access_letters = NULL,
		.access_name = "quiet",
		.description = "Disable log output.",
	},
	{
		.identifier = 'a',
		.access_letters = "h",
		.access_name = "help",
		.description = "Show help message.",
	},
};

static int8_t fetch_identifier(
	cag_option_context *context, configs_t *config, char identifier
);

static void show_help_message(void);

static void set_clock(uint16_t *clock, const char *value);
static void set_width(int16_t *width, const char *value);
static void set_height(int16_t *height, const char *value);

int8_t cfg_parse_options(configs_t *config, int argc, char *argv[]) {
	char identifier;
	cag_option_context context;

	/* No arguments provided, show help text and stop the program. */
	if (argc < 2) {
		show_help_message();
		return STATUS_STOP;
	}

	*config = (configs_t){
		.rom_filepath = "",
		.clock_speed = DEFAULT_CLOCK_SPEED,
		.width = DEFAULT_WIDTH,
		.height = DEFAULT_HEIGHT,
	};

	cag_option_prepare(&context, options, CAG_ARRAY_SIZE(options), argc, argv);

	while (cag_option_fetch(&context)) {
		identifier = cag_option_get(&context);

		if (fetch_identifier(&context, config, identifier) == STATUS_STOP) {
			return STATUS_STOP;
		}
	}

	/* Set logging mode. */
	log_set_quiet(log_mode == LOG_QUIET);

	memcpy(config->rom_filepath, argv[context.index], MAX_FILEPATH_SIZE);
	config->rom_filepath[MAX_FILEPATH_SIZE - 1] = '\0';

	return STATUS_OK;
}

static int8_t fetch_identifier(
	cag_option_context *context, configs_t *config, char identifier
) {
	const char *value = cag_option_get_value(context);

	switch (identifier) {
	case 'c':
		set_clock(&config->clock_speed, value);
		break;
	case 'w':
		set_width(&config->width, value);
		break;
	case 'h':
		set_height(&config->height, value);
		break;
	case 'v':
		log_mode = LOG_ALL;
		break;
	case 'q':
		log_mode = LOG_QUIET;
		break;
	case 'a':
		show_help_message();
		return STATUS_STOP;
	}

	return STATUS_OK;
}

static void show_help_message(void) {
	/* TODO: Better help message. */
	puts("Usage: Chip8 [OPTIONS] <path-to-rom>");
	cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
}

static void set_clock(uint16_t *clock, const char *value) {
	if (value != NULL) {
		int32_t speed = strtol(value, NULL, 10);
		*clock = speed > 0 && speed < MAX_CLOCK_SPEED ? speed : DEFAULT_CLOCK_SPEED;
	}
}

static void set_width(int16_t *width, const char *value) {
	if (value != NULL) {
		int32_t size = strtol(value, NULL, 10);
		*width = size != 0 ? size : DEFAULT_WIDTH;
	}
}

static void set_height(int16_t *height, const char *value) {
	if (value != NULL) {
		int32_t size = strtol(value, NULL, 10);
		*height = size != 0 ? size : DEFAULT_HEIGHT;
	}
}
