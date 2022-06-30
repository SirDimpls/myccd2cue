/*
 io.c -- Input/Output convenience functions;

 Copyright (C) 2013, 2014, 2015 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * \file       io.c
 * \brief      Input/Output convenience functions
 */


#include "config.h"
#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include "sysexits.h"
#include <error.h>
#include <stdarg.h>
#include <errno.h>

#include "i18n.h"
#include "io.h"


int
io_optimize_stream_buffer (FILE *stream, int mode)
{
  /* Information about STREAM's attributes; */
  struct stat stat;
  /* STREAM's file descriptor number; */
  int fd;

  /* Assert that STREAM is not NULL. */
  assert (stream != NULL);

  /* Assert that MODE is valid. */
  assert (mode == _IOFBF || mode == _IOLBF || mode == _IONBF);

  /* Get the file descriptor associated with STREAM.  Return with
     failure if you cannot. */
  fd = fileno (stream);
  if (fd == -1) return -1;

  /* Get the information about STREAM's attributes. */
  if (fstat (fd, &stat) == -1) return -1;

  /* Adjust buffer to the optimal block size for reading from and
     writing to STREAM with MODE. */
  size_t st_blksize = 512; /* as defined in linux I think */
  if (setvbuf (stream, NULL, mode, st_blksize) != 0) return -1;

  /* Return success. */
  return 0;
}

size_t
xfwrite (const void *data, size_t size, size_t count, FILE *stream)
{
  /* Assert DATA is a valid pointer. */
  assert (data != NULL);

  /* Assert STREAM is a valid pointer. */
  assert (stream != NULL);

  /* Write to stream, and if occurs an error exit with failure. */
  if (fwrite (data, size, count, stream) != count){
    exit(EX_OSERR);
  }
    //error (EX_OSERR, errno, _("%s: error writing to stream"), __func__);

  /* Return the number of objects actually written. */
  return count;
}

int
xputc (int c, FILE *stream)
{
  /* Assert STREAM is a valid pointer. */
  assert (stream != NULL);

  /* Write to stream, and if occurs an error exit with failure. */
  if (putc (c, stream) == EOF) {
    exit(EX_OSERR);
  }
    //error (EX_OSERR, errno, _("%s: error writing to stream"), __func__);

  /* Return the character just written. */
  return c;
}

int
xfprintf (FILE *stream, const char *template, ...)
{
  va_list ap;			/* Argument pointer; */
  int retval;			/* Return value; */

  /* Assert STREAM is a valid pointer. */
  assert (stream != NULL);

  /* Assert template is a valid string. */
  assert (template != NULL);

  /* Initialize argument pointer. */
  va_start (ap, template);

  /* Print to stream. */
  retval = vfprintf (stream, template, ap);

  /* If occurred an output error exit with failure. */
  if (retval < 0)
    exit(EX_OSERR);
    //error (EX_OSERR, errno, _("%s: error writing to stream"), __func__);

  /* Return the number of characters just written. */
  return retval;
}
