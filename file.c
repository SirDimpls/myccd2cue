/*
 file.c -- File and file name handling;

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
 * \file       file.c
 * \brief      File and file name handling
 */


#define _GNU_SOURCE

#include "config.h"
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "errors.h"
#include "file.h"


char *
make_reference_name (const char *filename, const int dirname_flag)
{
  char *str, *str_end;

  /* Don't modify the original string 'FILENAME', generate your own
     copy of it in 'str'.  If 'dirname_flag' is true, conserve the
     components names, otherwise only take the base name. */
  if (dirname_flag) str = xstrdup (filename);
  else str = xstrdup (basename (filename));

  /* To make a reference name it's necessary to discard any possible
     extension from the file name.  If 'STR' has an extension,
     it's all trailing the last dot.  Try to find that last dot.*/
  str_end = strrchr (str, '.');

  /* If you have found the referred dot, 'STR' has an extension, thus
     free the space occupied by it and mark the new end of 'STR': the
     location of that last dot. */
  if (str_end != NULL)
    {
      str = realloc (str, str_end - str + 1);
      *str_end = '\0';
    }

  /* Return to the caller the wanted reference name */
  return str;
}
