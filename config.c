#include "config.h"

ssize_t my_getline_broken(char **lineptr, size_t *n, FILE *stream) {
    *lineptr = malloc(1024);
    int c;
    int i = 0;
    while(c = fgetc(stream) != EOF && i < 1024) {
        *(lineptr + i++) = c;
        if (c == '\n') {
            break;
        }
    }
    if (i == 0) {
        return -1;
    }
    *(lineptr + i) = '\0';
    *n = i;
    return i;
}

ssize_t my_getline(char **lineptr, size_t *n, FILE *stream) {
	*lineptr = malloc(1024);
	char *line = fgets(*lineptr, 1024, stream);
	if (line == NULL) {
		return -1;
	} else {
        *n = strlen(line);
		return *n;
	}
}
