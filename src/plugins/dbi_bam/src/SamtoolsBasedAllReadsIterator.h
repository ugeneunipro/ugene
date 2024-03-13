/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#pragma once

#include "SamtoolsBasedDbi.h"

namespace U2 {

namespace BAM {

/*
 * @SamtoolsBasedAllReadsIterator allows one to iterate through all assembly reads
 * without caching more than one read at the time.
 * It allows one to handle all reads and find some required information
 * (for example, to calculate maximum possible read end position)
 * and prevents memory overflow.
 **/
class SamtoolsBasedAllReadsIterator : public SamtoolsBasedReadsIterator {
public:
    SamtoolsBasedAllReadsIterator(int assemblyId, SamtoolsBasedDbi& dbi);

    /*
     * Returns next assembly read.
     * This function is forbidden here to prevent memory overflow.
     **/
    U2AssemblyRead next() override;

    /* Compares end positions of each read and returns the maximum one */
    qint64 calculateMaxEndPos();

private:
    void fetchReads() override;

};

}

}
