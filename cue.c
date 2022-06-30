/*
 cue.c -- CUE sheet format structure;

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
 * \file       cue.c
 * \brief      CUE sheet format structure
 */


#include "config.h"
#include <assert.h>

#include "memory.h"
#include "io.h"
#include "cue.h"


/**
 * File type strings
 *
 * This array just associates the ::cue_filetype enumeration with its
 * string representation.  It is used by ::cue2stream function to
 * output a ::cue structure to a CUE stream.
 *
 * \sa ::cue_filetype and ::cue2stream
 *
 */

static char *filetype[] =
  { [BINARY] "BINARY",
    [MOTOROLA] "MOTOROLA",
    [AIFF] "AIFF",
    [WAVE] "WAVE",
    [MP3] "MP3" };

/**
 * Data type strings
 *
 * This array just associates the ::cue_datatype enumeration with its
 * string representation.  It is used by ::cue2stream function to
 * output a ::cue structure to a CUE stream.
 *
 * \sa ::cue_datatype and ::cue2stream
 *
 */

static char *datatype[] =
  { [AUDIO_2352] "AUDIO",
    [CDG_2448] "CDG",
    [MODE1_2048] "MODE1/2048",
    [MODE1_2352] "MODE1/2352",
    [MODE2_2336] "MODE2/2336",
    [MODE2_2352] "MODE2/2352",
    [CDI_2336] "CDI/2336",
    [CDI_2352] "CDI/2352" };


struct cue *
cue_init (size_t entries)
{
  struct cue *cue;	/* Pointer to the newly allocated
			   array of structures. */
  size_t entry;		/* Index of entry.  For iteration. */

  /* Allocate CUE structures. */
  cue = xmalloc (sizeof (*cue) * entries);

  /* Initialize CUE structures. */
  for (entry = 0; entry < entries; entry++)
    {
      cue[entry].CATALOG[0] = '\0';
      cue[entry].CDTEXTFILE = NULL;
      cue[entry].PERFORMER = NULL;
      cue[entry].SONGWRITER = NULL;
      cue[entry].TITLE = NULL;
      cue[entry].FILE = NULL;
      cue[entry].FileEntries = 0;
    }

  /* Return the newly allocated array of CUE structures. */
  return cue;
}

struct cue_FILE *
cue_FILE_init (size_t entries)
{
  struct cue_FILE *file;	/* Pointer to the newly allocated
				   array of structures. */
  size_t entry;			/* Index of entry.  For iteration. */

  /* Allocate FILE structures. */
  file = xmalloc (sizeof (*file) * entries);

  /* Initialize FILE structures. */
  for (entry = 0; entry < entries; entry++)
    {
      file[entry].filename = NULL;
      file[entry].filetype = MOTOROLA;
      file[entry].TRACK = NULL;
      file[entry].TrackEntries = 0;
      file[entry].FirstTrack = 1;
    }

  /* Return the newly allocated array of FILE structures. */
  return file;
}

struct cue_TRACK *
cue_TRACK_init (size_t entries)
{
  struct cue_TRACK *track;	/* Pointer to the newly allocated
				   array of structures. */
  size_t entry;			/* Index of entry.  For iteration. */

  /* Allocate TRACK structures. */
  track = xmalloc (sizeof (*track) * entries);

  /* Initialize TRACK structures. */
  for (entry = 0; entry < entries; entry++)
    {
      track[entry].datatype = 0;
      track[entry].FLAGS = NULL;
      track[entry].ISRC[0] = '\0';
      track[entry].PERFORMER = NULL;
      track[entry].SONGWRITER = NULL;
      track[entry].TITLE = NULL;
      track[entry].PREGAP.initialized = 0;
      track[entry].INDEX = NULL;
      track[entry].IndexEntries = 0;
      track[entry].POSTGAP.initialized = 0;
    }

  /* Return the newly allocated array of TRACK structures. */
  return track;
}

