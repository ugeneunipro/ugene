/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "PrimerPair.h"

#include "PrimerSingle.h"

#include <U2Core/U2SafePoints.h>

namespace U2 {

PrimerPair::PrimerPair(const primer_pair& primerPair, int offset)
    : leftPrimer((primerPair.left == nullptr) ? nullptr : new PrimerSingle(*primerPair.left, oligo_type::OT_LEFT, offset)),
      rightPrimer((primerPair.right == nullptr) ? nullptr : new PrimerSingle(*primerPair.right, oligo_type::OT_RIGHT, offset)),
      internalOligo((primerPair.intl == nullptr) ? nullptr : new PrimerSingle(*primerPair.intl, oligo_type::OT_INTL, offset)),
      complAny(primerPair.compl_any),
      complEnd(primerPair.compl_end),
      productSize(primerPair.product_size),
      quality(primerPair.pair_quality),
      tm(primerPair.product_tm),
      repeatSim(primerPair.repeat_sim),
      repeatSimName(primerPair.rep_name),
      complAnyStruct(primerPair.compl_any_struct),
      complEndStruct(primerPair.compl_end_struct) {
}

PrimerPair::PrimerPair(const PrimerPair& primerPair)
    : leftPrimer((primerPair.leftPrimer.isNull()) ? nullptr : new PrimerSingle(*primerPair.leftPrimer)),
      rightPrimer((primerPair.rightPrimer.isNull()) ? nullptr : new PrimerSingle(*primerPair.rightPrimer)),
      internalOligo((primerPair.internalOligo.isNull()) ? nullptr : new PrimerSingle(*primerPair.internalOligo)),
      complAny(primerPair.complAny),
      complEnd(primerPair.complEnd),
      productSize(primerPair.productSize),
      quality(primerPair.quality),
      tm(primerPair.tm),
      repeatSim(primerPair.repeatSim),
      repeatSimName(primerPair.repeatSimName),
      complAnyStruct(primerPair.complAnyStruct),
      complEndStruct(primerPair.complEndStruct) {
}

PrimerPair& PrimerPair::operator=(const PrimerPair& primerPair) {
    leftPrimer.reset((primerPair.leftPrimer.isNull()) ? nullptr : new PrimerSingle(*primerPair.leftPrimer));
    rightPrimer.reset((primerPair.rightPrimer.isNull()) ? nullptr : new PrimerSingle(*primerPair.rightPrimer));
    internalOligo.reset((primerPair.internalOligo.isNull()) ? nullptr : new PrimerSingle(*primerPair.internalOligo));
    complAny = primerPair.complAny;
    complEnd = primerPair.complEnd;
    productSize = primerPair.productSize;
    quality = primerPair.quality;
    tm = primerPair.tm;
    repeatSim = primerPair.repeatSim;
    repeatSimName = primerPair.repeatSimName;
    complAnyStruct = primerPair.complAnyStruct;
    complEndStruct = primerPair.complEndStruct;

    return *this;
}

bool PrimerPair::operator<(const PrimerPair& pair) const {
    CHECK(quality == pair.quality, quality < pair.quality);
    CHECK(tm == pair.tm, tm < pair.tm);
    CHECK(leftPrimer->start == pair.leftPrimer->start, leftPrimer->start > pair.leftPrimer->start);
    CHECK(rightPrimer->start == pair.rightPrimer->start, rightPrimer->start < pair.rightPrimer->start);
    CHECK(leftPrimer->length == pair.leftPrimer->length, leftPrimer->length < pair.leftPrimer->length);
    CHECK(rightPrimer->length == pair.rightPrimer->length, rightPrimer->length < pair.rightPrimer->length);

    return false;
}


}
