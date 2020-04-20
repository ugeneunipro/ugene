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

#include "CigarValidator.h"

#include "BAMDbiPlugin.h"
#include "InvalidFormatException.h"

namespace U2 {
namespace BAM {

CigarValidator::CigarValidator(QList<Alignment::CigarOperation> cigar_) : cigar(cigar_) {}

void CigarValidator::validate(int * totalLength) {
    const static Alignment::CigarOperation::Operation CIGAR_H = Alignment::CigarOperation::HardClip;
    const static Alignment::CigarOperation::Operation CIGAR_S = Alignment::CigarOperation::SoftClip;

    //bool hasRealOperation = false;
    int myTotalLength = 0;

    for(int i = 0; i < cigar.size(); ++i) {
        Alignment::CigarOperation cigarOperation = cigar.at(i);
        Alignment::CigarOperation::Operation op = cigarOperation.getOperation();

        if (isClippingOperation(op)) {
            //1. Check sentence "H can only be present as the first and/or last operation"
            if (CIGAR_H == op) {
                if (0 != i && (cigar.size() - 1) != i) {
                    throw InvalidFormatException(BAMDbiPlugin::tr("Hard clip in the middle of cigar"));
                }

            }
            //2. Check sentence "S may only have H operations between them and the ends of the CIGAR string"
            else if(CIGAR_S == op && (0 != i && cigar.size()-1 != i)) {
                if (1 == i) {
                    if (3 == cigar.size() && CIGAR_H == cigar.at(2).getOperation()) {
                        // ok
                    } else if (CIGAR_H != cigar.at(0).getOperation()) {
                        throw InvalidFormatException(BAMDbiPlugin::tr("Misplaced soft clip in the cigar: must be preceded with hard clip"));
                    }
                } else if (cigar.size() - 2 == i) {
                    if (CIGAR_H != cigar.at(cigar.size() - 1).getOperation()) {
                        throw InvalidFormatException(BAMDbiPlugin::tr("Misplaced soft clip in the cigar: must be succeeded with hard clip"));
                    }
                } else {
                    throw InvalidFormatException(BAMDbiPlugin::tr("Misplaced soft clip in the cigar: can't be in the middle of the string"));
                }

            }
        }
/*
 * Formally speaking this checks must be disabled since SAM-1.3-r882 spec does not contain such assertions.
 * However we keep them in source code in order to enable someday, for example to handle failed checks as warnings
 */
#if 0
        else if (isRealOperation(op)) {
            //1. Remember that CIGAR has at least one real operation
            hasRealOperation = true;
            //2. Validate that match or padding exists between in/del/skip operations
            if (isInDelOperation(op)) {
                for (int j = i+1; j < cigar.size(); ++j) {
                    const Alignment::CigarOperation::Operation nextOp = cigar.at(j).getOperation();
                    // ok
                    if ((isRealOperation(nextOp) && !isInDelOperation(nextOp)) || isPaddingOperation(nextOp)) {
                        break;
                    }
                    if (isInDelOperation(nextOp) && op == nextOp) {
                        throw InvalidFormatException(BAMDbiPlugin::tr("No match or skip operation between adjacent in/del/skip operations"));
                    }
                }
            }
        } else if (isPaddingOperation(op)) {
            //1. Check if padding is in the middle of CIGAR
            if (i == 0 || i == cigar.size() - 1) {
                throw InvalidFormatException(BAMDbiPlugin::tr("CIGAR can't start or end with padding operator"));
            } else if (!isRealOperation(cigar.at(i-1).getOperation()) || !isRealOperation(cigar.at(i+1).getOperation())) {
                throw InvalidFormatException(BAMDbiPlugin::tr("Padding operator must be located between real operations"));
            }
        }
#endif
        //adjust totalLength
        switch(op) {
        case Alignment::CigarOperation::AlignmentMatch:
        case Alignment::CigarOperation::SequenceMatch:
        case Alignment::CigarOperation::SequenceMismatch:
        case Alignment::CigarOperation::Insertion:
        case Alignment::CigarOperation::SoftClip:
            myTotalLength += cigarOperation.getLength();
            break;
        default:
            break;
        }
    } //foreach CIGAR operation
#if 0
    if (!hasRealOperation) {
        throw InvalidFormatException(BAMDbiPlugin::tr("CIGAR must contain at least one real operation (ins/del/match/skip)"));
    }
#endif
    assert(totalLength);
    *totalLength = myTotalLength;
}

bool CigarValidator::isClippingOperation(Alignment::CigarOperation::Operation op) {
    return Alignment::CigarOperation::HardClip == op ||
        Alignment::CigarOperation::SoftClip == op;
}

#if 0
bool CigarValidator::isRealOperation(Alignment::CigarOperation::Operation op) {
    switch(op) {
    case Alignment::CigarOperation::AlignmentMatch:
    case Alignment::CigarOperation::SequenceMatch:
    case Alignment::CigarOperation::SequenceMismatch:
    case Alignment::CigarOperation::Insertion:
    case Alignment::CigarOperation::Deletion:
    case Alignment::CigarOperation::Skipped:
        return true;
    default:
        return false;
    }
}

bool CigarValidator::isInDelOperation(Alignment::CigarOperation::Operation op) {
    return Alignment::CigarOperation::Insertion == op ||
        Alignment::CigarOperation::Deletion == op;
}

bool CigarValidator::isPaddingOperation(Alignment::CigarOperation::Operation op) {
    return Alignment::CigarOperation::Padding == op;
}
#endif

} //ns BAM
} //ns U2
