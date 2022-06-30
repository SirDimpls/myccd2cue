/*
 cue.h -- CUE sheet format structure;

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
 * \file       cue.h
 * \brief      CUE sheet format structure
 */


#ifndef CCD2CUE_CUE_H
#define CCD2CUE_CUE_H

#include <stdio.h>

/**
 * _FILE_ entry's file type;
 *
 * This enumeration lists the file types supported as the second
 * parameter in a _FILE_ entry.  It is supposed to identify the data
 * nature of the disc image or audio files to be written to CD when
 * the _CUE sheet_ is used to burn it.
 *
 * There are a total of 5 types: two types for raw data and three for
 * audio.  The raw data types are _BINARY_ and _MOTOROLA_, and they
 * differ only in endianness --- little and big endian respectively.
 * The audio types are AIFF, WAVE and MP3 and the correspondent files
 * must be setup to 44.1 KHz, 16 bits and stereo.
 *
 * Actually all _CCD sheets_ uses a disc image with the standard
 * ".img" extension and type _MOTOROLA_.
 *
 */

enum cue_filetype
  {
    BINARY, 			/**< Intel binary file, i.e., least
				   significant byte first. */
    MOTOROLA,			/**< Motorola binary file, i.e., most
				   significant byte first. */
    AIFF,			/**< AIFF audio file. */
    WAVE,			/**< WAVE audio file. */
    MP3				/**< MP3 audio file.  */
  };

/**
 * _FILE entry_ structure;
 *
 * This structure holds the information of a _FILE_ and _TRACK_
 * entries.  It is used to specify the data or audio file to be
 * written to CD when the _CUE sheet_ is used to burn it.  It also
 * specifies the tracks into which that file is split on the resultant
 * disc.
 *
 * All track numbers must be between 1 and 99 inclusive.  The first
 * track number can be greater than one, but all track numbers after
 * the first must be sequential.  Each file must have at least one
 * track.
 *
 */

struct cue_FILE
{
  char *filename;		/**< The audio or data file's name. */
  enum cue_filetype filetype;	/**< The audio or data file's type. */
  struct cue_TRACK *TRACK;	/**< Array of tracks for this file entry. */
  int TrackEntries; 		/**< Number of track entries. */
  int FirstTrack;               /**< First track's number. */
};

/**
 * _MSF time_ structure;
 *
 * This structure is used to represent time measure in a _CUE sheet_.
 * It is based on the following temporal correspondence:
 *
 *- 1 second = 75 frames
 *- 1 minute = 60 seconds
 *
 * _Seconds_ and _minutes_ holds its usual meaning and _frames_ is the
 * time length remainder less than 1 second, each unit being the 75th
 * part of a second.
 *
 * Often this structure is filled by the function ::frames2msf that
 * converts a raw frames based time to the MSF format.  It is used on
 * the ::cue_TRACK.PREGAP, ::cue_TRACK.POSTGAP and ::cue_TRACK.INDEX
 * fields of the ::cue_TRACK structure.
 *
 * \sa ::frames2msf
 *
 */

struct cue_time
{
  int initialized;		/**< A boolean indicating whether this
				   structure is initialized. */
  unsigned int minutes;         /**< 1 minute = 60 seconds. */
  unsigned int seconds;		/**< 1 second = 75 frames. */
  unsigned int frames;		/**< The standard unit of time for
				   Compact Discs. */
};

/**
 * _TRACK_ entry's data type;
 *
 * This enumeration lists the data types supported as the second
 * parameter in a _TRACK entry_.  It is supposed to identify the
 * track's data type that will be used to write the respective chunk
 * of data to the CD in the burning process.
 *
 * There are 8 different data types but not all recorders can handle
 * all of them.  It seems _CCD sheets_ only can use 3 of them: AUDIO,
 * MODE1/2352 and MODE2/2352.
 *
 */

enum cue_datatype
  {
    AUDIO_2352, 		/**< Audio/Music (2352) */
    CDG_2448,			/**< Karaoke CD+G (2448) */
    MODE1_2048,			/**< CD-ROM Mode1 Data (cooked) */
    MODE1_2352,			/**< CD-ROM Mode1 Data (raw) */
    MODE2_2336,			/**< CD-ROM XA Mode2 Data (2336) */
    MODE2_2352,			/**< CD-ROM XA Mode2 Data (2352) */
    CDI_2336,			/**< CD-I Mode2 Data (2336) */
    CDI_2352			/**< CD-I Mode2 Data (2352) */
  };

/**
 * _TRACK entry_ structure;
 *
 * This structure holds the information of an individual track.
 *
 * The fields that are all capital letters represents the homonym _CUE
 * sheet_ entries.
 *
 * The _PERFORMER_, _SONGWRITER_ and _TITLE_ entries allow strings in
 * the range of 1 to 80 characters.
 *
 * No data from the current data file is consumed by _PREGAP_ or
 * _POSTGAP_ entries.
 *
 * The index range is from 0 to 99.  The time in a index is relative
 * to the beginning of the current file.  The first index number must
 * be 0 or 1 with all other indexes being sequential to the first one.
 * The first index of a track must start at 00:00:00.  INDEX 0
 * specifies the starting time of the track pre-gap.  INDEX 1
 * specifies the starting time of the track data.  Index 1 is the only
 * index that is stored in the disc's table-of-contents.  INDEX
 * numbers greater than 1 specify a subindex within a track.
 *
 */

