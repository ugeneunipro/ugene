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

#ifndef _U2_SW_MUL_ALIGN_POS_NUMBER_TAG_H_
#define _U2_SW_MUL_ALIGN_POS_NUMBER_TAG_H_

#include "SWMulAlignResultNamesTag.h"

namespace U2 {

class SWMulAlignExternalPropTag : public SWMulAlignResultNamesTag {
public:
    enum Type {
        DATE, TIME, COUNTER
    };

    inline SWMulAlignExternalPropTag(const QString _shorthand, const QString _label, Type _type);
    virtual QString expandTag(const QVariant & argument) const;
    inline void resetCounter();
    inline Type getType() const;

private:
    mutable quint32 counter;
    Type type;
};



inline SWMulAlignExternalPropTag::SWMulAlignExternalPropTag(const QString _shorthand, const QString _label, Type _type)
    : SWMulAlignResultNamesTag(_shorthand, _label), counter(0), type(_type) {
    acceptableForSubseqNamesOnly = false;
}

inline void SWMulAlignExternalPropTag::resetCounter() {
    assert(COUNTER == type);
    counter = 0;
}

inline SWMulAlignExternalPropTag::Type SWMulAlignExternalPropTag::getType() const {
    return type;
}

} // namespace

#endif // _U2_SW_MUL_ALIGN_POS_NUMBER_TAG_H_
