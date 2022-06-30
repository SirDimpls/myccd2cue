/*
 ccd.h -- CCD sheet format structure;

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
 * \file       ccd.h
 * \brief      CCD sheet format structure
 */


#ifndef CCD2CUE_CCD_H
#define CCD2CUE_CCD_H

#include "cdt.h"

/* Each structure named according to ccd_SECTION regards the SECTION
   of a CCD sheet and are filled with its info. */

/**
 * CloneCD section;
 *
 * This section is not used by CUE sheet.
 */

struct ccd_CloneCD
{
  int Version;			/**< Usually 3.  Not used by _CUE
				  sheet_. */
};

/**
 * Disc section;
 */

struct ccd_Disc
{
  int TocEntries;		/**< Number of "Toc" sections.  That
                                     sections are named "Entry".  Not
                                     used by CUE sheet. */
  int Sessions;			/**< Number of "Session" sections.  Not
				     used by CUE sheet.*/
  int DataTracksScrambled;	/**< Not used by CUE sheet. */
  int CDTextLength;		/**< The size of CDT file in bytes
				     minus 1 for the terminating null
				     character.  Not accessed
				     directly, deduced by other
				     means. */
  char CATALOG [13 + 1]; 	/**< Media Catalog Number in UPC/EAN
				     encoding; must be 13
				     characters. */
};

/**
 * CDText section;
 */

struct ccd_CDText
{
  int Entries;			/**< Number of "Entry" CDText
				     entries; */
  struct cdt_data *Entry;	/**< Array of "Entry" CDText
				     entries; */
};

/**
 * Session section;
 *
 * This section is not used by CUE sheet.
 */

struct ccd_Session
{
  int PreGapMode;		/**< Not used by CUE sheet. */
  int PreGapSubC;		/**< Not used by CUE sheet. */
};

/**
 * Entry section;
 *
 * This section are also called "Toc" section.  This section is not
 * used by CUE sheet.
 *
 */

struct ccd_Entry
{
  int Session;			/**< Not used by CUE sheet. */
  unsigned int Point;		/**< Not used by CUE sheet. */
  unsigned int ADR;		/**< Not used by CUE sheet. */
  unsigned int Control;		/**< Not used by CUE sheet. */
  int TrackNo;			/**< Not used by CUE sheet. */
  int AMin;			/**< Not used by CUE sheet. */
  int ASec;			/**< Not used by CUE sheet. */
  int AFrame;			/**< Not used by CUE sheet. */
  int ALBA;			/**< Not used by CUE sheet. */
  int Zero;			/**< Not used by CUE sheet. */
  int PMin;			/**< Not used by CUE sheet. */
  int PSec;			/**< Not used by CUE sheet. */
  int PFrame;			/**< Not used by CUE sheet. */
  int PLBA;			/**< Not used by CUE sheet. */
};

/**
 * Track section;
 */

struct ccd_TRACK
{
  int MODE;			/**< Track mode:
				 - 0 = AUDIO
				 - 1 = MODE1/2352
				 - 2 = MODE2/2352 */
  char ISRC[12 + 1];		/**< International Standard Recording
				  Code; must be 12 characters in
				  length --- the first five characters
				  are alphanumeric, the last seven are
				  numeric only. */
  int *INDEX;			/**< Array of _INDEX_ entries; */
  int IndexEntries;		/**< Number of _INDEX_ entries; it is
				  not in the CCD sheet properly. */
  char *FLAGS;			/**< Special track's subcode flags;
				 - DCP = Digital copy permitted
				 - 4CH = Four channel audio
				 - PRE = Pre-emphasis enabled
				       (audio tracks only)
				 - SCMS = Serial Copy Management System
				 - DATA = data track
				          (not used directly)
				 One or more flags can be specified
				 separating them by a space.*/
};

/**
 * Structure representation of a _CCD sheet_.
 *
 * The function ::stream2ccd parses a _CCD sheet_ stream and fills out
 * this structure that is latter used in ::ccd2cue function to
 * transform it in a ::cue structure that is latter converted to a _CUE
 * sheet_ stream by ::cue2stream.
 *
 */

struct ccd
{
  struct ccd_CloneCD CloneCD; 	/**< CloneCD section; */
  struct ccd_Disc Disc;		/**< Disc section; */
  struct ccd_CDText CDText;	/**< CDText section; */
  struct ccd_Session *Session;	/**< Array of Session sections; */
  struct ccd_Entry *Entry;	/**< Array of Toc sections; */
  struct ccd_TRACK *TRACK;	/**< Array of Track sections; */
  int TrackEntries; 		/**< Number of Track sections; it is
				  not in the CCD sheet properly. */
};

/**
 * Parse _CCD sheet_ stream into a _CCD sheet_ structure.
 *
 * \param[in]   stream  Input stream;
 * \param[out]  ccd     Pointer to a uninitialized ccd structure to
 *                      fill out;
 *
 * \return
 * + =0  success
 * + <0  failure
 *
 * \since 0.2
 *
 * This function is intended to parse a correct CCD sheet and to __not__
 * generate unpredictable or inconsistent behavior when the stream is
 * an invalid CCD sheet.  So, even if the CCD sheet is not valid it
 * will be parsed and a correct structure, not necessarily useful,
 * will be returned.  It is __not__ made an attempt to maximize the
 * information extraction from stream in the rare case of a damaged
 * CCD sheet.  So, it can be the case that this function process
 * correctly some damaged CCD sheet and return a result as if a
 * non-damaged version of the same CCD sheet were the original input,
 * but it is not guaranteed and not all possible provisions were taken
 * to handle cases like this.
 *
 * This function is complete in the sense that it recognizes and
 * parses all possible CCD sheet declarations, even those that do not
 * are used in the conversion process to a CUE sheet.  So, in
 * principle, this conversion step is reversible, i.e, it is possible
 * to reconstruct the exact input CCD sheet from the generated ccd
 * structure.
 *
 * This function only fail on an obscure case where it is impossible
 * to read some line of the input stream.
 *
 * \sa
 * - Next step:
 *   + ::ccd2cue
 *   + ::ccd2cdt
 *
 */
int stream2ccd (FILE *stream, struct ccd *ccd)
  __attribute__ ((nonnull));

#endif	/* CCD2CUE_CCD_H */
