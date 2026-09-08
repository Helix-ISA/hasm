#include "lexer/lexer.h"
#include "lexer/token.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static b8 lexer_debug = false;

static void print_token(hx_token token)
{
	printf(
		"%-10s  line=%u  column=%u  text=\"%.*s\"\n",
		token_type_name(token.type),
		token.line,
		token.column,
		(int)token.text_length,
		token.text
	);
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		return 1;
	}

	for (s32 i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--lexer-debug") == 0) {
			lexer_debug = true;
			break;
		}
	}

	FILE *file = fopen(argv[1], "rb");

	if (file == NULL) {
		perror(argv[1]);
		return 1;
	}

	if (fseek(file, 0, SEEK_END) != 0) {
		perror("fseek");
		fclose(file);
		return 1;
	}

	u64 file_size = ftell(file);

	if (file_size < 0) {
		perror("ftell");
		fclose(file);
		return 1;
	}

	rewind(file);

	char *source = malloc((u32)file_size + 1);

	u32 bytes_read = fread(source, 1, file_size, file);

	fclose(file);

	if (bytes_read != file_size) {
		fprintf(stderr, "failed to read file\n");
		free(source);
		return 1;
	}

	source[file_size] = '\0';

	hx_lexer lexer;

	lexer_init(&lexer, source, bytes_read);

	if (lexer_debug) {
		for (;;) {
			hx_token token = lexer_next(&lexer);

			print_token(token);

			if (token.type == TOKEN_EOF)
				break;

		}
	}

	free(source);

	return 0;

}
