/*
 errors.h -- Error treatment;

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
 * \file       errors.h
 * \brief      Error treatment
 */


#ifndef CCD2CUE_ERROR_H
#define CCD2CUE_ERROR_H

#include <stddef.h>
#include <errno.h>


/**
 * Push error message into error stack.
 *
 * \param[in] function_name  Caller's name;
 * \param[in] template       Template string;
 * \param[in] ...            Template arguments;
 *
 * \since 0.2
 *
 * The message that is pushed into error stack is built from the
 * printf-like template and its arguments with a leading
 * "FUNCTION_NAME: ".
 *
 */

void error_push_f (const char function_name[], const char *template, ...)
  __attribute__ ((format (printf, 2, 3)));

/**
 * Print all error stack's messages and empty error stack.
 *
 * \since 0.2
 *
 * The messages are shown in a bottom-top fashion.
 *
 **/

void error_pop_f (void);


/**
 * Push an error message for the executing function and return STATUS.
 *
 * \param[in] status    Return status;
 * \param[in] template  Template string;
 * \param[in] ...       Template arguments;
 *
 * \since 0.2
 *
 * This function pushes an executing function error message, based on
 * TEMPLATE and its arguments, and returns STATUS.  It is useful for
 * reporting errors that do not directly depend on a C standard
 * library primitive.
 *
 * \sa ::error_push_lib
 */

#define error_push(status, template, ...)			\
  do								\
    {								\
      error_push_f (__func__, template, ## __VA_ARGS__);	\
      return status;						\
    }								\
  while (0)

/**
 * Push a C standard library's function error message followed by an
 * executing function error message and return STATUS.
 *
 * \param[in] function  C standard library's function identifier;
 * \param[in] status    Return status;
 * \param[in] template  Template string;
 * \param[in] ...       Template arguments;
 *
 * \since 0.2
 *
 * This function pushes a C standard library's function error message
 * for FUNCTION and another error message for the executing function,
 * based on TEMPLATE and its arguments, and returns STATUS.  It is
 * useful for reporting a function error that is caused by a C
 * standard library one.
 *
 * The FUNCTION identifier will be turned into a string.
 *
 * \sa ::error_push
 *
 */

#define error_push_lib(function, status, template, ...)	\
  do							\
    {							\
      error_push_f (#function, "%s", strerror (errno));	\
      error_push (status, template, ## __VA_ARGS__);	\
    }							\
  while (0)

/**
 * Print all error stack's messages followed by an error message for
 * the executing function and return STATUS.
 *
 * \param[in] status    Return status;
 * \param[in] template  Template string;
 * \param[in] ...       Template arguments;
 *
 * \since 0.2
 *
 * This function empty the error stack.  The executing function error
 * message is the last printed and it is based on TEMPLATE and its
 * arguments.  It is useful for reporting all accumulated errors,
 * inclusive that just founded, before returning.
 *
 * \sa ::error_pop_lib
 */

#define error_pop(status, template, ...)			\
  do								\
    {								\
      error_push_f (__func__, template, ## __VA_ARGS__);	\
      error_pop_f ();						\
      return status;						\
    }								\
  while (0)

/**
 * Print all error stack's messages, followed by a C standard
 * library's function error message, and another for the executing
 * function, and finally return STATUS.
 *
 * \param[in] function  C standard library's function identifier;
 * \param[in] status    Return status;
 * \param[in] template  Template string;
 * \param[in] ...       Template arguments;
 *
 * \since 0.2
 *
 * This function empty the error stack.  The FUNCTION identifier will
 * be turned into a string and will be part of the penultimate error
 * message.  The executing function error message is the last printed
 * and it is based on TEMPLATE and its arguments.  It is useful for
 * reporting all accumulated errors, inclusive that just founded, and
 * the another from a primitive function which caused it, before
 * returning.
 *
 * \sa ::error_pop
 *
 */

#define error_pop_lib(function, status, template, ...)		\
  do								\
    {								\
      error_push_f (#function, "%s", strerror (errno));		\
      error_pop (status, template, ## __VA_ARGS__);		\
    }								\
  while (0)

/**
 * Do just like ::error_pop but instead of returning STATUS, exit with
 * STATUS.
 *
 * \param[in] status    Return status;
 * \param[in] template  Template string;
 * \param[in] ...       Template arguments;
 *
 * \since 0.2
 *
 * This is ::error_pop fatal version.  It is useful to use it in the
 * last call of a chain where, due to the errors found, it is not
 * desirable to return.
 *
 * \sa ::error_fatal_pop_lib
 *
 */

#define error_fatal_pop(status, template, ...)			\
  do								\
    {								\
      error_push_f (__func__, template, ## __VA_ARGS__);	\
      error_pop_f ();						\
      exit (status);						\
    }								\
  while (0)

/**
 * Do just like ::error_pop_lib but instead of returning STATUS, exit
 * with STATUS.
 *
 * \param[in] function  C standard library's function identifier;
 * \param[in] status    Return status;
 * \param[in] template  Template string;
 * \param[in] ...       Template arguments;
 *
 * \since 0.2
 *
 * This is ::error_pop_lib fatal version.  It is useful to use it in
 * the last call of a chain where, due to the errors found, it is not
 * desirable to return.
 *
 * \sa ::error_fatal_pop
 *
 */

#define error_fatal_pop_lib(function, status, template, ...)	\
  do								\
    {								\
      error_push_f (#function, "%s", strerror (errno));		\
      error_fatal_pop (status, template, ## __VA_ARGS__);	\
    }								\
  while (0)

#endif	/* CCD2CUE_ERROR_H */
