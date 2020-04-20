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

#ifndef _U2_DNA_SEQUENCE_OBJECT_H_
#define _U2_DNA_SEQUENCE_OBJECT_H_

#include <U2Core/DNASequence.h>
#include <U2Core/GObject.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2Type.h>

namespace U2 {

class DNAAlphabet;
class U2OpStatus;

class U2CORE_EXPORT U2SequenceObject : public GObject {
    Q_OBJECT
public:
    U2SequenceObject(const QString& name, const U2EntityRef& seqRef, const QVariantMap& hintsMap = QVariantMap());

    U2EntityRef getSequenceRef() const {return getEntityRef();}

    qint64 getSequenceLength() const;

    QString getSequenceName() const;

    DNASequence getSequence(const U2Region &region, U2OpStatus& os) const;
    DNASequence getWholeSequence(U2OpStatus& os) const;

    QByteArray getWholeSequenceData(U2OpStatus& os) const;

    /** Obsolete, use the next method instead */
    QByteArray getSequenceData(const U2Region& r) const;

    QByteArray getSequenceData(const U2Region& r, U2OpStatus& os) const;

    /** Checks whether getting object from dbi can be performed correctly, error returned through U2OpStatus */
    bool isValidDbiObject(U2OpStatus &os);

    void setWholeSequence(const DNASequence& seq);

    virtual void setGObjectName(const QString& newName);

    bool isCircular() const;

    void setCircular(bool v);

    const DNAAlphabet* getAlphabet() const;

    void replaceRegion(const U2Region& region, const DNASequence& seq, U2OpStatus& os);
    void replaceRegion(const U2DataId &masterId, const U2Region &region, const DNASequence &seq, U2OpStatus &os);

    void removeRegion(U2OpStatus &os, const U2Region &region);

    GObject* clone(const U2DbiRef& ref, U2OpStatus& os, const QVariantMap &hints = QVariantMap()) const;

    bool checkConstraints(const GObjectConstraints* c) const;

    void setQuality(const DNAQuality& q);

    DNAQuality getQuality() const;

    void setSequenceInfo(const QVariantMap& info);

    QVariantMap getSequenceInfo() const;

    QString getStringAttribute(const QString& seqAttr) const;

    void setStringAttribute(const QString& newStringAttributeValue, const QString& type);

    qint64 getIntegerAttribute(const QString& seqAttr) const;

    void setIntegerAttribute(int newIntegerAttributeValue, const QString& type);

    double getRealAttribute(const QString& seqAttr) const;

    void setRealAttribute(double newRealAttributeValue, const QString& type);

    QByteArray getByteArrayAttribute(const QString& seqAttr) const;

    void setByteArrayAttribute(const QByteArray& newByteArrayAttributeValue, const QString& type);

    static bool lessThan( const U2SequenceObject *one, const U2SequenceObject *two){return one->name < two->name;}

private slots:
    void sl_resetDataCaches();

signals:
    void si_sequenceChanged();
    void si_sequenceCircularStateChanged();

protected:
    void updateCachedValues() const;

    mutable const DNAAlphabet*          cachedAlphabet;
    mutable qint64                      cachedLength;
    mutable QString                     cachedName;
    mutable TriState                    cachedCircular;
    mutable QPair<U2Region, QByteArray> cachedLastAccessedRegion;
};


class U2CORE_EXPORT U2SequenceObjectConstraints : public GObjectConstraints   {
    Q_OBJECT
public:
    U2SequenceObjectConstraints(QObject* p = NULL);
    qint64          sequenceSize;
    DNAAlphabetType alphabetType;
};


}//namespace


#endif
