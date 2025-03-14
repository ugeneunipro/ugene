/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QFlags>
#include <QList>
#include <QMap>
#include <QVariant>

#include <U2Core/U2Assembly.h>

namespace U2 {
namespace BAM {

class Alignment {
public:
    class CigarOperation {
    public:
        enum Operation {
            AlignmentMatch,
            Insertion,
            Deletion,
            Skipped,
            SoftClip,
            HardClip,
            Padding,
            SequenceMatch,
            SequenceMismatch
        };
        CigarOperation(int length, Operation operation);
        int getLength() const;
        Operation getOperation() const;
        void setLength(int length);
        void setOperation(Operation operation);

    private:
        int length;
        Operation operation;
    };
    Alignment();
    int getReferenceId() const;
    int getPosition() const;
    int getBin() const;
    int getMapQuality() const;
    const ReadFlags& getFlags() const;
    int getNextReferenceId() const;
    QByteArray getNextReferenceName() const;
    int getNextPosition() const;
    int getTemplateLength() const;
    const QByteArray& getName() const;
    const QList<CigarOperation>& getCigar() const;
    const QByteArray& getSequence() const;
    const QByteArray& getQuality() const;
    const QList<U2AuxData>& getAuxData() const;
    void setReferenceId(int referenceId);
    void setPosition(int position);
    void setBin(int bin);
    void setMapQuality(int mapQuality);
    void setFlags(const ReadFlags& flags);
    void setNextReferenceId(int nextReferenceId);
    void setNextReferenceName(const QByteArray& nextReferenceName);
    void setNextPosition(int nextPosition);
    void setTemplateLength(int templateLength);
    void setName(const QByteArray& name);
    void setCigar(const QList<CigarOperation>& cigar);
    void setSequence(const QByteArray& sequence);
    void setQuality(const QByteArray& quality);
    void setAuxData(const QList<U2AuxData>& aux);

    static int computeLength(const QList<CigarOperation>& cigar);
    static int computeLength(const QList<U2CigarToken>& cigar);

private:
    int referenceId;
    int position;
    int bin;
    int mapQuality;
    ReadFlags flags;
    int nextReferenceId;
    QByteArray nextReferenceName;
    int nextPosition;
    int templateLength;
    QByteArray name;
    QList<CigarOperation> cigar;
    QByteArray sequence;
    QByteArray quality;
    QList<U2AuxData> aux;
};

}  // namespace BAM
}  // namespace U2
