/*
 ccd.c -- CCD sheet format structure;

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
 * \file       ccd.c
 * \brief      CCD sheet format structure
 */


#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "memory.h"
#include "i18n.h"
#include "ccd.h"
#include "errors.h"
#include "array.h"


/**
 * Initialize ::ccd structure.
 *
 * \param[out]  ccd  CCD structure;
 *
 * \since 0.2
 *
 * This function initialize a ccd structure by assigning the canonical
 * initial values to each field.  It means that integers gets a
 * appropriate default value or zero if there is not one suitable,
 * pointers gets a NULL pointer and character arrays a null character
 * on the first array position.
 *
 * This function is used in the function ::stream2ccd in order to mark
 * the fields on ccd structure that were not originally supplied by
 * the CCD sheet stream and then consistently apply posterior
 * transformation with ::ccd2cue.
 *
 */

static void ccd_init (struct ccd *ccd)
  __attribute__ ((nonnull));

/**
 * Initialize ::ccd_TRACK structure.
 *
 * \param[out]  TRACK  ccd track structure.
 *
 * \since 0.2
 *
 * This function initialize a ccd track structure by assigning the
 * canonical initial values to each field.  It means that integers
 * gets a appropriate default value or zero if there is not one
 * suitable, pointers gets a _NULL pointer_ and character arrays a null
 * character on the first array position.
 *
 * This function is used in the function ::stream2ccd in order to mark
 * the fields on ccd track structure that were not originally supplied
 * by the CCD sheet stream and then consistently apply posterior
 * transformation with ::ccd2cue.
 *
 * The first two indexes (0 and 1) are allocated and it is assigned a
 * value of -1 to each one to distinguish from a supplied value that
 * will have a positive value.
 *
 *
 */

static void ccd_TRACK_init (struct ccd_TRACK *TRACK)
  __attribute__ ((nonnull));


