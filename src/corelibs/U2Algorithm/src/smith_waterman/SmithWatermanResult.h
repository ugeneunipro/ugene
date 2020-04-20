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

#ifndef _U2_SMITH_WATERMAN_RESULT_H_
#define _U2_SMITH_WATERMAN_RESULT_H_

#include <U2Core/AnnotationData.h>

namespace U2 {

struct U2ALGORITHM_EXPORT SmithWatermanResult {
    SharedAnnotationData toAnnotation(const QString &name) const;
    bool operator <(const SmithWatermanResult &op1) const;

    U2Strand strand;
    bool trans;
    float score;
    U2Region refSubseq;
    bool isJoined;
    U2Region refJoinedSubseq;

    U2Region ptrnSubseq;
    QByteArray pairAlignment;

    static const char UP = 'u';
    static const char LEFT = 'l';
    static const char DIAG = 'd';
};

class U2ALGORITHM_EXPORT SmithWatermanResultListener {
public:
    virtual ~SmithWatermanResultListener();

    virtual void pushResult(const SmithWatermanResult &r);
    virtual void pushResult(const QList<SmithWatermanResult> &r);
    virtual QList<SmithWatermanResult> popResults();
    virtual QList<SmithWatermanResult> getResults() const;

private:
    QList<SmithWatermanResult> result;
};

} // namespace

#endif
