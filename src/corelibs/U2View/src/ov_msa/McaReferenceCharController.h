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

#ifndef _U2_MCA_REFERENCE_RULER_CONTROLLER_H_
#define _U2_MCA_REFERENCE_RULER_CONTROLLER_H_

#include <U2Core/U2Region.h>

namespace U2 {

class McaEditor;
class U2SequenceObject;

class MultipleAlignment;
class MaModificationInfo;

class OffsetRegions {
public:
    OffsetRegions();

    void append(const U2Region& region, int offset);
    int findIntersectedRegion(const U2Region& region) const;

    U2Region getRegion(int i) const;
    int getOffset(int i) const;
    int getSize() const;

    void clear();

private:
    QVector<U2Region>   regions;
    QVector<int>        offsets;
};

class McaReferenceCharController : public QObject {
    Q_OBJECT
public:
    McaReferenceCharController(QObject* p, McaEditor* editor);

    OffsetRegions getCharRegions(const U2Region& region) const;

    int getUngappedPosition(int pos) const;
    int getUngappedLength() const;

signals:
    void si_cacheUpdated();

public slots:
    void sl_update();
    void sl_update(const MultipleAlignment &maBefore, const MaModificationInfo &modInfo);

private:
    void initRegions();

    OffsetRegions       charRegions;
    U2SequenceObject*   refObject;
    int                 ungappedLength;
};

} // namespace

#endif // _U2_MCA_REFERENCE_RULER_CONTROLLER_H_
