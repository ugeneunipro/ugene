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

#ifndef _U2_FINDALGORITHM_H_
#define _U2_FINDALGORITHM_H_

#include <U2Core/U2Region.h>
#include <U2Core/AnnotationData.h>
#include <U2Core/U2SafePoints.h>

#include <QList>

namespace U2 {

class DNAAlphabet;

class U2ALGORITHM_EXPORT FindAlgorithmResult {
public:
    static const int NOT_ENOUGH_MEMORY_ERROR;

public:
    FindAlgorithmResult() : translation(false), err(0) {}
    FindAlgorithmResult(const int _err)
        : translation(false), err(_err){}
    FindAlgorithmResult(const U2Region& _r, bool t, U2Strand s, int _err)
        : region(_r), translation(t), strand(s), err(_err){}

    void clear();

    bool isEmpty() const {
        return region.startPos == 0 && region.length == 0;
    }

    bool operator ==(const FindAlgorithmResult& o) const {
        return region == o.region && err == o.err && strand == o.strand && translation == o.translation;
    }

    SharedAnnotationData toAnnotation(const QString &name, bool splitCircular = false, int seqLen = -1) const;

    static bool lessByRegionStartPos(const FindAlgorithmResult &r1, const FindAlgorithmResult &r2);

    U2Region    region;
    bool        translation;
    U2Strand    strand;
    int         err;

    static QList<SharedAnnotationData> toTable(const QList<FindAlgorithmResult> &res, const QString &name, bool splitCircular = false, int seqLen = -1);
};

class DNATranslation;

class U2ALGORITHM_EXPORT FindAlgorithmResultsListener {
public:
    virtual ~FindAlgorithmResultsListener(){}
    virtual void onResult(const FindAlgorithmResult& r) = 0;
};

enum FindAlgorithmStrand {
    FindAlgorithmStrand_Both,
    FindAlgorithmStrand_Direct,
    FindAlgorithmStrand_Complement
};

enum FindAlgorithmPatternSettings {
    FindAlgorithmPatternSettings_InsDel,
    FindAlgorithmPatternSettings_Subst,
    FindAlgorithmPatternSettings_RegExp,
    FindAlgorithmPatternSettings_Exact
};

class U2ALGORITHM_EXPORT FindAlgorithmSettings {
public:
    FindAlgorithmSettings(const QByteArray& pattern = QByteArray(),
        FindAlgorithmStrand strand = FindAlgorithmStrand_Direct,
        DNATranslation* complementTT = NULL,
        DNATranslation* proteinTT = NULL,
        const DNAAlphabet* sequenceAlphabet = NULL,
        const U2Region& searchRegion = U2Region(),
        int maxErr = 0,
        FindAlgorithmPatternSettings _patternSettings = FindAlgorithmPatternSettings_Subst,
        bool ambBases = false,
        int _maxRegExpResult = 100,
        int _maxResult2Find = 5000);

    QByteArray                          pattern;
    FindAlgorithmStrand                 strand;
    DNATranslation*                     complementTT;
    DNATranslation*                     proteinTT;
    /** Alphabet of the sequence. May be NULL if unknown */
    const DNAAlphabet*                  sequenceAlphabet;
    U2Region                            searchRegion;
    int                                 maxErr;
    FindAlgorithmPatternSettings        patternSettings;
    bool                                useAmbiguousBases;
    int                                 maxRegExpResultLength;
    int                                 maxResult2Find;

    static const int                    MAX_RESULT_TO_FIND_UNLIMITED = -1;
};

class U2ALGORITHM_EXPORT FindAlgorithm {
public:
    // Note: pattern is never affected by either aminoTT or complTT

    static void find(
        FindAlgorithmResultsListener* rl,
        DNATranslation* aminoTT, // if aminoTT!=NULL -> pattern must contain amino data and sequence must contain DNA data
        DNATranslation* complTT, // if complTT!=NULL -> sequence is complemented before comparison with pattern
        FindAlgorithmStrand strand, // if not direct there complTT must not be NULL
        FindAlgorithmPatternSettings patternSettings,
        bool supportAmbigiousBases,
        const char* sequence,
        int seqLen,
        const DNAAlphabet* sequenceAlphabet, // Sequence alphabet. May be NULL if not known.
        bool searchIsCircular,
        const U2Region& range,
        const char* pattern,
        int patternLen,
        int maxErr,
        int maxRegExpResult,
        int& stopFlag,
        int& percentsCompleted);

    static void find(
        FindAlgorithmResultsListener* rl,
        const FindAlgorithmSettings& config,
        const char* sequence,
        int seqLen,
        bool searchIsCircular,
        int& stopFlag,
        int& percentsCompleted) {
            find(rl,
                config.proteinTT,
                config.complementTT,
                config.strand,
                config.patternSettings,
                config.useAmbiguousBases,
                sequence,
                seqLen,
                config.sequenceAlphabet,
                searchIsCircular,
                config.searchRegion,
                config.pattern.constData(),
                config.pattern.length(),
                config.maxErr,
                config.maxRegExpResultLength,
                stopFlag,
                percentsCompleted);
    }

    static int estimateRamUsageInMbytes(const FindAlgorithmPatternSettings patternSettings,
        const bool searchInAminoTT, const int patternLength, const int maxError);

    static bool cmpAmbiguous( char a, char b);
};


}//namespace

#endif
