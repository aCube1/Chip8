#ifndef _CONFIGS_H_
#define _CONFIGS_H_

#include <stdint.h>

#define MAX_FILEPATH_SIZE 1024

typedef struct {
	char rom_filepath[MAX_FILEPATH_SIZE];
	int16_t width;	/* Window Width */
	int16_t height; /* Window Height */
} configs_t;

void cfg_parse_options(configs_t *config, int argc, const char *argv[]);

#endif /* _CONFIGS_H_ */
