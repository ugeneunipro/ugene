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

#include "Index.h"

namespace U2 {
namespace BAM {

// Index::ReferenceIndex::Chunk

Index::ReferenceIndex::Chunk::Chunk(VirtualOffset start, VirtualOffset end):
    start(start),
    end(end)
{
}

VirtualOffset Index::ReferenceIndex::Chunk::getStart()const {
    return start;
}
VirtualOffset Index::ReferenceIndex::Chunk::getEnd()const {
    return end;
}

// Index::ReferenceIndex::Bin

Index::ReferenceIndex::Bin::Bin(unsigned int _bin, QList<Chunk> _chunks) : bin(_bin), chunks(_chunks) {}

unsigned int Index::ReferenceIndex::Bin::getBin() const {
    return bin;
}

const QList<Index::ReferenceIndex::Chunk>& Index::ReferenceIndex::Bin::getChunks() const {
    return chunks;
}

// Index::ReferenceIndex

Index::ReferenceIndex::ReferenceIndex(const QList<Bin> &bins, const QList<VirtualOffset> &intervals):
    bins(bins),
    intervals(intervals)
{
}

const QList<Index::ReferenceIndex::Bin> &Index::ReferenceIndex::getBins()const {
    return bins;
}

const QList<VirtualOffset> &Index::ReferenceIndex::getIntervals()const {
    return intervals;
}

// Index

Index::Index(const QList<ReferenceIndex> &referenceIndices):
    referenceIndices(referenceIndices)
{
}

const QList<Index::ReferenceIndex> &Index::getReferenceIndices()const {
    return referenceIndices;
}

} // namespace BAM
} // namespace U2
