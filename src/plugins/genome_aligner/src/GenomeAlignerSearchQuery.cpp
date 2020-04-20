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

#include "GenomeAlignerSearchQuery.h"
#include <U2Core/Log.h>
#include <limits.h>

namespace U2 {

SearchQuery::SearchQuery(const DNASequence *shortRead, SearchQuery *revCompl) {
    dna = true;
    wroteResult = false;
    this->revCompl = revCompl;
    seqLength = shortRead->length();
    nameLength = shortRead->getName().length();
    seq = new char[seqLength+1];
    name = new char[nameLength+1];
    qstrcpy(seq, shortRead->constData());
    qstrcpy(name, shortRead->getName().toLatin1().constData());
    if (shortRead->hasQualityScores()) {
        quality = new DNAQuality(shortRead->quality);
    } else {
        quality = NULL;
    }

    results.reserve(2);
    mismatchCounts.reserve(2);
    overlapResults.reserve(2);
}

qint64 SearchQuery::memoryHint() const {
    qint64 m = sizeof(*this);

    m += seqLength+1; // seq
    m += nameLength+1; // name

    m += results.capacity() * sizeof(SAType);
    m += overlapResults.capacity() * sizeof(SAType);
    m += mismatchCounts.capacity() * sizeof(quint32);

    m += quality ? quality->memoryHint() : 0;

    return m*2; // overhead due to many new calls of small regions
}

SearchQuery::SearchQuery(const U2AssemblyRead &, SearchQuery *revCompl) {
    dna = false;
    wroteResult = false;
    this->revCompl = revCompl;
    seq = NULL;
    name = NULL;
    quality = NULL;
}

SearchQuery::~SearchQuery() {
    delete seq;
    delete name;
    delete quality;
    revCompl = NULL;
}

QString SearchQuery::getName() const {
    if (dna) {
        return QString(name);
    } else {
        return NULL;
    }
}

int SearchQuery::length() const {
    if (dna) {
        return seqLength;
    } else {
        return 0;
    }
}

int SearchQuery::getNameLength() const {
    if (dna) {
        return nameLength;
    } else {
        return 0;
    }
}

char *SearchQuery::data() {
    if (dna) {
        return seq;
    } else {
        return NULL;
    }
}

const char *SearchQuery::constData() const {
    if (dna) {
        return seq;
    } else {
        return NULL;
    }
}

const QByteArray SearchQuery::constSequence() const {
    if (dna) {
        return QByteArray(seq);
    } else {
        return NULL;
    }
}

bool SearchQuery::hasQuality() const {
    if (dna) {
        return NULL != quality;
    } else {
        return false;
    }
}

const DNAQuality &SearchQuery::getQuality() const {
    assert(dna);
    return *quality;
}

bool SearchQuery::haveResult() const {
    return !results.isEmpty();
}

bool SearchQuery::haveMCount() const {
    return !mismatchCounts.isEmpty();
}

void SearchQuery::addResult(SAType result, quint32 mCount) {
    results.append(result);
    mismatchCounts.append(mCount);
}

void SearchQuery::addOveplapResult(SAType result) {
    overlapResults.append(result);
}

void SearchQuery::onPartChanged() {
    clear();
    results += overlapResults;
    overlapResults.clear();
}

void SearchQuery::clear() {
    results.clear();
    results.squeeze();
    mismatchCounts.clear();
    mismatchCounts.squeeze();
}

SAType SearchQuery::firstResult() const {
    return results.first();
}

quint32 SearchQuery::firstMCount() const {
    if (mismatchCounts.isEmpty()) {
        return INT_MAX;
    }
    return mismatchCounts.first();
}

bool SearchQuery::contains(SAType result) const {
    return (results.contains(result) || overlapResults.contains(result));
}

const QVector<SAType>& SearchQuery::getResults() const {
    return results;
}

const quint64 SearchQueryContainer::reallocSize = 100;
SearchQueryContainer::SearchQueryContainer() {
    queries = NULL;
    allocated = 0;
    length = 0;
}

SearchQueryContainer::~SearchQueryContainer() {
    delete[] queries;
    allocated = 0;
    length = 0;
}

void SearchQueryContainer::append(SearchQuery *qu) {
    if (length == allocated) {
         void *reallocRetValue = realloc(queries, (allocated + reallocSize)*sizeof(SearchQuery*));
        if (reallocRetValue == NULL) {
            delete[] queries;
            queries = NULL;
            coreLog.error(QString("Unable to perform realloc() at %1:%2").arg(__FILE__).arg(__LINE__));
            return;
        } else {
            queries = (SearchQuery **)reallocRetValue;
        }
        allocated += reallocSize;
    }

    queries[length] = qu;
    length++;
}

void SearchQueryContainer::clear() {

}

quint64 SearchQueryContainer::size() const {
    return length;
}

SearchQuery *SearchQueryContainer::at(quint64 pos) {
    assert(pos <= size());
    return queries[pos];
}

} //U2
