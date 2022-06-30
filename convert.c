/*
 convert.c -- CCD sheet structure to CUE sheet structure conversion;

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
 * \file       convert.c
 * \brief      _CCD sheet_ structure to _CUE sheet_ structure conversion
 */


#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <error.h>
#include "sysexits.h"

#include "errors.h"
#include "array.h"
#include "memory.h"
#include "i18n.h"
#include "io.h"
#include "ccd.h"
#include "cue.h"
#include "cdt.h"
#include "crc.h"
#include "convert.h"


/**
 * Convert frames based time to MSF.
 *
 * \param[in]   frames  Frames;
 * \param[out]  msf     MSF structure;
 *
 * \since 0.2
 *
 * This function converts raw frames based time specification as
 * supplied by CCD sheet format, on INDEX entries inside TRACK
 * sections, to MSF (Minutes/Seconds/Frames) structure, as used in the
 * CUE sheet format, on INDEX entries inside TRACK sections.  The
 * temporal correspondence used by the conversion algorithm is as
 * follow:
 *
 *- 1 second = 75 frames
 *- 1 minute = 60 seconds
 *
 * \sa ::ccd_TRACK.INDEX and ::cue_TRACK.INDEX
 *
 */

static void frames2msf (int frames, struct cue_time *msf)
  __attribute__ ((nonnull));


/* Frame temporal definition */
#define FRAMES_PER_SECOND 75 	/**< How many frames a second has; */
#define SECONDS_PER_MINUTE 60	/**< How many seconds a minute has; */
  /** How many frames a minute has; */
#define FRAMES_PER_MINUTE (FRAMES_PER_SECOND * SECONDS_PER_MINUTE)

static void
frames2msf (int frames, struct cue_time *msf)
{
  /* Assert frames time is positive.  */
  assert(frames >= 0);

  /* Assert MSF is pointing to a valid structure. */
  assert(msf != NULL);

  /* Do the conversion according to the definition above. */
  msf->minutes = frames / FRAMES_PER_MINUTE;
  msf->seconds = (frames % FRAMES_PER_MINUTE) / FRAMES_PER_SECOND;
  msf->frames = (frames % FRAMES_PER_MINUTE) % FRAMES_PER_SECOND;

  /* Mark the MSF structure as initialized. */
  msf->initialized = 1;
}

struct cue *
ccd2cue (const struct ccd *ccd, const char *img_name, const char *cdt_name)
{
  struct cue *cue;		/* Pointer to the resulting CUE structure. */

  /* Assert the CCD structure is valid. */
  assert(ccd != NULL);

  /* Assert the image file name is valid. */
  assert(img_name != NULL);

  /* Assert the CDT file name is valid. */
  assert(cdt_name != NULL);

  /* Initialize the CUE structure. */
  cue = cue_init (1);

  /* If there is MCN add a CATALOG entry.  */
  if (ccd->Disc.CATALOG[0] != '\0') strncpy (cue->CATALOG, ccd->Disc.CATALOG, 13 + 1);

  /* If there is CDText data add a CDTEXTFILE entry. */
  if (ccd->CDText.Entries != 0)
    cue->CDTEXTFILE = xstrdup (cdt_name);

  /* Add FILE entry. */
  cue->FileEntries = 1;
  cue->FILE = cue_FILE_init (cue->FileEntries);
  cue->FILE[0].filename = xstrdup (img_name);
  cue->FILE[0].filetype = BINARY;

  /* If there is any TRACK section, process it. */
  if (ccd->TrackEntries > 0)
    {
      size_t i;			/* TRACK index; */

      /* Allocate the CUE structure's TRACK array.  */
      cue->FILE[0].TrackEntries = ccd->TrackEntries;
      cue->FILE[0].FirstTrack = 1;
      cue->FILE[0].TRACK = cue_TRACK_init (cue->FILE[0].TrackEntries + 1);

      /* Add each TRACK section. */
      for (i = cue->FILE[0].FirstTrack; i <= ccd->TrackEntries; i++)
	{
	  size_t j;		/* INDEX index; */

	  /* Add datatype entry */
	  switch (ccd->TRACK[i].MODE)
	    {
	    case 0:		/* 0 means AUDIO */
	      cue->FILE[0].TRACK[i].datatype = AUDIO_2352;
	      break;
	    case 1:		/* 1 means MODE1/2352 */
	      cue->FILE[0].TRACK[i].datatype = MODE1_2352;
	      break;
	    case 2:		/* 2 means MODE2/2352 */
	      cue->FILE[0].TRACK[i].datatype = MODE2_2352;
	      break;
	    default:		/* If you got here, the program
				   certainly has a bug. */
	      //error (1, 0, _("unknown track data type %d; please report a bug"),
		   //  ccd->TRACK[i].MODE);
          exit(EX_UNAVAILABLE);
	    }

	  /* If there is a FLAGS entry for this track, add it. */
	  if (ccd->TRACK[i].FLAGS != NULL)
	    cue->FILE[0].TRACK[i].FLAGS = xstrdup (ccd->TRACK[i].FLAGS);

	  /* If there is ISRC entry for this track, add it. */
	  if (ccd->TRACK[i].ISRC[0] != '\0')
	    strncpy (cue->FILE[0].TRACK[i].ISRC, ccd->TRACK[i].ISRC, 12 + 1);

	  /* Allocate TRACK structure's INDEX array. */
	  cue->FILE[0].TRACK[i].INDEX = xmalloc (sizeof (*cue->FILE[0].TRACK[i].INDEX)
					 * ccd->TRACK[i].IndexEntries);
	  cue->FILE[0].TRACK[i].IndexEntries = ccd->TRACK[i].IndexEntries;

	  /* Add each INDEX entry. */
	  for (j = 0; j < ccd->TRACK[i].IndexEntries; j++)
	    if (ccd->TRACK[i].INDEX[j] != -1)
	      frames2msf (ccd->TRACK[i].INDEX[j], &cue->FILE[0].TRACK[i].INDEX[j]);
	    else cue->FILE[0].TRACK[i].INDEX[j].initialized = 0;
	}

    }

  /* Return success. */
  return cue;
}

int
ccd2cdt (const struct ccd *ccd, struct cdt *cdt)
{
  size_t i;			/* CDT entry index; */

  /* Assert the CCD structure is valid. */
  assert(ccd != NULL);

  /* Assert the CDT structure is valid. */
  assert(cdt != NULL);

  /* Allocate the CDT structure's entries. */
  cdt->entries = ccd->CDText.Entries;
  cdt->entry = xmalloc (sizeof (*cdt->entry) * cdt->entries);

  /* Fill each CDT entry. */
  for (i = 0; i < ccd->CDText.Entries; i++)
    {
      uint16_t crc;		/* Negated CRC-16 (CCITT) */

      /* Copy the CDT data itself. */
      memcpy (&cdt->entry[i].data, &ccd->CDText.Entry[i], sizeof (ccd->CDText.Entry[i]));

      /* Calculate the negated CRC-16 (CCITT). */
      crc = crc16 (&cdt->entry[i].data, sizeof (cdt->entry[i].data));
      cdt->entry[i].crc[0] = (crc >> 8) & 0xff;
      cdt->entry[i].crc[1] = crc & 0xff;
    }

  /* Return the number of CDT entries. */
  return cdt->entries;
}