struct cue_TRACK
{
  enum cue_datatype datatype;	/**< Track's data type. */
  char *FLAGS;                  /**< Special track's subcode flags;
				 - DCP = Digital copy permitted
				 - 4CH = Four channel audio
				 - PRE = Pre-emphasis enabled
				       (audio tracks only)
				 - SCMS = Serial Copy Management System
				 - DATA = data track
				          (not used directly).
  				 One or more flags can be specified
				 separating them by a space.  */
  char ISRC[12 + 1];		/**< International Standard Recording
				   Code; must be 12 characters in
				   length --- the first five
				   characters are alphanumeric, the
				   last seven are numeric only. */
  char *PERFORMER;		/**< Name of the track's performer. */
  char *SONGWRITER;		/**< Name of the track's songwriter. */
  char *TITLE;			/**< Title of the track. */
  struct cue_time PREGAP;	/**< Length of a track pre-gap. */
  struct cue_time *INDEX;	/**< Array of _INDEX entries_. */
  int IndexEntries;		/**< Number of _INDEX entries_. */
  struct cue_time POSTGAP;	/**< Length of a track post-gap. */
};

/**
 * Structure representation of a _CUE sheet_.
 *
 * The function ::ccd2cue transforms a ::ccd structure into a structure
 * of this type.  That is the core procedure of the conversion
 * process.  Latter this structure is converted to a _CUE sheet_
 * stream by ::cue2stream.
 *
 * The fields that are all capital letters represents the homonym _CUE
 * sheet_ entries.
 *
 * \sa ::ccd, ::ccd2cue and ::cue2stream
 *
 **/

struct cue
{
  char CATALOG[13 + 1];		/**< Media Catalog Number in UPC/EAN
				     encoding; must be 13
				     characters. */
  char *CDTEXTFILE;		/**< The CD-Text file's name. */
  char *PERFORMER;		/**< Name of the disc's performer. */
  char *SONGWRITER;		/**< Name of the disc's songwriter. */
  char *TITLE;			/**< Title of the disc. */
  struct cue_FILE *FILE;	/**< Array of _FILE entries_. */
  int FileEntries;		/**< Number of _FILE entries_. */
};

/*
 * The following functions are higher level interfaces to allocation
 * and initialization of arrays of structures defined in this file.
 * They provide an uniform way of allocation and initialization that
 * latter transformation code can rely upon.  Thus, usually, it is an
 * error to allocate and initialize arrays of those structures
 * manually, as that can be a big source of hard to trace bugs.
 *
 * The function naming follow a consistent scheme: <struc_name>_init,
 * where <struc_name> is the correspondent structure name.  The
 * function prototypes are also consistent; all functions are given
 * the same array size argument and returns a pointer to the newly
 * allocated array.  Every element in the array is initialized
 * accordingly.
 *
 * On the following functions, after the allocation the initialization
 * procedure takes place. It works by assigning the canonical initial
 * values to each field.  It means that integers gets a appropriate
 * default value or zero if there is not one suitable, pointers gets a
 * _NULL pointer_ and character arrays a null character on the first
 * array position.
 *
 * The functions are often used in the ::ccd2cue function in order to
 * mark the fields of the respective structures that were not
 * originally supplied by the ::ccd structure and then consistently
 * apply posterior transformation with ::cue2stream.
 *
 * The functions only fail if there is some allocation error.  A
 * failure is always fatal, i.e., the program exits with error.
 * Therefore, all functions only return with success.
 *
 */

/**
 * Allocate and initialize an array of ::cue structures.
 *
 * \param[in]  entries  Array's size;
 *
 * \return Pointer to the newly allocated array;
 *
 * \since 0.2
 *
 * \sa ::ccd, ::ccd2cue and ::cue2stream
 *
 */

struct cue * cue_init (size_t entries);

/**
 * Allocate and initialize an array of ::cue_FILE structures.
 *
 * \param[in]  entries  Array's size;
 *
 * \return Pointer to the newly allocated array;
 *
 * \since 0.2
 *
 * \sa ::ccd, ::ccd2cue and ::cue2stream
 *
 */

struct cue_FILE * cue_FILE_init (size_t entries);

/**
 * Allocate and initialize an array of ::cue_TRACK structures.
 *
 * \param[in]  entries  Array's size;
 *
 * \return Pointer to the newly allocated array;
 *
 * \since 0.2
 *
 * \sa ::ccd, ::ccd2cue and ::cue2stream
 *
 */

struct cue_TRACK * cue_TRACK_init (size_t entries);

/**
 * Parse _CUE sheet_ structure into a _CUE sheet_ stream.
 *
 * \param[in]   cue     Pointer to the ::cue structure;
 * \param[out]  stream  Output stream;
 *
 * \return
 * + =0  success
 * + <0  failure
 *
 * \since 0.2
 *
 * This function is intended to generate the stream representation of
 * a _CUE sheet_ from its structured memory representation.  This is
 * the last step in the conversion process.
 *
 * This function is complete in the sense that it recognizes and
 * outputs all possible _CUE sheet_ declarations, even those that do
 * not are used in the conversion process from a _CCD sheet_.  So, in
 * principle, this conversion step can create a _CUE sheet_ with any
 * standard supported feature.
 *
 * This function only fail on an obscure case where it is impossible
 * to write some line for the output stream.
 *
 * \sa
 * - Previous step:
 *   + ::ccd2cue
 *   + ::ccd2cdt
 *
 */

int cue2stream (const struct cue *cue, FILE *stream)
  __attribute__ ((nonnull));

#endif	/* CCD2CUE_CUE_H */
