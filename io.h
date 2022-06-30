/*
 io.h -- Input/Output convenience functions;

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
 * \file       io.h
 * \brief      Input/Output convenience functions
 */


#ifndef CCD2CUE_IO_H
#define CCD2CUE_IO_H

#include <stdio.h>

/**
 * Optimize reading from and writing to STREAM with MODE.
 *
 * \param[in] stream  The stream;
 * \param[in] mode    One of _IOFBF, _IOLBF or _IONBF;
 *
 * \return
 * - =0  on success;
 * - <0  on failure;
 *
 * \since 0.2
 *
 * This function adjust buffer to the optimal block size for reading
 * from and writing to STREAM with MODE.
 *
 * The mode parameter accepted values has the following meanings:
 *
 *- _IOFBF: fully buffered;
 *- _IOLBF: line buffered;
 *- _IONBF: unbuffered;
 *
 * \sa [Controlling Which Kind of Buffering] (https://gnu.org/software/libc/manual/html_node/Controlling-Buffering.html#Controlling-Buffering)
 *
 **/

int io_optimize_stream_buffer (FILE *stream, int mode)
  __attribute__ ((nonnull));

/**
 * Write a data block to stream.
 *
 * \param[in]   data    Data array;
 * \param[in]   size    Object size in bytes;
 * \param[in]   count   Number of objects;
 * \param[out]  stream  Output stream;
 *
 * \return Return COUNT;
 *
 * \since 0.2
 *
 * This function writes up to COUNT objects of size SIZE from the
 * array DATA, to the stream STREAM.  The return value is always
 * COUNT.
 *
 * If the call fails for an obscure error like running out of space,
 * the function exits with error.  Therefore, this function only
 * returns with success.
 *
 * This function is just a wrapper around 'fwrite' that adds fatal
 * error handling.
 *
 * \sa [Block Input/Output] (https://gnu.org/software/libc/manual/html_node/Block-Input_002fOutput.html#Block-Input_002fOutput)
 *
 */

size_t xfwrite (const void *data, size_t size, size_t count, FILE *stream)
  __attribute__ ((nonnull));

/**
 * Write a character to stream.
 *
 * \param[in]   c       Character;
 * \param[out]  stream  Output stream;
 *
 * \return Return C;
 *
 * \since 0.2
 *
 * The 'fputc' function converts the character C to type 'unsigned
 * char', and writes it to the stream STREAM.  This function always
 * returns the character C.
 *
 * If the call fails for an obscure writing error, the function exits
 * with error.  Therefore, this function only returns with success.
 *
 * This function is just a wrapper around 'putc' macro that adds fatal
 * error handling.
 *
 * \sa [Simple Output] (https://gnu.org/software/libc/manual/html_node/Simple-Output.html#Simple-Output)
 *
 */

int xputc (int c, FILE *stream)
  __attribute__ ((nonnull));

/**
 * Print the optional arguments under the control of the template
 * string TEMPLATE to the stream STREAM.
 *
 * \param[out]  stream    Output stream;
 * \param[in]   template  Template string;
 * \param[in]   ...       Template arguments;
 *
 * \return Return the number of characters printed.
 *
 * \since 0.2
 *
 * If the call fails for an obscure writing error, the function exits
 * with error.  Therefore, this function only returns with success.
 *
 * This function is just a wrapper around 'vfprintf' function that is
 * meant to add fatal error handling to 'fprintf' function.
 *
 * \sa [Formatted Output Functions] (https://gnu.org/software/libc/manual/html_node/Formatted-Output-Functions.html#Formatted-Output-Functions)
 *
 */

int xfprintf (FILE *stream, const char *template, ...)
  __attribute__ ((nonnull));

#endif	/* CCD2CUE_IO_H */
