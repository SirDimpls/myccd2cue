/*
 file.h -- File and file name handling;

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
 * \file       file.h
 * \brief      File and file name handling
 */


#ifndef CCD2CUE_FILE_H
#define CCD2CUE_FILE_H

/**
 * \fn char * make_reference_name (const char *filename, const int dirname_flag)
 *
 * Make a reference name for the given file name.
 *
 * \param[in] filename File name used to derive the reference name.
 * \param[in] dirname_flag Whether to keep components names.
 *
 * \return
 * + Success: the reference name as a new malloc'ed string.
 * + Failure: a NULL pointer.
 *
 * \note This function can fail only if it's impossible to malloc the
 * resulting reference name string.
 *
 * \since 0.2
 *
 * ### Concept ###
 *
 * In this context, a file name's _reference name_ is a name used to
 * derive another names for correlate files.  For instance, if we have
 * the file 'qux/foo.bar' a correlate file name would be 'qux/foo.baz'
 * or even 'foo.baz', where the reference names are 'qux/foo' and
 * 'foo', respectively.
 *
 * So, basically, a _reference name_ is the original name without
 * extension, if it has one, regardless if one takes its full
 * component names or only the base name.
 *
 */

char * make_reference_name (const char *filename, const int dirname_flag);

#endif	/* CCD2CUE_FILE_H */
