/*
 crc.h -- Cyclic Redundancy Check;

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
 * \file       crc.h
 * \brief      Cyclic Redundancy Check
 */


#ifndef CCD2CUE_CRC_H
#define CCD2CUE_CRC_H

#include <stdint.h>
#include <stddef.h>

/* Polynomials */
#define P16CCITT_N 0x1021 	/**< CRC-16-CCITT Normal */

/**
 * Calculate a negated 16 bit Cyclic Redundancy Check using a normal
 * CCITT polynomial.
 *
 * \param[in]  message  A pointer to the message.
 * \param[in]  length   The length of the message in bytes.
 *
 * \return Return the negated normal CRC-16-CCITT.
 *
 * \note This function never raises an error.
 *
 * \since 0.2
 *
 * This function computes the negated 16 bit CRC using the polynomial
 * P16CCITT_N (0x1021).
 *
 * This function is used to calculate the checksum for _CD-Text_
 * entries as required by the _CDT file_ format in the ::ccd2cdt
 * function.
 *
 * \sa cdt_entry.crc
 *
 */

uint16_t crc16 (const void *message, size_t length)
  __attribute__ ((nonnull, warn_unused_result, pure));

#endif	/* CCD2CUE_CRC_H */
