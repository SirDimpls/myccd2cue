#ifndef __CONFIG_H__
#define __CONFIG_H__

#define _GNU_SOURCE

#include <corecrt.h>
#include <stdio.h>

// if (getline (&line, &line_size, stream) == -1 && errno != 0)
extern ssize_t my_getline(char **lineptr, size_t *n, FILE *stream);

#define _(a) a

#endif // __CONFIG_H
