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

#pragma once

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

    U2EntityRef getSequenceRef() const {
        return getEntityRef();
    }

    qint64 getSequenceLength() const;

    QString getSequenceName() const;

    DNASequence getSequence(const U2Region& region, U2OpStatus& os) const;
    DNASequence getWholeSequence(U2OpStatus& os) const;

    QByteArray getWholeSequenceData(U2OpStatus& os) const;

    /** Obsolete, use the next method instead */
    QByteArray getSequenceData(const U2Region& r) const;

    QByteArray getSequenceData(const U2Region& r, U2OpStatus& os) const;

    /** Checks whether getting object from dbi can be performed correctly, error returned through U2OpStatus */
    bool isValidDbiObject(U2OpStatus& os);

    void setWholeSequence(const DNASequence& seq);

    void setGObjectName(const QString& newName) override;

    bool isCircular() const;

    void setCircular(bool v);

    const DNAAlphabet* getAlphabet() const;

    void replaceRegion(const U2Region& region, const DNASequence& seq, U2OpStatus& os);
    void replaceRegion(const U2DataId& masterId, const U2Region& region, const DNASequence& seq, U2OpStatus& os);

    void removeRegion(U2OpStatus& os, const U2Region& region);

    GObject* clone(const U2DbiRef& ref, U2OpStatus& os, const QVariantMap& hints = QVariantMap()) const override;

    bool checkConstraints(const GObjectConstraints* c) const override;

    void setQuality(const DNAQuality& q);

    DNAQuality getQuality() const;

    void setSequenceInfo(const QVariantMap& info);

    QVariantMap getSequenceInfo() const;

    /** Returns string attribute value of empty string if the attribute does not exist. */
    QString getStringAttribute(const QString& name) const;

    /** Sets string attribute by name.*/
    void setStringAttribute(const QString& name, const QString& value);

    /** Returns integer attribute value of 0.0 if the attribute does not exist. */
    qint64 getIntegerAttribute(const QString& name) const;

    /** Sets integer attribute by name.*/
    void setIntegerAttribute(const QString& name, int value);

    /** Returns real attribute value of 0.0 if the attribute does not exist. */
    double getRealAttribute(const QString& name) const;

    /** Sets real attribute by name. */
    void setRealAttribute(const QString& name, double value);

    /** Returns QByteArray attribute value of empty QByteArray if the attribute does not exist. */
    QByteArray getByteArrayAttribute(const QString& name) const;

    /** Sets QByteArray attribute by name. */
    void setByteArrayAttribute(const QString& name, const QByteArray& value);

    /** Compares names of 2 sequence objects using '<' operator of QStrings.. */
    static bool lessThan(const U2SequenceObject* one, const U2SequenceObject* two) {
        return one->name < two->name;
    }

public slots:
    void sl_resetDataCaches();

signals:
    void si_sequenceChanged();
    void si_sequenceCircularStateChanged();

protected:
    void updateCachedValues() const;

    mutable const DNAAlphabet* cachedAlphabet;
    mutable qint64 cachedLength;
    mutable QString cachedName;
    mutable TriState cachedCircular;
    mutable QPair<U2Region, QByteArray> cachedLastAccessedRegion;
};

class U2CORE_EXPORT U2SequenceObjectConstraints : public GObjectConstraints {
    Q_OBJECT
public:
    U2SequenceObjectConstraints(QObject* p = nullptr);
    qint64 sequenceSize;
    DNAAlphabetType alphabetType;
};

}  // namespace U2
