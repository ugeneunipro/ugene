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

#ifndef __CIGAR_VALIDATOR_H__
#define __CIGAR_VALIDATOR_H__

#include "Alignment.h"

namespace U2{
namespace BAM{

class CigarValidator {
public:
    CigarValidator(QList<Alignment::CigarOperation> cigar_);

    //fulfills the totalLength so caller can check if it conforms to read length
    void validate(int * totalLength);

private:
    bool static isClippingOperation(Alignment::CigarOperation::Operation op);

#if 0
    bool static isRealOperation(Alignment::CigarOperation::Operation op);

    bool static isInDelOperation(Alignment::CigarOperation::Operation op);

    bool static isPaddingOperation(Alignment::CigarOperation::Operation op);
#endif

    QList<Alignment::CigarOperation> cigar;
};

}
}

#endif