int
stream2ccd (FILE *stream, struct ccd *ccd)
{
  /* Entries counters; these are used for numbering the entries
     successively in the resulting cue structure regardless of order
     or gaps that could have in the input CCD stream. */
  int Session = 0;		/* Session; starts from 1;*/
  int TocEntry = -1;		/* Toc; starts from 0; */
  int CDTextEntry = -1;		/* CDText; starts from 0; */
  int TRACK = 0;		/* Track; starts from 1; */

  /* Assert the stream is valid. */
  assert (stream != NULL);

  /* Assert the CCD structure is valid. */
  assert (ccd != NULL);

  /* Initialize the CCD structure. */
  ccd_init (ccd);

  /* Parse the whole stream. */
  while (!feof (stream))
    {
      /* Last line read from stream; */
      char *line = NULL;
      /* Size of LINE buffer; */
      size_t line_size = 0;

      /* CCD entries counters; these are the entries counters
	 explicitly specified by actual CCD sheet entries from the
	 input stream. */
      int Sessions;		/* "Sessions" inside "Disc"
				   section; */
      int TocEntries;		/* "TocEntries" inside "Disc"
				   section; */
      int CDTextEntries;	/* "Entries" inside "CDText"
				   section;*/

      /* Temporary entry values; these are used as place holders to
	 detect 'scanf' assignments and, except INDEX_tmp, are not
	 used in any other fashion.  It works that way because this
	 function disregards the CCD sheet original numbering to avoid
	 invalid input from resulting unpredictable behavior.
	 Nevertheless, a valid CCD sheet will be always parsed the
	 right way, because it is consistent between number of entries
	 declaration fields and the actual number of entries and has
	 consecutive numbering, starting at 0 or 1 --- depending on
	 the case, as well. */
      int Session_tmp, TocEntry_tmp, CDTextEntry_tmp, TRACK_tmp, INDEX_tmp;

      /* Read the current stream's line.  If it is not possible push
	 an error. */
      errno = 0;
      if (my_getline (&line, &line_size, stream) == -1 && errno != 0)
            error_push_lib (getline, -1, "cannot parse CCD sheet stream");

      /* Simple data --- these are data that do not need dynamic
	 allocation; insert the value of each entry on the respective
	 fields on ccd structure.  It is not enforced that entries be
	 inside their respective, usual and correct sections, as they
	 are uniquely named in the CCD sheet, and thus uniquely
	 identified without verifying section belonging correctness.
	 With a correct CCD sheet all should work fine. */

      /* "Version" entry inside "CloneCD" section; */
      sscanf (line, " Version = %d ", &ccd->CloneCD.Version);
      /* "DataTracksScrambled" entry inside "Disc" section; */
      sscanf (line, " DataTracksScrambled = %d ", &ccd->Disc.DataTracksScrambled);
      /* "CDTextLength" entry inside "Disc" section ; */
      sscanf (line, " CDTextLength = %d ", &ccd->Disc.CDTextLength);
      /* "CATALOG" entry inside "Disc" section;*/
      sscanf (line, " CATALOG = %13[a-zA-Z0-9] ", ccd->Disc.CATALOG);

      /* Predetermined size composite data --- these are data that
	 needs dynamic allocation but we know the whole size of the
	 structure beforehand.  There are 3 entities of this type:
	 "Session" and "Toc" sections, this last being simple named as
	 "Entry" on the CUE sheet stream, and "CDText" data entries.

	 The number of "Session" sections is given by the "Sessions"
	 entry inside the "Disc" section.  The number of "Toc"
	 sections is given by the "TocEntries" entry inside the "Disc"
	 section.  The number of "CDText" data entries is given in the
	 "Entries" entry inside the "CDText" section.

	 If any of "Sessions", "TocEntries" or "Entries" entries are
	 not found it is assumed none section or entry of that type.
	 If there is more than one of them, just the first is
	 considered and it is taken as the authoritative reference.
	 For each of them that are found an appropriate sized
	 structure is allocated with the read size.  If there is less
	 sections or entries than informed by them, the allocated
	 structure is reduced to the exact size that accommodate the
	 found sections and the number of records is updated as if it
	 were correctly supplied.  If there is more sections or
	 entries than informed by them, the additional sections and
	 entries are just ignored.

	 With a correct CCD sheet all should work just fine.
      */

      /* "Sessions" entry; check whether the current line is a
	 "Sessions" entry, it is the first found and it has a positive
	 index.*/
      if (sscanf (line, " Sessions = %d ", &Sessions) == 1
	  && Sessions > 0
	  && ccd->Disc.Sessions == 0)
	{
	  /* Consider the indicated number of "Session" sections as an
	     authoritative reference. */
	  ccd->Disc.Sessions = Sessions;
	  /* Allocate the necessary space to accommodate all "Session"
	     sections. */
	  ccd->Session = xmalloc (sizeof (*ccd->Session) * (ccd->Disc.Sessions + 1));
	}

      /* Check if you have already found a "Sessions" entry indicating
	 how many "Session" sections supposedly there are. */
      if (ccd->Disc.Sessions > 0)
	{
	  /* If the current line is declaring a new "Session" section,
	     and you did not found all sections announced, count up
	     this section. */
	  if (sscanf (line, " [ Session %d ] ", &Session_tmp) == 1
	      && Session <= ccd->Disc.Sessions)
	    Session++;

	  /* If you found a "Session" section header declaration,
	     check if the current line is one of its contents.  And
	     add its value to the structure in affirmative case. */
	  if (Session >= 1)
	    {
	      sscanf (line, " PreGapMode = %d ", &ccd->Session[Session].PreGapMode);
	      sscanf (line, " PreGapSubC = %d ", &ccd->Session[Session].PreGapSubC);
	    }
	}

      /* "TocEntries" (Entry) entry; check whether the current line is
	 a "TocEntries" entry, it is the first found and it has a
	 positive index. */
      if (sscanf (line, " TocEntries = %d ", &TocEntries) == 1
	  && TocEntries > 0
	  && ccd->Disc.TocEntries == 0)
	{
	  /* Consider the indicated number of "Toc" sections as an
	     authoritative reference. */
	  ccd->Disc.TocEntries = TocEntries;
	  /* Allocate the necessary space to accommodate all "Toc"
	     sections. */
	  ccd->Entry = xmalloc (sizeof (*ccd->Entry) * (ccd->Disc.TocEntries + 1));
	}

      /* Check if you have already found a "TocEntries" entry indicating
	 how many "Toc" sections supposedly there are. */
      if (ccd->Disc.TocEntries > 0)
	{
	  /* If the current line is declaring a new "Toc" section, and
	     you did not found all sections announced, count up this
	     section. */
	  if (sscanf (line, " [ Entry %d ] ", &TocEntry_tmp) == 1
	      && (TocEntry + 1) <= ccd->Disc.TocEntries)
	    TocEntry++;

	  /* If you found a "Toc" section header declaration, check if
	     the current line is one of its contents.  And add its
	     value to the structure in affirmative case. */
	  if (TocEntry >= 0)
	    {
	      sscanf (line, " Session = %d ", &ccd->Entry[TocEntry].Session);
	      sscanf (line, " Point = %x ", &ccd->Entry[TocEntry].Point);
	      sscanf (line, " ADR = %x ", &ccd->Entry[TocEntry].ADR);
	      sscanf (line, " Control = %x ", &ccd->Entry[TocEntry].Control);
	      sscanf (line, " TrackNo = %d ", &ccd->Entry[TocEntry].TrackNo);
	      sscanf (line, " AMin = %d ", &ccd->Entry[TocEntry].AMin);
	      sscanf (line, " ASec = %d ", &ccd->Entry[TocEntry].ASec);
	      sscanf (line, " AFrame = %d ", &ccd->Entry[TocEntry].AFrame);
	      sscanf (line, " ALBA = %d ", &ccd->Entry[TocEntry].ALBA);
	      sscanf (line, " Zero = %d ", &ccd->Entry[TocEntry].Zero);
	      sscanf (line, " PMin = %d ", &ccd->Entry[TocEntry].PMin);
	      sscanf (line, " PSec = %d ", &ccd->Entry[TocEntry].PSec);
	      sscanf (line, " PFrame = %d ", &ccd->Entry[TocEntry].PFrame);
	      sscanf (line, " PLBA = %d ", &ccd->Entry[TocEntry].PLBA);
	    }
	}

      /* "Entries" (CDText) entries; check whether the current line is a "Entries"
	 entry, it is the first found and it has a positive index. */
      if (sscanf (line, " Entries = %d ", &CDTextEntries) == 1
	  && CDTextEntries > 0
	  && ccd->CDText.Entries == 0)
      	{
	  /* Consider the indicated number of "Entry" (CDText) entries
	     as an authoritative reference. */
	  ccd->CDText.Entries = CDTextEntries;
	  /* Allocate the necessary space to accommodate all "Entry"
	     (CDText) entries. */
      	  ccd->CDText.Entry = xmalloc (sizeof (*ccd->CDText.Entry)
				       * ccd->CDText.Entries);
      	}

      /* Check if you have already found a "Entries" (CDText) entry
	 indicating how many "Entry" (CDText) entries supposedly there
	 are. */
      if (ccd->CDText.Entries > 0)
	{
	  /* If the current line is declaring a new "Entry" (CDText)
	     entry, and you did not found all sections announced,
	     count up this section. */
	  if ((sscanf (line, " Entry %d = ", &CDTextEntry_tmp) == 1)
	      && (CDTextEntry + 1) <= ccd->CDText.Entries)
	    CDTextEntry++;

	  /* If you found a "Entry" (CDText) entry declaration, check
	     if the current line is one of its contents.  And add its
	     value to the structure in affirmative case.*/
	  if (CDTextEntry >= 0)
	    {
	      sscanf (line, " Entry %*d = %hhx %hhx %hhx %hhx %hhx %hhx %hhx \
%hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx ",
		      &ccd->CDText.Entry[CDTextEntry].type,
		      &ccd->CDText.Entry[CDTextEntry].track,
		      &ccd->CDText.Entry[CDTextEntry].sequence,
		      &ccd->CDText.Entry[CDTextEntry].block,
		      &ccd->CDText.Entry[CDTextEntry].text[0],
		      &ccd->CDText.Entry[CDTextEntry].text[1],
		      &ccd->CDText.Entry[CDTextEntry].text[2],
		      &ccd->CDText.Entry[CDTextEntry].text[3],
		      &ccd->CDText.Entry[CDTextEntry].text[4],
		      &ccd->CDText.Entry[CDTextEntry].text[5],
		      &ccd->CDText.Entry[CDTextEntry].text[6],
		      &ccd->CDText.Entry[CDTextEntry].text[7],
		      &ccd->CDText.Entry[CDTextEntry].text[8],
		      &ccd->CDText.Entry[CDTextEntry].text[9],
		      &ccd->CDText.Entry[CDTextEntry].text[10],
		      &ccd->CDText.Entry[CDTextEntry].text[11]);
	    }
	}

      /* Unknown size composite data --- these are data that needs
	 dynamic allocation and we do not know the whole size of the
	 structure beforehand.  These are "Track" sections and they
	 are allocated as they are found.  In the end of the process
	 the field ccd->TrackEntries holds the number of "Tracks"
	 sections that have been found.  So, the ccd structure
	 processing could be done more efficiently.  */

      /* "TRACK" section; If the current line is a "TRACK" section
	 declaration header, allocate room for more one "Track"
	 section in the ccd structure. */
      if (sscanf (line, " [ TRACK %d ] ", &TRACK_tmp) == 1)
	{
	  /* Count up this section on the track counter.  */
	  ccd->TrackEntries = ++TRACK;
	  /* Reallocate the track array to accommodate more one track
	     in the structure.  */
	  ccd->TRACK = xrealloc (ccd->TRACK, sizeof (*ccd->TRACK) * (TRACK + 1));
	  /* Initialize the newly allocated track structure. */
	  ccd_TRACK_init (&ccd->TRACK[TRACK]);
	}

      /* If you have already found a "Track" section header verify if
	 the current line is one of its contents. */
      if (ccd->TrackEntries > 0)
	{
	  /* If the current line holds the mode for the track, place
	     that value into the track structure. */
	  sscanf (line, " MODE = %d ", &ccd->TRACK[TRACK].MODE);

	  /* If the current line holds the flags for the track, place
	     that value into the track structure. */
	  if (sscanf (line, " FLAGS = %a[a-zA-Z0-9 ] ", &ccd->TRACK[TRACK].FLAGS) == 1)
	    /* Remove trailing white space if any */
	    ccd->TRACK[TRACK].FLAGS =
	      array_remove_trailing_whitespace (ccd->TRACK[TRACK].FLAGS);

	  /* If the current line holds the ISRC for the track, place
	     that value into the track structure. */
	  sscanf (line, " ISRC = %12[a-zA-Z0-9] ", ccd->TRACK[TRACK].ISRC);

	  /* If the current line holds an "INDEX" entry add it into
	     the track structure. */
	  if (sscanf (line, " INDEX %d = ", &INDEX_tmp) == 1)
	    {
	      /* If the "INDEX" entry's index is 0 or 1, just place
		 its value into the INDEX field already allocated to
		 the track structure. */
	      if (INDEX_tmp == 0 || INDEX_tmp == 1)
		sscanf (line, " INDEX %*d = %d ", &ccd->TRACK[TRACK].INDEX[INDEX_tmp]);
	      /* If different from 0 or 1, allocate a new entry and
		 update the index record counter. */
	      else
		{
		  /* Update the index counter. */
		  ccd->TRACK[TRACK].IndexEntries++;
		  /* Allocate a new entry for this index on the track
		     structure. */
		  ccd->TRACK[TRACK].INDEX = xrealloc (ccd->TRACK[TRACK].INDEX,
						      sizeof (*ccd->TRACK[TRACK].INDEX)
						      * ccd->TRACK[TRACK].IndexEntries);
		  /* Save the entry's value on the newly allocated
		     field. */
		  sscanf (line, " INDEX %*d = %d ",
			  &ccd->TRACK[TRACK].INDEX[ccd->TRACK[TRACK].IndexEntries - 1]);
		}
	    }
	}
    }

  /* If you have found less "Session" sections than informed on the
     "Sessions" entry, reduce the allocated structure to the exact
     size that accommodate the found sections and update the number of
     records. */
  if (Session < ccd->Disc.Sessions)
    {
      ccd->Session = xrealloc (ccd->Session,
			       sizeof (*ccd->Session) * Session);
      ccd->Disc.Sessions = Session;
    }

  /* If you have found less "Toc" sections than informed on the
     "TocEntries" entry, reduce the allocated structure to the exact
     size that accommodate the found sections and update the number of
     records. */
  if (TocEntry + 1 < ccd->Disc.TocEntries)
    {
      ccd->Entry = xrealloc (ccd->Entry,
			     sizeof (*ccd->Entry) * (TocEntry + 1));
      ccd->Disc.TocEntries = TocEntry + 1;
    }

  /* If you have found less "Entry" (CDText) entries than informed on
     the "Entries" (CDText) entry, reduce the allocated structure to
     the exact size that accommodate the found sections and update the
     number of records. */
  if (CDTextEntry + 1 < ccd->CDText.Entries)
    {
      ccd->CDText.Entry = xrealloc (ccd->CDText.Entry,
				    sizeof (*ccd->CDText.Entry) * (CDTextEntry + 1));
      ccd->CDText.Entries = CDTextEntry + 1;
    }

  /* Return success. */
  return 0;
}

