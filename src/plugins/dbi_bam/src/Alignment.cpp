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

#include "Alignment.h"

namespace U2 {
namespace BAM {

// Alignment::CigarOperation
Alignment::CigarOperation::CigarOperation(int length, Operation operation):
    length(length),
    operation(operation)
{
}

int Alignment::CigarOperation::getLength()const {
    return length;
}

Alignment::CigarOperation::Operation Alignment::CigarOperation::getOperation()const {
    return operation;
}

void Alignment::CigarOperation::setLength(int length) {
    this->length = length;
}

void Alignment::CigarOperation::setOperation(Operation operation) {
    this->operation = operation;
}

// Alignment

Alignment::Alignment():
    referenceId(-1),
    position(-1),
    bin(0),
    mapQuality(0xff),
    flags(0),
    nextReferenceId(-1),
    nextReferenceName("*"),
    nextPosition(-1),
    templateLength(0)
{
}

int Alignment::getReferenceId()const {
    return referenceId;
}

int Alignment::getPosition()const {
    return position;
}

int Alignment::getBin()const {
    return bin;
}

int Alignment::getMapQuality()const {
    return mapQuality;
}

qint64 Alignment::getFlags()const {
    return flags;
}

int Alignment::getNextReferenceId()const {
    return nextReferenceId;
}

QByteArray Alignment::getNextReferenceName()const {
    return nextReferenceName;
}

int Alignment::getNextPosition()const {
    return nextPosition;
}

int Alignment::getTemplateLength()const {
    return templateLength;
}

const QByteArray &Alignment::getName()const {
    return name;
}

const QList<Alignment::CigarOperation> &Alignment::getCigar()const {
    return cigar;
}

const QByteArray &Alignment::getSequence()const {
    return sequence;
}

const QByteArray &Alignment::getQuality()const {
    return quality;
}

const QList<U2AuxData> & Alignment::getAuxData()const {
    return aux;
}

void Alignment::setReferenceId(int referenceId) {
    this->referenceId = referenceId;
}

void Alignment::setPosition(int position) {
    this->position = position;
}

void Alignment::setBin(int bin) {
    this->bin = bin;
}

void Alignment::setMapQuality(int mapQuality) {
    this->mapQuality = mapQuality;
}

void Alignment::setFlags(qint64 flags) {
    this->flags = flags;
}

void Alignment::setNextReferenceId(int nextReferenceId) {
    this->nextReferenceId = nextReferenceId;
}

void Alignment::setNextReferenceName(const QByteArray &nextReferenceName) {
    if (nextReferenceName.isEmpty()) {
        this->nextReferenceName = "*";
    }
    this->nextReferenceName = nextReferenceName;
}

void Alignment::setNextPosition(int nextPosition) {
    this->nextPosition = nextPosition;
}

void Alignment::setTemplateLength(int templateLength) {
    this->templateLength = templateLength;
}

void Alignment::setName(const QByteArray &name) {
    this->name = name;
}

void Alignment::setCigar(const QList<CigarOperation> &cigar) {
    this->cigar = cigar;
}

void Alignment::setSequence(const QByteArray &sequence) {
    this->sequence = sequence;
}

void Alignment::setQuality(const QByteArray &quality) {
    this->quality = quality;
}

void Alignment::setAuxData(const QList<U2AuxData> &aux) {
    this->aux = aux;
}

int Alignment::computeLength(const QList<CigarOperation> &cigar) {
    int length = 0;
    foreach(const Alignment::CigarOperation &operation, cigar) {
        if((operation.getOperation() != Alignment::CigarOperation::Insertion)) {
            length += operation.getLength();
        }
    }
    return length;
}

int Alignment::computeLength(const QList<U2CigarToken> &cigar) {
    int length = 0;
    foreach(const U2CigarToken &operation, cigar) {
        if((operation.op != U2CigarOp_I)) {
            length += operation.count;
        }
    }
    return length;
}

} // namespace BAM
} // namespace U2
