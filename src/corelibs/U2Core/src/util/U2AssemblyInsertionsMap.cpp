/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "U2AssemblyInsertionsMap.h"

#include <QMap>

#include <U2Core/U2AssemblyReadIterator.h>
#include <U2Core/U2AssemblyUtils.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const int U2AssemblyInsertionsMap::MAX_SITE_WIDTH = 20;

void U2AssemblyInsertionsMap::clear() {
    region = U2Region();
    sites.clear();
    totalWidth = 0;
}

void U2AssemblyInsertionsMap::build(const QList<U2AssemblyRead>& reads, const U2Region& regionToBuild) {
    clear();
    region = regionToBuild;
    CHECK(!region.isEmpty(), );

    QMap<qint64, int> widthByRefPos;
    for (const U2AssemblyRead& read : qAsConst(reads)) {
        CHECK_CONTINUE(read.data() != nullptr);

        U2Region readBases(read->leftmostPos, U2AssemblyUtils::getEffectiveReadLength(read));
        U2Region visibleBases = readBases.intersect(region);
        CHECK_CONTINUE(!visibleBases.isEmpty());

        // A read with no CIGAR is shown as a plain match, so it has nothing to contribute here.
        CHECK_CONTINUE(!read->cigar.isEmpty());

        U2AssemblyReadIterator it(read->readSequence, read->cigar, visibleBases.startPos - readBases.startPos);
        for (qint64 refPos = visibleBases.startPos; refPos < visibleBases.endPos() && it.hasNext(); refPos++) {
            it.nextLetter();
            int insertionLength = it.getInsertionBeforeLastLetter().size();
            CHECK_CONTINUE(insertionLength > 0);

            int width = qMin(insertionLength, MAX_SITE_WIDTH);
            widthByRefPos[refPos] = qMax(widthByRefPos.value(refPos), width);
        }
    }

    sites.reserve(widthByRefPos.size());
    for (auto it = widthByRefPos.constBegin(); it != widthByRefPos.constEnd(); ++it) {
        sites << U2AssemblyInsertionSite(it.key(), it.value());
        totalWidth += it.value();
    }
}

int U2AssemblyInsertionsMap::getInsertionWidthBefore(qint64 refPos) const {
    for (const U2AssemblyInsertionSite& site : qAsConst(sites)) {
        if (site.refPos == refPos) {
            return site.width;
        }
        if (site.refPos > refPos) {
            break;
        }
    }
    return 0;
}

qint64 U2AssemblyInsertionsMap::getColumnOfRefPos(qint64 refPos) const {
    qint64 column = refPos - region.startPos;
    for (const U2AssemblyInsertionSite& site : qAsConst(sites)) {
        if (site.refPos > refPos) {
            break;
        }
        column += site.width;
    }
    return column;
}

qint64 U2AssemblyInsertionsMap::getColumnCount() const {
    return region.length + totalWidth;
}

qint64 U2AssemblyInsertionsMap::getRefPosOfColumn(qint64 column) const {
    qint64 refPos = region.startPos;
    qint64 columnsLeft = column;
    for (const U2AssemblyInsertionSite& site : qAsConst(sites)) {
        qint64 referenceColumns = site.refPos - refPos;
        if (columnsLeft < referenceColumns) {
            break;
        }
        columnsLeft -= referenceColumns;
        refPos = site.refPos;
        if (columnsLeft < site.width) {
            return refPos;
        }
        columnsLeft -= site.width;
    }
    return refPos + columnsLeft;
}

}  // namespace U2
