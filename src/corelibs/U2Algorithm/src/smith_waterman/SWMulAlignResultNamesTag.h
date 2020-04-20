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

#ifndef _U2_SW_MUL_ALIGN_RESULT_NAMES_TAGS_H_
#define _U2_SW_MUL_ALIGN_RESULT_NAMES_TAGS_H_

#include <U2Algorithm/SmithWatermanResult.h>

#include <QString>
#include <QVariant>

namespace U2 {

class U2ALGORITHM_EXPORT SWMulAlignResultNamesTag {
public:
    inline SWMulAlignResultNamesTag(const QString _shorthand, const QString _label);
    virtual ~SWMulAlignResultNamesTag() {}
    virtual QString expandTag(const QVariant & argument = QVariant ()) const = 0;

    inline const QString & getShorthand() const;
    inline const QString & getLabel() const;
    inline const bool & isAcceptableForSubseqNamesOnly() const;

protected:
    bool acceptableForSubseqNamesOnly;

private:
    const QString shorthand;
    const QString label;
};



inline SWMulAlignResultNamesTag::SWMulAlignResultNamesTag(const QString _shorthand, const QString _label)
    : shorthand(_shorthand), label(_label) {}

inline const QString & SWMulAlignResultNamesTag::getShorthand() const {
    return shorthand;
}

inline const QString & SWMulAlignResultNamesTag::getLabel() const {
    return label;
}

inline const bool & SWMulAlignResultNamesTag::isAcceptableForSubseqNamesOnly() const {
    return acceptableForSubseqNamesOnly;
}

} // namespace

#endif // _U2_SW_MUL_ALIGN_RESULT_NAMES_TAGS_H_
