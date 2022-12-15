#include "utils.h"

#include "log.h"

#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 512

/* TODO: Read file in a better way. */
int8_t get_file_content(file_t *file, FILE *origin) {
	if (origin == NULL || origin == NULL) {
		log_error("Unable read NULL file.");
		return STATUS_ERROR;
	}

	uint32_t file_size = 0;
	char tmp_buffer[MAX_BUFFER_SIZE + 1] = {0}; /* Temporary buffer to append content */
	char *file_content = calloc(MAX_BUFFER_SIZE, sizeof(char)); /* Content buffer */
	if (file_content == NULL) {
		log_error("Unable to allocate memory for content buffer.");
		return STATUS_ERROR;
	}

	while (!feof(origin)) {
		/* Read file and store content in to the tmp_buffer */
		size_t bytes_read = fread(tmp_buffer, sizeof(char), MAX_BUFFER_SIZE, origin);
		if (bytes_read != MAX_BUFFER_SIZE && ferror(origin) != 0) {
			log_error("An error occurried while reading file.");
			free(file_content);
			return STATUS_ERROR;
		}
		file_size += bytes_read; /* Store file lenght */

		/* Make buffer larger to put more memory. */
		char *new_content = realloc(file_content, file_size * sizeof(char));
		if (new_content == NULL) {
			log_error("Cannot realloc memory for: %d bytes", file_size);
			free(file_content);
			return STATUS_ERROR;
		}
		file_content = new_content;

		/* Copy tmp_buffer content to file_content */
		memcpy(
			file_content + (file_size - bytes_read), tmp_buffer, bytes_read * sizeof(char)
		);
	}

	file->content = file_content;
	file->lenght = file_size;
	return STATUS_OK;
}

void file_free(file_t *file) {
	if (file == NULL) {
		return;
	}

	free(file->content);
}
