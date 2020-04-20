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
#ifndef _U2_VARIATION_PROPERTIES_UTILS_H_
#define _U2_VARIATION_PROPERTIES_UTILS_H_

#include <U2Core/U2Variant.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2FeatureDbi.h>
#include <U2Core/DNATranslation.h>

#include <QByteArray>
#include <QVector>

namespace U2 {


//////////////////////////////////////////////////////////////////////////
//SequenceQueryCache
/** a simple cache for fast access sequence data for variations
assuming that variations are sorted by start pos*/
class U2CORE_EXPORT SequenceQueryCache {
public:
    SequenceQueryCache(U2SequenceDbi* seqDbi, const U2DataId& seqDataId);
    void setSequenceDbi(U2SequenceDbi* sDbi);
    void setSequenceId(const U2DataId& seqDataId);
    void clear();
    U2Region getRegion() { return seqRegion; }
    QByteArray getSequenceData(const U2Region& region, U2OpStatus& os);

private:
    void fetchData(int startPos, U2OpStatus& os);
    const static int OVERLAP_SIZE = 1024;
    const static int SEQ_BUF_SIZE = 1 * 256 * 1024;
    U2Region seqRegion;
    QByteArray seqBuffer;
    U2SequenceDbi* seqDbi;
    U2DataId seqId;
};



/*Class for managing variation properties*/
class U2CORE_EXPORT VariationPropertiesUtils {
public:

    /*Convert damage effect matrix to compressed array*/
    static QByteArray DamageEffectToArray(const QVector<float>& vect);

    /*Convert damage effect compressed array to matrix*/
    static QVector<float> DamageEffectToVector(const QByteArray& ba);

    /*Check if a variation could damage protein of the gene*/
    static bool isDamageProtein(const U2Variant& var, const Gene& gene);

    /*The function is 'in silico' splicing. It provides start position of Variant in nucl sequence and corresponding codon number after splicing.
    Return true if spliced successfully and false otherwise*/
    static bool getFrameStartPositionsForCoding(int* nuclSeqPos, int* aaSeqPos, int* codonPos, const U2Variant& var, const Gene& gene);

    static int getFrameStartPosition(const U2Variant& var, const Gene& gene);

    static QByteArray getSortedAAcidAlphabet();

    static QByteArray getDamagedTriplet(const Gene& gene, int nuclPos, const U2DataId& seqID, U2SequenceDbi* seqDbi, U2OpStatus& os);

    static QByteArray getDamagedTripletBufferedSeq(const Gene& gene, int nuclPos, SequenceQueryCache& seqCache, U2OpStatus& os);

    static float getDEffectValue(char aa, int aaPos, const QByteArray& compressedMatrix, int matrixLen);

    static QByteArray varyTriplet(const QByteArray& tripl, const U2Variant& var, int codonPos, DNATranslation* complTransl = NULL);

    static QByteArray getCodingSequence(const Gene& gene, const U2DataId& seqId, U2SequenceDbi* dbi, U2OpStatus& os);

    static QByteArray getAASequence(const QByteArray& nuclSeq);

    static QPair< QByteArray, QByteArray> getAASubstitution(U2Dbi* database, const Gene& gene, const U2DataId& seqId, const U2Variant& var, int* aaPos, U2OpStatus& os);

    static inline bool isInDonorSpliceSite(const U2Region& exon, qint64 varPos, int spliceSiteLen) {
        bool res = false;
        res = U2Region(exon.endPos() - spliceSiteLen, 2 * spliceSiteLen).contains(varPos);
        return res;
    }
    static inline bool isInAcceptorSpliceSite(const U2Region& exon, qint64 varPos, int spliceSiteLen) {
        bool res = false;
        res = U2Region(exon.startPos - spliceSiteLen, 2 * spliceSiteLen).contains(varPos);
        return res;
    }

    /**Try to find intron region.
    If varPos intersects a region between two items in the sorted list, returns it.
    otherwise U2_REGION_MAX returned
    if in splice-site nearest exon and is donor/acceptor are set
    else neaarestExon = U2_REGION_MAX*/
    static U2Region getIntron(const U2Region& geneRegion, const QVector<U2Region>& exons, qint64 varPos, U2Region& nearestExon, bool& isDonor);

    static bool isInSpliceSite(const QVector<U2Region>& exons, qint64 varPos, int spliceSiteLen, bool isCompl);

    static qint64 positionFromTranscriptionStart(const Gene& gene, const U2Variant& var);

private:
    static QByteArray aaAlphabet;
};


} //namespace

#endif //_U2_VARIATION_PROPERTIES_UTILS_H_