int
cue2stream (const struct cue *cue, FILE *stream)
{
  int file; 			/* File number (zero based); */

  /* Assert the cue structure is valid. */
  assert(cue != NULL);

  /* Assert the stream is valid. */
  assert(stream != NULL);

  /* If there is a CATALOG entry output it. */
  if (cue->CATALOG[0] != '\0')
    xfprintf (stream, "CATALOG %.13s\n", &cue->CATALOG);

  /* If there is a CDTEXTFILE entry output it. */
  if (cue->CDTEXTFILE != NULL)
    xfprintf (stream, "CDTEXTFILE \"%s\"\n", cue->CDTEXTFILE);

  /* If there is a PERFORMER entry output it. */
  if (cue->PERFORMER != NULL)
    xfprintf (stream, "PERFORMER \"%.80s\"\n", cue->PERFORMER);

  /* If there is a SONGWRITER entry output it. */
  if (cue->SONGWRITER != NULL)
    xfprintf (stream, "SONGWRITER \"%.80s\"\n", cue->SONGWRITER);

  /* If there is a TITLE entry output it. */
  if (cue->TITLE != NULL)
    xfprintf (stream, "TITLE \"%.80s\"\n", cue->TITLE);

  /* Process FILE entries. */
  for (file = 0; file < cue->FileEntries; file++)
    {
      int track;			/* Track number (at least one
					   based); */

      /* If there is a FILE entry, output it. */
      if (cue->FILE[file].filename != NULL)
	xfprintf (stream, "FILE \"%s\" %s\n",
		  cue->FILE[file].filename, filetype[cue->FILE[file].filetype]);

      /* Process TRACK entries. */
      for (track = cue->FILE[file].FirstTrack;
	   track <= cue->FILE[file].TrackEntries;
	   track++)
	{
	  int index;			/* Index number (zero based); */

	  /* Print TRACK entry. */
	  xfprintf (stream, "  TRACK %i %s\n", track,
		    datatype[cue->FILE[file].TRACK[track].datatype]);

	  /* If there is a FLAGS entry, output it. */
	  if (cue->FILE[file].TRACK[track].FLAGS != NULL)
	    xfprintf (stream, "    FLAGS %s\n", cue->FILE[file].TRACK[track].FLAGS);

	  /* If there is a ISRC entry, output it. */
	  if (cue->FILE[file].TRACK[track].ISRC[0] != '\0')
	    xfprintf (stream, "    ISRC %s\n", cue->FILE[file].TRACK[track].ISRC);

	  /* If there is a PERFORMER entry output it. */
	  if (cue->FILE[file].TRACK[track].PERFORMER != NULL)
	    xfprintf (stream, "    PERFORMER \"%.80s\"\n",
		      cue->FILE[file].TRACK[track].PERFORMER);

	  /* If there is a SONGWRITER entry output it. */
	  if (cue->FILE[file].TRACK[track].SONGWRITER != NULL)
	    xfprintf (stream, "    SONGWRITER \"%.80s\"\n",
		      cue->FILE[file].TRACK[track].SONGWRITER);

	  /* If there is a TITLE entry output it. */
	  if (cue->FILE[file].TRACK[track].TITLE != NULL)
	    xfprintf (stream, "    TITLE \"%.80s\"\n",
		      cue->FILE[file].TRACK[track].TITLE);

	  /* If there is pre-gap, output it. */
	  if (cue->FILE[file].TRACK[track].PREGAP.initialized)
	    xfprintf (stream, "    PREGAP %02u:%02u:%02u\n",
		      cue->FILE[file].TRACK[track].PREGAP.minutes,
		      cue->FILE[file].TRACK[track].PREGAP.seconds,
		      cue->FILE[file].TRACK[track].PREGAP.frames);

	  /* Process INDEX entries */
	  for (index = 0; index < cue->FILE[file].TRACK[track].IndexEntries; index++)
	    if (cue->FILE[file].TRACK[track].INDEX[index].initialized)
	      xfprintf (stream, "    INDEX %02u %02u:%02u:%02u\n", index,
			cue->FILE[file].TRACK[track].INDEX[index].minutes,
			cue->FILE[file].TRACK[track].INDEX[index].seconds,
			cue->FILE[file].TRACK[track].INDEX[index].frames);

	  /* If there is post-gap, output it. */
	  if (cue->FILE[file].TRACK[track].POSTGAP.initialized)
	    xfprintf (stream, "    POSTGAP %02u:%02u:%02u\n",
		      cue->FILE[file].TRACK[track].POSTGAP.minutes,
		      cue->FILE[file].TRACK[track].POSTGAP.seconds,
		      cue->FILE[file].TRACK[track].POSTGAP.frames);
	}
    }

  /* Return with success. */
  return 0;
}
