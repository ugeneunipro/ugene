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

#ifndef _U2_RNA_SEQ_COMMON_H_
#define _U2_RNA_SEQ_COMMON_H_

#include <QString>


namespace U2 {


enum BowtieMode {vMode = 0, nMode = 1};

class RnaSeqLibraryType
{
public:
    // Init library type as "fr-unstranded"
    RnaSeqLibraryType();

    // Return "false" if the value is not correct,
    // otherwise sets the new value and returns "true"
    bool setLibraryType(int val);

    // fr-unstranded - for example, Standard Illumina
    // fr-firststrand - examples are dUTP, NSR, NNSR
    // fr-secondstrand - examples are Directional Illumina (Ligation), Standard SOLiD
    QString getLibraryTypeAsStr();

private:
    int libraryType;

    static const QString frUnstranded;
    static const QString frFirstStrand;
    static const QString frSecondStrand;
};


} // namespace

#endif
