/*
 convert.h -- CCD sheet structure to CUE sheet structure conversion;

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
 * \file       convert.h
 * \brief      _CCD sheet_ structure to _CUE sheet_ structure conversion
 */


#ifndef CCD2CUE_CONVERT_H
#define CCD2CUE_CONVERT_H

#include "ccd.h"
#include "cue.h"

/**
 * Convert _CCD structure_ to _CUE structure_.
 *
 * \param[in]   ccd       _CCD structure_;
 * \param[in]   img_name  Disc image file name; used in _FILE_ entry.
 * \param[in]   cdt_name  CDT file name; used in _CDTEXTFILE_ entry.
 *
 * \return A pointer to the resulting _CUE structure_ or _NULL_ in
 * case of a conversion error.
 *
 * \since 0.2
 *
 * This function is the core of the conversion process.  It transforms
 * a _CCD structure_, that is the structured memory representation of
 * a _CCD sheet_, into a _CUE sheet_, that is the structured memory
 * representation of a _CUE sheet_.  After this process have been done
 * only is necessary to call ::cue2stream with the resulting _CUE
 * structure_ to generate the final _CUE sheet_ output.
 *
 * There is no equivalence between _CCD sheets_ and _CUE sheets_,
 * because they do not have the same expressiveness power.  In fact,
 * regarding the CD features supported, the _CCD sheet_ format is a
 * super-set of CUE sheet format.  So, _CCD sheets_ are more closely
 * to a 1:1 correspondence with the source disc --- but still it is
 * not a exact copy, though.  Nevertheless, for most practical
 * purposes the _CUE sheet_ is expressive enough and it is handled by
 * various free software programs.
 *
 * This function extracts all information that can be expressed in a
 * CUE sheet from the _CCD structure_.  From the _CUE sheet_ point of
 * view it includes:
 *
 *- _CATALOG_ entry;
 *- _CDTEXTFILE_ entry;
 *- _FILE_ entry;
 *  + File type;
 *- _TRACK_ sections;
 *  + Data type;
 *  + _FLAGS_;
 *  + _ISRC_;
 *  + _INDEX_ entries;
 *
 * The _CDText_ information is not put inside the _CUE sheet_
 * structure using the entries _PERFORMER_, _SONGWRITER_ and _TITLE_
 * at global or track scope, rather the ::ccd2cdt function is used to
 * extract the _CDText_ data from the _CCD structure_ into a _CDT
 * structure_ that can later be converted to a stream by ::cdt2stream
 * function.  Thus, if the _CCD structure_ has _CDText data_, it is
 * indicated by the _CDTEXTFILE_ entry in the resulting _CUE sheet_
 * structure.
 *
 * _REM_ entries that are intended for comments are not used.
 * _PREGAP_ and _POSTGAP_ entries, that only appears inside _TRACK_
 * sections, are not used, also.
 *
 * From the _CCD sheet_ point of view it includes:
 *
 *- _Disc_ section;
 *  + _CATALOG_ entry;
 *- _TRACK_ sections;
 *  + _MODE_ entry;
 *  + _FLAGS_ entry;
 *  + _ISRC_ entry;
 *  + _INDEX_ entries;
 *
 * For the _CDText data_ extracted by ::ccd2cdt it also includes:
 *
 *- _Disc_ section;
 *  + _CDTextLength_ entry (not accessed);
 *- _CDText_ section;
 *  + _Entries_ entry (indirectly);
 *  + _Entry_ entries;
 *
 * The remainder, not used, sections and entries from a CCD sheet are:
 *
 *- _CloneCD_ section;
 *  + _Version_ entry;
 *- _Disc_ section;
 *  + _TocEntries_ entry;
 *  + _Sessions_ entry;
 *  + _DataTracksScrambled_ entry
 *- _Session_ section;
 *  + _PreGapMode_ entry;
 *  + _PreGapSubC_ entry;
 *- _Entry_ sections;
 *  + _Session_ entry;
 *  + _Point_ entry;
 *  + _ADR_ entry;
 *  + _Control_ entry;
 *  + _TrackNo_ entry;
 *  + _AMin_ entry;
 *  + _ASec_ entry;
 *  + _AFrame_ entry;
 *  + _ALBA_ entry;
 *  + _Zero_ entry;
 *  + _PMin_ entry;
 *  + _PSec_ entry;
 *  + _PFrame_ entry;
 *  + _PLBA_ entry;
 *
 * Only memory allocation problems can make this function return with
 * failure.
 *
 * \sa
 * - Previous step:
 *   + ::stream2ccd
 * - Parallel step:
 *   + ::ccd2cdt
 * - Next step:
 *   + ::cue2stream
 *   + ::cdt2stream
 *
 */

struct cue * ccd2cue (const struct ccd *ccd, const char *img_name, const char *cdt_name)
  __attribute__ ((nonnull));

/**
 * Extract _CDText data_ from _CCD structure_.
 *
 * \param[in]   ccd  _CCD structure_;
 * \param[out]  cdt  _CDT structure_;
 *
 * \return  Number of _CDText entries_ in the resulting _CDT
 *          structure_;
 *
 * \since 0.2
 *
 * The _CDText_ information is not put inside the _CUE sheet_
 * structure, by the ::ccd2cue function, using the entries
 * _PERFORMER_, _SONGWRITER_ and _TITLE_ at global or track scope,
 * rather this function is used to extract the _CDText_ data from the
 * _CCD structure_ into a _CDT structure_ that can later be converted
 * to a stream by ::cdt2stream function.  Thus, if the _CCD structure_
 * has _CDText data_, it is indicated by the _CDTEXTFILE_ entry in the
 * resulting _CUE sheet_ structure.
 *
 * Besides moving the raw _CDText data_ to a _CDT structure_, this
 * function also calculates the negated CRC-16, with a normal
 * polynomial CCITT, for each record, as required by the _CDT_ stream
 * format to which the _CDT structure_ ought to be converted by
 * ::cdt2stream.
 *
 * It is worth to emphasize that even when the input _CCD structure_
 * has no _CDText data_ a correct, and empty, _CDT structure_ is
 * generated and 0 is returned.
 *
 * \sa
 *- Internal:
 *  + ::crc16
 *- Previous step:
 *  + ::stream2ccd
 *- Parallel step:
 *  + ::ccd2cue
 *- Next step:
 *  + ::cue2stream
 *  + ::cdt2stream
 *
 */

int ccd2cdt (const struct ccd *ccd, struct cdt *cdt)
  __attribute__ ((nonnull));

#endif	/* CCD2CUE_CONVERT_H */
