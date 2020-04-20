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

#ifndef _U2_DNA_SEQUENCE_H_
#define _U2_DNA_SEQUENCE_H_

#include <QVariantMap>

#include "DNAInfo.h"
#include "DNAQuality.h"

namespace U2 {

class DNAAlphabet;

class U2CORE_EXPORT DNASequence {
public:
    DNASequence(const QString& name, const QByteArray& s = QByteArray(), const DNAAlphabet* a = NULL);
    DNASequence(const QByteArray& s = QByteArray(), const DNAAlphabet* a = NULL);

    QVariantMap     info;
    QByteArray      seq;
    const DNAAlphabet*    alphabet;
    bool            circular;
    DNAQuality      quality;

    QString getName() const { return DNAInfo::getName(info);}
    void setName(const QString& name);
    bool isNull() const {return !alphabet && seq.length() == 0;}
    int length() const {return seq.length();}
    bool hasQualityScores() const {return (!quality.isEmpty()); }
    const char* constData() const {return seq.constData();}
    const QByteArray &constSequence() const {return seq;}
};

}//namespace

Q_DECLARE_METATYPE(U2::DNASequence)
Q_DECLARE_METATYPE(QList<U2::DNASequence>)

#endif
