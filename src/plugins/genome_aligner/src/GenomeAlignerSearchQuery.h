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

#ifndef _U2_GENOME_ALIGNER_SEARCH_QUERY_H_
#define _U2_GENOME_ALIGNER_SEARCH_QUERY_H_

#include <QVector>

#include <U2Core/DNASequence.h>
#include <U2Core/U2AssemblyUtils.h>
#include "GenomeAlignerIndexPart.h"

#define BinarySearchResult qint64

namespace U2 {

class GenomeAlignerIndex;

class CacheResult {
public:
    int posAtShortRead;
    int numberOfPart;
    quint64 bitValue;
};

class SearchQuery {
    Q_DISABLE_COPY(SearchQuery)
public:
    SearchQuery(const DNASequence *shortRead, SearchQuery *revCompl = NULL);
    SearchQuery(const U2AssemblyRead &shortRead, SearchQuery *revCompl = NULL);
    ~SearchQuery();

    QString getName() const;
    int length() const;
    int getNameLength() const;
    char *data();
    const char *constData() const;
    const QByteArray constSequence() const;
    bool hasQuality() const;
    const DNAQuality &getQuality() const;

    bool haveResult() const;
    bool haveMCount() const;
    void addResult(SAType result, quint32 mCount);
    void addOveplapResult(SAType result);
    void onPartChanged();
    void clear();
    SAType firstResult() const;
    quint32 firstMCount() const;
    bool contains(SAType result) const;
    const QVector<SAType>& getResults() const;
    bool isWroteResult() const { return wroteResult; }
    void writeResult() { wroteResult = true; }
    SearchQuery *getRevCompl() { return revCompl; }
    void setRevCompl(SearchQuery *rc) { revCompl = rc; }

    qint64 memoryHint() const;
private:
    //U2AssemblyRead assRead;
    DNAQuality *quality;
    char *seq;
    char *name;
    quint32 seqLength;
    quint32 nameLength;
    bool dna;
    bool wroteResult;
    SearchQuery *revCompl;

    QVector<SAType> results;
    QVector<SAType> overlapResults;
    QVector<quint32> mismatchCounts;
};

class SearchQueryContainer {
public:
    SearchQueryContainer();
    ~SearchQueryContainer();
    void append(SearchQuery *qu);
    void clear();
    quint64 size() const;
    SearchQuery *at(quint64 pos);

private:
    SearchQuery **queries;
    quint64 length;
    quint64 allocated;

    static const quint64 reallocSize;
};

} //U2
#endif //_U2_GENOME_ALIGNER_SEARCH_QUERY_H_