static void
ccd_init (struct ccd *ccd)
{
  assert (ccd != NULL);

  /* Assume CloneCD version 3 by default.  It is the most common
     version.*/
  ccd->CloneCD.Version = 3;

  /* Fill the "Disc" section structure with initial values. */
  ccd->Disc.TocEntries = 0;
  ccd->Disc.Sessions = 0;
  ccd->Disc.DataTracksScrambled = 0;
  ccd->Disc.CDTextLength = 0;
  ccd->Disc.CATALOG[0] = '\0';

  /* Fill the "CDText" section structure with initial values. */
  ccd->CDText.Entries = 0;
  ccd->CDText.Entry = NULL;

  /* Initialize the remainder fields. */
  ccd->Session = NULL;
  ccd->Entry = NULL;
  ccd->TRACK = NULL;
  ccd->TrackEntries = 0;
}

static void
ccd_TRACK_init (struct ccd_TRACK *TRACK)
{
  assert (TRACK != NULL);

  /* Initialize some fields; */
  TRACK->MODE = 0;
  TRACK->FLAGS = NULL;
  TRACK->ISRC[0] = '\0';

  /* Allocate the two base indexes for this track structure. */
  TRACK->INDEX = xmalloc (sizeof (*TRACK->INDEX) * 2);
  TRACK->INDEX[0] = -1;
  TRACK->INDEX[1] = -1;
  TRACK->IndexEntries = 2;
}
