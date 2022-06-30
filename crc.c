/*
 crc.c -- Cyclic Redundancy Check;

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
 * \file       crc.c
 * \brief      Cyclic Redundancy Check
 */


#include "config.h"
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "crc.h"


uint16_t
crc16 (const void *message, size_t length)
{
  /* Assert the message pointer is valid. */
  assert (message != NULL);

  size_t i;			/* Offset inside the message;  */
  int j;			/* Bit offset inside current message's
				   byte; */
  uint16_t crc = 0;		/* CRC accumulator; */
  uint16_t polynomial = P16CCITT_N; /* CRC polynomial; */

  /* Process all bytes from message. */
  for (i = 0; i < length; i++)
    {
      crc ^= (uint16_t) *((uint8_t *) message + i) << 8;
      /* Process all bits from the current byte. */
      for (j = 0; j < 8; j++)
	crc = crc & (1 << 15) ? (crc << 1) ^ polynomial : crc << 1;
    }

  /* Return the negated CRC. */
  return ~crc;
}
