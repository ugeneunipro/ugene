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

#ifndef _U2_BAM_INDEX_H_
#define _U2_BAM_INDEX_H_

#include <QList>
#include "VirtualOffset.h"

namespace U2 {
namespace BAM {

class Index
{
public:
    Index() {}
    class ReferenceIndex {
    public:
        class Chunk {
        public:
            Chunk(VirtualOffset start, VirtualOffset end);
            VirtualOffset getStart()const;
            VirtualOffset getEnd()const;
        private:
            VirtualOffset start;
            VirtualOffset end;
        };

        class Bin {
        public:
            Bin(unsigned int bin, QList<Chunk> chunks);
            const QList<Chunk> &getChunks() const;
            unsigned int getBin() const;
        private:
            unsigned int bin;
            QList<Chunk> chunks;
        };
        ReferenceIndex(const QList<Bin> &bins, const QList<VirtualOffset> &intervals);
        const QList<Bin> &getBins()const;
        const QList<VirtualOffset> &getIntervals()const;
    private:
        QList<Bin> bins;
        QList<VirtualOffset> intervals;
    };
    Index(const QList<ReferenceIndex> &referenceIndices);
    const QList<ReferenceIndex> &getReferenceIndices()const;
private:
    QList<ReferenceIndex> referenceIndices;
};

} // namespace BAM
} // namespace U2

#endif // _U2_BAM_INDEX_H_
