/*
 array.h -- Array facilities;

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
 * \file       array.h
 * \brief      Array facilities
 */


#ifndef CCD2CUE_ARRAY_H
#define CCD2CUE_ARRAY_H

/**
 * Concatenate strings
 *
 * \param[in] str,...  The strings to concatenate.
 *
 * \return
 * + Success: the result as a new malloc'ed string.
 * + Failure: a NULL pointer.
 *
 * \attention The last parameter must be a NULL pointer.
 *
 * \note: This function can fail only if it's impossible to malloc the
 * resulting concatenated string.
 *
 * \since 0.2
 *
 */

char * concat (const char *str, ...)
  __attribute__ ((warn_unused_result));

/**
 * Copy a null-terminated string into a newly allocated string.
 *
 * \param[in]  s  String;
 *
 * \return  A pointer to the newly allocated string that is a copy of
 *          the supplied string;
 *
 * \since 0.2
 *
 * This function returns a pointer to a newly allocated string, that
 * is a copy of the supplied one, or exits with failure if the string
 * could not be allocated.  It is just a wrapper around 'strdup'.
 *
 * \sa [Copying and Concatenation] (https://gnu.org/software/libc/manual/html_node/Copying-and-Concatenation.html#Copying-and-Concatenation)
 *
 */
char * xstrdup (const char *s);

/**
 * Remove trailing white space from a string.
 *
 * \param[in] str  The string
 *
 * \return A pointer to the new string.
 *
 * \since 0.2
 *
 * A pointer is returned because the string is reallocated to only
 * occupy the necessary space.  It can be the same pointer provided by
 * the caller, but it is not guaranteed.
 *
 */
char * array_remove_trailing_whitespace (char *str)
  __attribute__ ((nonnull, warn_unused_result));

#endif	/* CCD2CUE_ARRAY_H */
