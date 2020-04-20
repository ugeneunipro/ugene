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

#include <QColor>

#include "MsaHighlightingSchemeTransitions.h"

namespace U2 {

MsaHighlightingSchemeTransitions::MsaHighlightingSchemeTransitions(QObject *parent, const MsaHighlightingSchemeFactory *factory, MultipleAlignmentObject *maObj)
    : MsaHighlightingScheme(parent, factory, maObj)
{

}

void MsaHighlightingSchemeTransitions::process(const char refChar, char &seqChar, QColor &color, bool &highlight, int refCharColumn, int refCharRow) const {
    switch (refChar) {
    case 'N':
        highlight = true;
        break;
    case 'A':
        highlight = (seqChar == 'G');
        break;
    case 'C':
        highlight = (seqChar == 'T');
        break;
    case 'G':
        highlight = (seqChar == 'A');
        break;
    case 'T':
        highlight = (seqChar == 'C');
        break;
    default:
        highlight = false;
        break;
    }

    if (!highlight) {
        color = QColor();
    }

    MsaHighlightingScheme::process(refChar, seqChar, color, highlight, refCharColumn, refCharRow);
}

MsaHighlightingSchemeTransitionsFactory::MsaHighlightingSchemeTransitionsFactory(QObject *parent, const QString &id, const QString &name, const AlphabetFlags &supportedAlphabets)
    : MsaHighlightingSchemeFactory(parent, id, name, supportedAlphabets)
{

}

MsaHighlightingScheme * MsaHighlightingSchemeTransitionsFactory::create(QObject *parent, MultipleAlignmentObject *maObj ) const {
    return new MsaHighlightingSchemeTransitions(parent, this, maObj);
}

}   // namespace U2
