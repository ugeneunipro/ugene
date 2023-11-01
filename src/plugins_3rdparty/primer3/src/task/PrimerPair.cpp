/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

QSharedPointer<PrimerSingle> PrimerPair::getLeftPrimer() const {
    return leftPrimer;
}

QSharedPointer<PrimerSingle> PrimerPair::getRightPrimer() const {
    return rightPrimer;
}

QSharedPointer<PrimerSingle> PrimerPair::getInternalOligo() const {
    return internalOligo;
}

double PrimerPair::getComplAny() const {
    return complAny;
}

double PrimerPair::getComplEnd() const {
    return complEnd;
}

int PrimerPair::getProductSize() const {
    return productSize;
}

double PrimerPair::getProductQuality() const {
    return quality;
}

double PrimerPair::getProductTm() const {
    return tm;
}

void PrimerPair::setLeftPrimer(const QSharedPointer<PrimerSingle>& _leftPrimer) {
    leftPrimer.reset(_leftPrimer.isNull() ? nullptr : new PrimerSingle(*_leftPrimer));
}

void PrimerPair::setRightPrimer(const QSharedPointer<PrimerSingle>& _rightPrimer) {
    rightPrimer.reset(_rightPrimer.isNull() ? nullptr : new PrimerSingle(*_rightPrimer));
}

void PrimerPair::setInternalOligo(const QSharedPointer<PrimerSingle>& _internalOligo) {
    internalOligo.reset(_internalOligo.isNull() ? nullptr : new PrimerSingle(*_internalOligo));
}

void PrimerPair::setComplAny(double newComplAny) {
    complAny = newComplAny;
}

void PrimerPair::setComplEnd(double newComplEnd) {
    complEnd = newComplEnd;
}

void PrimerPair::setProductSize(int newProductSize) {
    productSize = newProductSize;
}

double PrimerPair::getRepeatSim() const {
    return repeatSim;
}

void PrimerPair::setProductQuality(double quality) {
    this->quality = quality;
}

const QString& PrimerPair::getRepeatSimName() const {
    return repeatSimName;
}

void PrimerPair::setProductTm(double tm) {
    this->tm = tm;
}

const QString& PrimerPair::getComplAnyStruct() const {
    return complAnyStruct;
}

const QString& PrimerPair::getComplEndStruct() const {
    return complEndStruct;
}

void PrimerPair::setRepeatSim(double repeatSim) {
    this->repeatSim = repeatSim;
}

void PrimerPair::setRepeatSimName(const QString& repeatSimName) {
    this->repeatSimName = repeatSimName;
}

void PrimerPair::setComplAnyStruct(const QString& complAnyStruct) {
    this->complAnyStruct = complAnyStruct;
}

void PrimerPair::setComplEndStruct(const QString& complEndStruct) {
    this->complEndStruct = complEndStruct;
}

bool PrimerPair::operator<(const PrimerPair& pair) const {
    if (quality < pair.quality) {
        return true;
    }
    if (quality > pair.quality) {
        return false;
    }
    if (tm < pair.tm) {
        return true;
    }
    if (tm > pair.tm) {
        return false;
    }
    if (leftPrimer->getStart() > pair.leftPrimer->getStart()) {
        return true;
    }
    if (leftPrimer->getStart() < pair.leftPrimer->getStart()) {
        return false;
    }

    if (rightPrimer->getStart() < pair.rightPrimer->getStart()) {
        return true;
    }
    if (rightPrimer->getStart() > pair.rightPrimer->getStart()) {
        return false;
    }

    if (leftPrimer->getLength() < pair.leftPrimer->getLength()) {
        return true;
    }
    if (leftPrimer->getLength() > pair.leftPrimer->getLength()) {
        return false;
    }

    if (rightPrimer->getLength() < pair.rightPrimer->getLength()) {
        return true;
    }
    if (rightPrimer->getLength() > pair.rightPrimer->getLength()) {
        return false;
    }

    return false;
}


}
