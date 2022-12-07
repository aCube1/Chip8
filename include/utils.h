#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <stdio.h>

#define STATUS_ERROR -1
#define STATUS_OK    0

/* Window settings */
#define TITLE          "Chip8 Emulator"
#define DEFAULT_WIDTH  800
#define DEFAULT_HEIGHT 600

/* File management. */
typedef struct {
	char *content;
	uint32_t lenght;
} file_t;

int8_t get_file_content(file_t *file, FILE *origin);
void file_free(file_t *file);

#endif /* _UTILS_H_ */
