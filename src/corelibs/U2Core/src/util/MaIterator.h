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

#ifndef _MA_ITERATOR_H_
#define _MA_ITERATOR_H_

#include <U2Core/MultipleAlignment.h>

namespace U2 {

class U2CORE_EXPORT MaIterator {
public:
    MaIterator(const MultipleAlignment &ma, NavigationDirection direction, const QList<int> &rowsIndexes = QList<int>());

    bool hasNext() const;
    char next();

    MaIterator &operator ++();
    char operator *();

    bool operator ==(const MaIterator &other) const;
    bool operator !=(const MaIterator &other) const;

    void setCircular(bool isCircular);
    void setIterateInCoreRegionsOnly(bool coreRegionsOnly);
    void setMaPoint(const QPoint &maPoint);
    void setDirection(NavigationDirection direction);

    QPoint getMaPoint() const;

protected:
    bool isInRange(qint64 position) const;
    qint64 getNextPosition() const;
    int getStep(qint64 position) const;

    int getRowNumber(qint64 position) const;
    int getColumnNumber(qint64 position) const;

    const MultipleAlignment ma;
    QList<int> rowsIndexes;
    NavigationDirection direction;

    bool isCircular;
    bool coreRegionsOnly;

    qint64 position;
    qint64 maSquare;

    static const qint64 INVALID_POSITION;
};

}   // namespace U2

#endif // _MA_ITERATOR_H_
