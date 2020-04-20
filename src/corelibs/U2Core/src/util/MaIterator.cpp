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

#include <QPoint>

#include <U2Core/U2SafePoints.h>

#include "MaIterator.h"

namespace U2 {

const qint64 MaIterator::INVALID_POSITION = -1;

MaIterator::MaIterator(const MultipleAlignment &ma, NavigationDirection direction, const QList<int> &_rowsIndexes)
    : ma(ma),
      rowsIndexes(_rowsIndexes),
      direction(direction),
      isCircular(false),
      coreRegionsOnly(false),
      position(-1),
      maSquare(static_cast<qint64>(ma->getLength()) * rowsIndexes.size())
{
    if (rowsIndexes.isEmpty()) {
        for (int index = 0; index < ma->getNumRows(); index++) {
            rowsIndexes << index;
        }
        maSquare = static_cast<qint64>(ma->getLength()) * rowsIndexes.size();
    }
}

bool MaIterator::hasNext() const {
    CHECK(!ma->isEmpty(), false);
    return INVALID_POSITION != getNextPosition();
}

char MaIterator::next() {
    SAFE_POINT(hasNext(), "Out of boundaries", U2Msa::INVALID_CHAR);
    return *(operator ++());
}

MaIterator &MaIterator::operator ++() {
    SAFE_POINT(hasNext(), "Out of boundaries", *this);
    position = getNextPosition();
    SAFE_POINT(isInRange(position), "Out of boundaries", *this);
    return *this;
}

char MaIterator::operator *() {
    SAFE_POINT(isInRange(position), "Out of boundaries", U2Msa::INVALID_CHAR);
    const QPoint maPoint = getMaPoint();
    SAFE_POINT(0 <= maPoint.x() && maPoint.x() < ma->getLength() &&
               0 <= maPoint.y() && maPoint.y() < ma->getNumRows(), "Out of boundaries", U2Msa::INVALID_CHAR);
    return ma->charAt(maPoint.y(), maPoint.x());
}

bool MaIterator::operator ==(const MaIterator &other) const {
    return ma == other.ma && position == other.position;
}

void MaIterator::setCircular(bool isCircular) {
    this->isCircular = isCircular;
}

void MaIterator::setIterateInCoreRegionsOnly(bool coreRegionsOnly) {
    this->coreRegionsOnly = coreRegionsOnly;
}

void MaIterator::setMaPoint(const QPoint &maPoint) {
    const qint64 newPosition = maPoint.y() * ma->getLength() + maPoint.x();
    SAFE_POINT(isInRange(newPosition), "The new position is out of boundaries", );
    position = newPosition;
}

void MaIterator::setDirection(NavigationDirection newDirection) {
    direction = newDirection;
}

QPoint MaIterator::getMaPoint() const {
    SAFE_POINT(isInRange(position), "Out of boundaries", QPoint(-1, -1));
    return QPoint(getColumnNumber(position), getRowNumber(position));
}

bool MaIterator::isInRange(qint64 position) const {
    return (0 <= position && position < maSquare);
}

qint64 MaIterator::getNextPosition() const {
    CHECK(!ma->isEmpty(), INVALID_POSITION);

    qint64 nextPosition = position;
    const int step = getStep(nextPosition);
    switch (direction) {
    case Forward:
        nextPosition += step;
        break;
    case Backward:
        nextPosition -= step;
        break;
    default:
        FAIL("An unknown direction", INVALID_POSITION);
    }

    if (isCircular) {
        nextPosition = (nextPosition + maSquare) % maSquare;
    }

    CHECK(isInRange(nextPosition), INVALID_POSITION);
    return nextPosition;
}

int MaIterator::getStep(qint64 position) const {
    CHECK(!coreRegionsOnly, 1);
    SAFE_POINT(isInRange(position), "Out of boundaries", 1);
    const int rowNumber = getRowNumber(position);
    const int columnNumber = getColumnNumber(position);
    const MultipleAlignmentRow row = ma->getRow(rowsIndexes[rowNumber]);
    CHECK(!row->isTrailingOrLeadingGap(columnNumber), 1);
    switch (direction) {
    case Forward:
        return row->getCoreEnd() <= columnNumber ? ma->getLength() - columnNumber : 1;
    case Backward:
        return row->getCoreStart() >= columnNumber ? ma->getLength() - columnNumber : 1;
    default:
        FAIL("An unknown direction", 1);
    }
}

int MaIterator::getRowNumber(qint64 position) const {
    return static_cast<int>(position / ma->getLength());
}

int MaIterator::getColumnNumber(qint64 position) const {
    return static_cast<int>(position % ma->getLength());
}

}   // namespace U2
