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

#include "SmithWatermanResult.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// SmithWatermanResult
//////////////////////////////////////////////////////////////////////////

SharedAnnotationData SmithWatermanResult::toAnnotation(const QString &name) const {
    SharedAnnotationData data(new AnnotationData);
    data->name = name;
    data->location->regions << refSubseq;
    if (isJoined) {
        data->location->regions << refJoinedSubseq;
    }
    data->setStrand(strand);
    data->qualifiers.append(U2Qualifier("score", QString::number(score)));
    return data;
}

bool SmithWatermanResult::operator <(const SmithWatermanResult &op1) const {
    return score < op1.score;
}

//////////////////////////////////////////////////////////////////////////
/// SmithWatermanResultListener
//////////////////////////////////////////////////////////////////////////

SmithWatermanResultListener::~SmithWatermanResultListener() {

}

void SmithWatermanResultListener::pushResult(const SmithWatermanResult& r) {
    result.append(r);
}

void SmithWatermanResultListener::pushResult(const QList<SmithWatermanResult>& r) {
    result.append(r);
}

QList<SmithWatermanResult> SmithWatermanResultListener::popResults() {
    QList<SmithWatermanResult> res = result;
    result.clear();
    return res;
}

QList<SmithWatermanResult> SmithWatermanResultListener::getResults() const {
    return result;
}

} // namespace
