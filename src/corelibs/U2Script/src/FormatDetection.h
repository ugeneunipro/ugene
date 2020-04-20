/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef _FORMAT_DETECTION_H_
#define _FORMAT_DETECTION_H_

#include "globals.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * List of supported by U2Script library formats for storing biological data.
 * Currently sequences and multiple sequence alignments are supported only.
 *
 */

typedef enum U2Format {
    U2_ABI,
    U2_ACE,
    U2_CLUSTAL,
    U2_EMBL,
    U2_FASTA,
    U2_FASTQ,
    U2_GENBANK,
    U2_MEGA,
    U2_MSF,
    U2_NEXUS,
    U2_PLAIN_TEXT,
    U2_STOCKHOLM,
    U2_SWISS,
    U2_UNSUPPORTED
} U2Format;

/*
 * This function determines the format of the given file
 *
 * Possible returning error codes:
 * U2_OK -                 success
 * U2_INVALID_PATH -       the `pathToFile` does not exist or contains path to a folder
 *
 */
U2SCRIPT_EXPORT U2ErrorType         detectFileFormat(   const wchar_t *     pathToFile,
                                                        U2Format *          format );

#ifdef __cplusplus
}
#endif

#endif // _FORMAT_DETECTION_H_
