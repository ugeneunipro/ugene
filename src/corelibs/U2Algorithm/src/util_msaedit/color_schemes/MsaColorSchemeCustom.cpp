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

#include "ColorSchemeUtils.h"
#include "MsaColorSchemeCustom.h"
#include "MsaColorSchemeStatic.h"

namespace U2 {

MsaColorSchemeCustomFactory::MsaColorSchemeCustomFactory(QObject *parent, const ColorSchemeData &scheme)
    : MsaColorSchemeFactory(parent, scheme.name, scheme.name, scheme.type | DNAAlphabet_RAW),
      colorsPerChar(colorMapToColorVector(scheme.alpColors))
{

}

MsaColorScheme * MsaColorSchemeCustomFactory::create(QObject *parent, MultipleAlignmentObject *maObj) const {
    return new MsaColorSchemeStatic(parent, this, maObj, colorsPerChar);
}

bool MsaColorSchemeCustomFactory::isEqualTo(const ColorSchemeData &scheme) const {
    bool result = true;
    result &= getName() == scheme.name;
    result &= isAlphabetTypeSupported(scheme.type);
    result &= colorsPerChar == colorMapToColorVector(scheme.alpColors);
    return result;
}

void MsaColorSchemeCustomFactory::setScheme(const ColorSchemeData &scheme) {
    CHECK(!isEqualTo(scheme), );
    name = scheme.name;
    supportedAlphabets &= ~supportedAlphabets;
    supportedAlphabets |= scheme.type;
    colorsPerChar = colorMapToColorVector(scheme.alpColors);
    emit si_factoryChanged();
}

QVector<QColor> MsaColorSchemeCustomFactory::colorMapToColorVector(const QMap<char, QColor> &map) {
    QVector<QColor> colorsPerChar;
    ColorSchemeUtils::fillEmptyColorScheme(colorsPerChar);
    QMapIterator<char, QColor> it(map);
    while (it.hasNext()) {
        it.next();
        colorsPerChar[it.key()] = colorsPerChar[it.key() + ('a'-'A')] = it.value();
    }
    return colorsPerChar;
}

}   // namespace U2
