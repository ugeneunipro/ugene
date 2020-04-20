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

#include <U2Core/U2SafePoints.h>

#include "U2LongLongValidator.h"

namespace U2 {

U2LongLongValidator::U2LongLongValidator(qint64 minimum, qint64 maximum, QObject *parent) :
    QValidator(parent),
    minimum(minimum),
    maximum(maximum)
{
}

QValidator::State U2LongLongValidator::validate(QString &input, int & /*pos*/) const {
    CHECK(!input.isEmpty(), QValidator::Acceptable);

    if ((minimum >= 0 && input.startsWith('-')) || (maximum < 0 && input.startsWith('+'))) {
        return Invalid;
    }

    if (input.size() == 1 && (input[0] == '+' || input[0] == '-')) {
        return Intermediate;
    }

    bool isConverted = false;
    const qint64 currentValue = input.toLongLong(&isConverted);
    if (!isConverted) {
        return QValidator::Invalid;
    }

    if (currentValue < minimum) {
        return QValidator::Intermediate;
    }

    if (maximum < currentValue) {
        return QValidator::Invalid;
    }

    return QValidator::Acceptable;
}

}   // namespace U2
