/*
 memory.c -- Memory management;

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
 * \file       memory.c
 * \brief      Memory management
 */


#include "config.h"
#include <stddef.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include "sysexits.h"
#include <error.h>

#include "i18n.h"
#include "memory.h"


/**
 * Obstack failed allocation handler
 *
 * This function prints an error and exits with failure when it is not
 * possible to allocate memory on obstack functions.
 *
 * \sa [Preparing for Using Obstacks] (https://gnu.org/software/libc/manual/html_node/Preparing-for-Obstacks.html#Preparing-for-Obstacks)
 *
 **/
static void memory_obstack_alloc_failed (void);

/**
 * Obstack failed allocation handler
 *
 * The Obstack failed allocation handler specified here gets called
 * when 'obstack_chunck_alloc' fails to allocate memory.
 *
 * \sa [Preparing for Using Obstacks] (https://gnu.org/software/libc/manual/html_node/Preparing-for-Obstacks.html#Preparing-for-Obstacks)
 *
 */

void (*obstack_alloc_failed_handler) (void) = memory_obstack_alloc_failed;


void *
xmalloc (size_t size)
{
  /* Try to allocate SIZE memory bytes. */
  register void *value = malloc (size);

  /* If it is not possible exit with failure. */
  if (value == NULL) {
    exit(EX_OSERR);
    //error (EX_OSERR, errno, _("%s: error allocating memory"), __func__);
  }
  /* If allocation succeeded return the address to it. */
  return value;
}

void *
xrealloc (void *ptr, size_t newsize)
{
  /* Try to reallocate data in PTR to a chunk SIZE bytes long. */
  register void *value = realloc (ptr, newsize);

  /* If it is not possible exit with failure. */
  if (value == NULL) {
    exit(EX_OSERR);
    //error (EX_OSERR, errno, _("%s: error reallocating memory"), __func__);
  }
  /* If reallocation succeeded return the address to it. */
  return value;
}

static void
memory_obstack_alloc_failed (void)
{
  /* Print an error message and exit with failure. */

  /* TRANSLATORS: See
     http://www.gnu.org/software/libc/manual/html_node/Obstacks.html
     for context. */
    printf("%s: error allocating chunk for obstack", __func__);
  exit(EX_OSERR);
  //error (EX_OSERR, errno, _("%s: error allocating chunk for obstack"), __func__);
}
