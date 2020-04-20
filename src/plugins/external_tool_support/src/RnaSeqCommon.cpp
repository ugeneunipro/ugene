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

#include "RnaSeqCommon.h"

#include <U2Core/U2SafePoints.h>


namespace U2 {

const QString RnaSeqLibraryType::frUnstranded = "fr-unstranded";
const QString RnaSeqLibraryType::frFirstStrand = "fr-firststrand";
const QString RnaSeqLibraryType::frSecondStrand = "fr-secondstrand";


RnaSeqLibraryType::RnaSeqLibraryType()
{
    libraryType = 0;
}


bool RnaSeqLibraryType::setLibraryType(int val)
{
    if (val < 0 || val > 2) {
        return false;
    }
    else {
        libraryType = val;
        return true;
    }
}


QString RnaSeqLibraryType::getLibraryTypeAsStr()
{
    switch(libraryType) {
        case 0:
            return frUnstranded;
        case 1:
            return frFirstStrand;
        case 2:
            return frSecondStrand;
        default:
            FAIL(QObject::tr("Unexpected value '%1' of the library type").arg(libraryType), frUnstranded);
    }
}

} // namespace
