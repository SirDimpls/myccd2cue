/*
 errors.c -- Error treatment;

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
 * \file       errors.c
 * \brief      Error treatment
 */


#define _GNU_SOURCE

#include "config.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "err.h"
#include "sysexits.h"
#include <error.h>

#include "i18n.h"
#include "memory.h"
#include "errors.h"


/**
 * Error stack;
 *
 * This variable holds an array of error strings.  Each time
 * ::error_push_f is called, possibly by its higher level interface
 * macros, explicitly by ::error_push or ::error_push_lib, or implicitly
 * by ::error_pop, ::error_pop_lib, ::error_fatal_pop and
 * ::error_fatal_pop_lib, it pushes a error to this stack.
 *
 */

static char **error_stack = NULL;

/**
 * Error stack entries count;
 *
 * This variable holds an integer indicating the error stack's size in
 * error messages unity.
 *
 */

static size_t error_entries = 0;


void
error_push_f (const char function_name[], const char *template, ...)
{
  va_list ap;
  char *str;

  va_start (ap, template);

  /* Allocate space on the error stack for the new error message. */
  error_stack = xrealloc (error_stack, sizeof (*error_stack) * ++error_entries);

  /* Store error message, with the supplied function name, on top of error
     stack. */
  if (vasprintf (&str, template, ap) < 0) {
    printf("%s: cannot push error into stack", __func__);
    exit(EX_OSERR);
  }
    //error (EX_OSERR, errno, _("%s: cannot push error into stack"), __func__);
  if (asprintf (&error_stack[error_entries - 1], "%s: %s", function_name, str) < 0) {
    printf("%s: cannot push error into stack", __func__);
  }
    //error (EX_OSERR, errno, _("%s: cannot push error into stack"), __func__);
  free (str);

  va_end (ap);
}


void
error_pop_f (void)
{
  size_t i;

  /* Print every error message from error stack in a bottom-top
     fashion.  Empty error stack. */

  for (i = 0; i < error_entries; i++)
    {
      //error (0, 0, "%s", error_stack[i]);
      printf("%s", error_stack[i]);
      free (error_stack[i]);
    }

  free (error_stack);
  error_stack = NULL;
  error_entries = 0;
  exit(EX_DATAERR);
}
