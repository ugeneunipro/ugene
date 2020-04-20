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

#ifndef _U2_MSA_HIGHLIGHTING_SCHEME_AGREEMENTS_H_
#define _U2_MSA_HIGHLIGHTING_SCHEME_AGREEMENTS_H_

#include "MsaHighlightingScheme.h"

namespace U2 {

class MsaHighlightingSchemeAgreements : public MsaHighlightingScheme{
    Q_OBJECT
public:
    MsaHighlightingSchemeAgreements(QObject *parent, const MsaHighlightingSchemeFactory *factory, MultipleAlignmentObject *maObj);

    void process(const char refChar, char &seqChar, QColor &color, bool &highlight, int refCharColumn, int refCharRow) const;
};


class U2ALGORITHM_EXPORT MsaHighlightingSchemeAgreementsFactory : public MsaHighlightingSchemeFactory {
public:
    MsaHighlightingSchemeAgreementsFactory(QObject *parent, const QString &id, const QString &name, const AlphabetFlags &supportedAlphabets);

    MsaHighlightingScheme * create(QObject *parent, MultipleAlignmentObject * maObj) const;
};

}   // namespace U2

#endif // _U2_MSA_HIGHLIGHTING_SCHEME_AGREEMENTS_H_
