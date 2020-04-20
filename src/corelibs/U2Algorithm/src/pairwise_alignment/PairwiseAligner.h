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

#ifndef _U2_PAIRWISE_ALIGNER_H_
#define _U2_PAIRWISE_ALIGNER_H_

#include <U2Core/MultipleSequenceAlignment.h>

namespace U2 {

class U2ALGORITHM_EXPORT PairwiseAligner {
public:
    PairwiseAligner(const QByteArray &seq1, const QByteArray &seq2);
    virtual ~PairwiseAligner();

    virtual MultipleSequenceAlignment align() = 0;
    virtual void setSeq1(const QByteArray &value);
    virtual void setSeq2(const QByteArray &value);
    virtual void setSeqs(const QByteArray &value1, const QByteArray &value2);

protected:
    QByteArray seq1;
    QByteArray seq2;
};

class U2ALGORITHM_EXPORT PairwiseAlignerFactory {
public:
    static PairwiseAligner * createAligner(const QString &alignerId,
        const QByteArray &seq1, const QByteArray &seq2);

    static const QString NEEDLEMAN_WUNSCH;
};

} // U2

#endif // _U2_PAIRWISE_ALIGNER_H_
