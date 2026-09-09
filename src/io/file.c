#include "io/file.h"
#include <stdio.h>
#include <stdlib.h>

char *read_file(FILE *file, u32 *length)
{
	if (fseek(file, 0, SEEK_END) != 0) {
		perror("fseek");
		return NULL;
	}

	long file_size = ftell(file);

	if (file_size < 0) {
		perror("ftell");
		return NULL;
	}

	rewind(file);

	char *source = malloc((u32)file_size + 1);

	if (source == NULL) {
		fprintf(stderr, "out of memory\n");
		return NULL;
	}

	u32 bytes_read = fread(source, 1, (u32)file_size, file);

	if (bytes_read != (u32)file_size) {
		fprintf(stderr, "failed to read file\n");
		free(source);
		return NULL;
	}

	source[bytes_read] = '\0';

	*length = (u32)bytes_read;

	return source;
}
