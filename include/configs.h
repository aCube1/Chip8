#ifndef _CONFIGS_H_
#define _CONFIGS_H_

#include <stdint.h>

#define MAX_FILEPATH_SIZE 1024

#define STATUS_STOP		  1 /* Stop program execution. */
#define STATUS_CONTINUE	  2 /* Continue program execution. */

typedef struct {
	char rom_filepath[MAX_FILEPATH_SIZE];
	uint16_t clock_speed;
	int16_t width;	/* Window Width */
	int16_t height; /* Window Height */
} configs_t;

int8_t cfg_parse_options(configs_t *config, int argc, char *argv[]);

#endif /* _CONFIGS_H_ */
