/*
 array.c -- Array facilities;

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
 * \file       array.c
 * \brief      Array facilities
 */


#include "config.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "sysexits.h"
#include <error.h>

#include "i18n.h"
#include "errors.h"
#include "array.h"


char *
concat (const char *str, ...)
{
  va_list ap, ap2;
  size_t total = 1;		/* We have one fixed argument. */
  const char *s;
  char *result, *result_end;

  va_start (ap, str);

  /* You will need to scroll through the arguments twice: to determine
     the space needed and to copy each string.  So, backup the
     argument pointer. */
  __va_copy (ap2, ap);

  /* Determine how much space we need.  */
  for (s = str; s != NULL; s = va_arg (ap, const char *))
    total += strlen (s);

  /* We are done with the first argument pointer. */
  va_end (ap);

  /* Allocate the room needed and push an error, returning NULL, if it
     isn't possible. */
  result = malloc (total);
  if (result == NULL)
    error_push_lib (malloc, NULL, "cannot concatenate strings");//_("cannot concatenate strings"));

  /* Copy the strings.  */
  result_end = result;
  for (s = str; s != NULL; s = va_arg (ap2, const char *)) {
//    result_end = stpcpy (result_end, s);
    strcpy(result_end, s);
    result_end += strlen(s);
  }

  /* We are done with the second and last argument pointer. */
  va_end (ap2);

  /* Return the intended string concatenation */
  return result;
}

char *
xstrdup (const char *s)
{
  /* Assert the supplied string is valid. */
  assert (s != NULL);

  /* Try to copy the string to a new allocated space. */
  register char *str = strdup (s);

  /* If it is not possible exit with failure. */
  if (str == NULL) {
    //error (EX_OSERR, errno, "%s: error copying string", __func__);
    printf("%s: error copying string", __func__);
    exit(EX_OSERR);
  }

  /* If allocation succeeded return the address to it. */
  return str;
}

char *
array_remove_trailing_whitespace (char *str)
{
  /* This pointer will be place in the strings' end and will be
     decremented until there is no more whitespace.  */
  char *j;

  /* Assert that the string is valid. */
  assert (str != NULL);

  /* If the string is empty there is nothing to do, so just return. */
  if (str[0] == '\0') return str;

  /* Find the last non-null character of the string. */
  j = strchr (str, '\0') - 1;

  /* Find the first non-white space character backwards. */
  while (isspace (*j)) j--;

  /* Mark the strings' new end */
  *(j + 1) = '\0';

  /* Reallocate the string to the new size. */
  return realloc (str, j - str + 2);
}
