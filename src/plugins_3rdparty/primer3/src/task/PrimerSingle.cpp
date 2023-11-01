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

#include "PrimerSingle.h"

namespace U2 {

PrimerSingle::PrimerSingle(oligo_type type)
    : type(type) {
}

PrimerSingle::PrimerSingle(const primer_rec& primerRec, oligo_type _type, int offset)
    : start(primerRec.start + offset),
      length(primerRec.length),
      meltingTemperature(primerRec.temp),
      bound(primerRec.bound),
      gcContent(primerRec.gc_content),
      selfAny(primerRec.self_any),
      selfEnd(primerRec.self_end),
      templateMispriming(oligo_max_template_mispriming(&primerRec)),
      hairpin(primerRec.hairpin_th),
      endStability(primerRec.end_stability),
      quality(primerRec.quality),
      repeatSim(primerRec.repeat_sim.score != nullptr ? primerRec.repeat_sim.score[primerRec.repeat_sim.max] : 0),
      repeatSimName(primerRec.repeat_sim.name),
      selfAnyStruct(primerRec.self_any_struct),
      selfEndStruct(primerRec.self_end_struct),
      type(_type) {
    if (type == oligo_type::OT_RIGHT) {
        // Primer3 calculates all positions from 5' to 3' sequence ends -
        // from the left to the right in case of the direct sequence and from the right to the left in case of the reverse-complementary sequence
        start = start - length + 1;
    }
}

int PrimerSingle::getStart() const {
    return start;
}

int PrimerSingle::getLength() const {
    return length;
}

double PrimerSingle::getMeltingTemperature() const {
    return meltingTemperature;
}

double PrimerSingle::getBound() const {
    return bound;
}

double PrimerSingle::getGcContent() const {
    return gcContent;
}

double PrimerSingle::getSelfAny() const {
    return selfAny;
}

double PrimerSingle::getSelfEnd() const {
    return selfEnd;
}

double PrimerSingle::getTemplateMispriming() const {
    return templateMispriming;
}

double PrimerSingle::getHairpin() const {
    return hairpin;
}

double PrimerSingle::getEndStability() const {
    return endStability;
}

double PrimerSingle::getQuality() const {
    return quality;
}

double PrimerSingle::getRepeatSim() const {
    return repeatSim;
}

const QString& PrimerSingle::getRepeatSimName() const {
    return repeatSimName;
}

const QString& PrimerSingle::getSelfAnyStruct() const {
    return selfAnyStruct;
}

const QString& PrimerSingle::getSelfEndStruct() const {
    return selfEndStruct;
}

oligo_type PrimerSingle::getType() const {
    return type;
}

void PrimerSingle::setStart(int start) {
    this->start = start;
}

void PrimerSingle::setLength(int length) {
    this->length = length;
}

void PrimerSingle::setMeltingTemperature(double meltingTemperature) {
    this->meltingTemperature = meltingTemperature;
}

void PrimerSingle::setBound(double bound) {
    this->bound = bound;
}

void PrimerSingle::setGcContent(double gcContent) {
    this->gcContent = gcContent;
}

void PrimerSingle::setSelfAny(double selfAny) {
    this->selfAny = selfAny;
}

void PrimerSingle::setSelfEnd(double selfEnd) {
    this->selfEnd = selfEnd;
}

void PrimerSingle::setTemplateMispriming(double templateMispriming) {
    this->templateMispriming = templateMispriming;
}

void PrimerSingle::setHairpin(double hairpin) {
    this->hairpin = hairpin;
}

void PrimerSingle::setEndStability(double endStability) {
    this->endStability = endStability;
}

void PrimerSingle::setQuality(double quality) {
    this->quality = quality;
}

void PrimerSingle::setRepeatSim(double repeatSim) {
    this->repeatSim = repeatSim;
}

void PrimerSingle::setRepeatSimName(const QString& repeatSimName) {
    this->repeatSimName = repeatSimName;
}

void PrimerSingle::setSelfAnyStruct(const QString& selfAnyStruct) {
    this->selfAnyStruct = selfAnyStruct;
}

void PrimerSingle::setSelfEndStruct(const QString& selfEndStruct) {
    this->selfEndStruct = selfEndStruct;
}

QVector<U2Region> PrimerSingle::getSequenceRegions(int sequenceLength) const {
    int correctedStart = start + (start > sequenceLength ? (-sequenceLength) : 0);
    QVector<U2Region> result;
    if (correctedStart + length <= sequenceLength) {
        result << U2Region(correctedStart, length);
    } else {
        // primer covers the junction point
        result << U2Region(correctedStart, sequenceLength - correctedStart);
        result << U2Region(0, correctedStart + length - sequenceLength);
    }

    return result;
}


}
