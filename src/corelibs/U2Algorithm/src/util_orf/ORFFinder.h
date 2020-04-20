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

#ifndef _U2_ORFFINDALGORITHM_H_
#define _U2_ORFFINDALGORITHM_H_

#include <U2Core/U2Region.h>
#include <U2Core/AnnotationData.h>
#include <U2Core/DNASequenceObject.h>

#include <QList>

namespace U2 {

class DNATranslation;
class TaskStateInfo;

class U2ALGORITHM_EXPORT ORFFindResult {
public:
    ORFFindResult () : region(0, 0), frame(0), isJoined(false){}
    ORFFindResult (const U2Region& _r, int frame)
        : region(_r), frame(frame), isJoined(false){}
    ORFFindResult (const U2Region& _r, const U2Region& _r_joined, int frame)
        :region(_r), joinedRegion(_r_joined), frame(frame), isJoined(true){}

    void clear() {region.startPos = 0; region.length = 0; frame = 0;}

    bool isEmpty() const {return region.startPos == 0 && region.length == 0;}

    bool operator ==(const ORFFindResult& o) const {
        return region == o.region && frame == o.frame;
    }

    SharedAnnotationData toAnnotation(const QString &name) const {
        SharedAnnotationData data(new AnnotationData);
        data->name = name;
        data->location->regions << region;
        int regLen = region.length;
        if(isJoined){
            data->location->regions << joinedRegion;
            regLen += joinedRegion.length;
        }
        data->setStrand(frame < 0 ? U2Strand::Complementary : U2Strand::Direct);
        data->qualifiers.append(U2Qualifier("dna_len", QString::number(regLen)));
        if (regLen >= 6) { // 3 bp - end codon
            data->qualifiers.append(U2Qualifier("protein_len", QString::number(regLen/3)));
        }
        return data;
    }

    U2Region region;
    U2Region joinedRegion;
    int frame;
    bool isJoined;

    static QList<SharedAnnotationData> toTable(const QList<ORFFindResult>& res, const QString& name) {
        QList<SharedAnnotationData> list;
        foreach (const ORFFindResult &f, res) {
            list << f.toAnnotation(name);
        }
        return list;
    }
};

class U2ALGORITHM_EXPORT ORFFindResultsListener {
public:
    virtual ~ORFFindResultsListener(){}
    virtual void onResult(const ORFFindResult& r, U2OpStatus& os) = 0;
};

enum ORFAlgorithmStrand {
    ORFAlgorithmStrand_Both,
    ORFAlgorithmStrand_Direct,
    ORFAlgorithmStrand_Complement
};

class U2ALGORITHM_EXPORT ORFAlgorithmSettings {
public:
    ORFAlgorithmSettings(ORFAlgorithmStrand strand = ORFAlgorithmStrand_Both,
                         DNATranslation* complementTT = NULL,
                         DNATranslation* proteinTT = NULL,
                         const U2Region& searchRegion = U2Region(),
                         int minLen = 0,
                         bool mustFit = false,
                         bool mustInit = true,
                         bool allowAltStart = false,
                         bool allowOverlap = false,
                         bool includeStopCodon = false,
                         bool circularSearch = false)
        : strand(strand),
          complementTT(complementTT),
          proteinTT(proteinTT),
          searchRegion(searchRegion),
          minLen(minLen),
          mustFit(mustFit),
          mustInit(mustInit),
          allowAltStart(allowAltStart),
          allowOverlap(allowOverlap),
          includeStopCodon(includeStopCodon),
          circularSearch(circularSearch),
          maxResult2Search(0),
          isResultsLimited(false)
    {

    }

    ORFAlgorithmStrand          strand;
    DNATranslation*             complementTT;
    DNATranslation*             proteinTT;
    U2Region                    searchRegion;
    int                         minLen;
    bool                        mustFit;
    bool                        mustInit;
    bool                        allowAltStart;
    bool                        allowOverlap;
    bool                        includeStopCodon;
    bool                        circularSearch;
    int                         maxResult2Search;
    bool                        isResultsLimited;
    static const QString        ANNOTATION_GROUP_NAME;
    // strand string ids
    static const QString        STRAND_DIRECT;
    static const QString        STRAND_COMPL;
    static const QString        STRAND_BOTH;
    static QString              getStrandStringId(ORFAlgorithmStrand strand);
    static ORFAlgorithmStrand   getStrandByStringId(const QString& id);
};


class U2ALGORITHM_EXPORT ORFFindAlgorithm {
public:
    static void find(
        ORFFindResultsListener* rl,
        const ORFAlgorithmSettings& config,
        U2EntityRef& entityRef,
        int& stopFlag,
        int& percentsCompleted);
private:
    static void addStartCodonsFromJunction(const U2SequenceObject &seq,
                                           const ORFAlgorithmSettings &cfg,
                                           ORFAlgorithmStrand strand,
                                           QList<int> *start);
    static void checkStopCodonOnJunction(const U2SequenceObject &dnaSeq,
                                         const ORFAlgorithmSettings &cfg,
                                         ORFAlgorithmStrand strand,
                                         ORFFindResultsListener* rl,
                                         QList<int> *start,
                                         TaskStateInfo &os);
    static char* getCodonFromJunction(const U2SequenceObject &dnaSeq,
                                      ORFAlgorithmStrand strand,
                                       // points to number of characters that are located in the end of the sequence
                                      int symbolsFromEnd);
};

}//namespace

#endif
