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

#include "SiteconAlgorithm.h"
#include "DIPropertiesSitecon.h"

#include "SiteconMath.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/Log.h>
#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Core/TextUtils.h>
#include <QFile>

#include <math.h>

namespace U2 {

bool SiteconModel::operator !=(const SiteconModel& model) const {
    bool eq = true;
    if(matrix.size() != model.matrix.size()) {
        eq = false;
    }
    for(int i = 0; i<matrix.size() && eq;i++) {
        PositionStats pos = matrix[i];
        if(pos.size() != model.matrix[i].size()) {
            eq = false;
        }
        for(int j = 0;j < pos.size() && eq;j++) {
            DiStat ds1 = pos[j];
            DiStat ds2 = model.matrix[i][j];
            for(int k = 0; k < 16 && eq; k++) {
                eq = ds1.prop->original[k] == ds2.prop->original[k] && ds1.prop->normalized[k] == ds2.prop->normalized[k];
            }
            eq = eq && ds1.average == ds2.average && ds1.sdeviation == ds2.sdeviation;
            eq = eq && ds1.weighted == ds2.weighted && ds1.prop->average == ds2.prop->average && ds1.prop->sdeviation == ds2.prop->sdeviation;
            eq = eq && ds1.prop->keys == ds2.prop->keys;
        }
    }
    for(int i = 0; i < err1.size() && eq; i++) {
        eq = err1[i] == model.err1[i] && err2[i] == model.err2[i];
    }
    return !eq;
}

QVector<PositionStats> SiteconAlgorithm::calculateDispersionAndAverage(const MultipleSequenceAlignment& ma, const SiteconBuildSettings& config, TaskStateInfo& ts) 
{
    const QList<DiPropertySitecon*>& props = config.props;
    assert(!props.isEmpty());
    QVector<PositionStats> matrix;
    int N = ma->getNumRows();
    for (int i = 0, n = ma->getLength()-1; i < n && !ts.cancelFlag; i++) { //for every di-nucl
        PositionStats posResult;
        foreach(DiPropertySitecon* p, props) { // for every property
            float average = 0; //average in a column
            foreach(const MultipleSequenceAlignmentRow& row, ma->getMsaRows()) { // collect di-position stat for all sequence in alignment
                average+=p->getOriginal(row->charAt(i), row->charAt(i+1));
            }
            average/=N;

            float dispersion = 0; // dispersion in a column
            for (int j = 0; j < ma->getNumRows(); j++) {// collect di-position stat for all sequence in alignment
                const MultipleSequenceAlignmentRow row = ma->getMsaRow(j);
                char c1 = row->charAt(i);
                char c2 = row->charAt(i+1);
                float v = p->getOriginal(c1, c2);
                dispersion+=(average - v)*(average - v);
            }
            dispersion/=(N-1);
            float sdeviation= sqrt(dispersion);

            posResult.append(DiStat(p, sdeviation, average));
        }
        matrix.append(posResult);
    }
    if (ts.cancelFlag || ts.hasError()) {
        matrix.clear();
        return matrix;
    } 
    assert(matrix.size() == ma->getLength() - 1);
    return matrix;
}

float SiteconAlgorithm::calculatePSum(const char* seq, int len, const QVector<PositionStats>& normalizedMatrix, 
                                      const SiteconBuildSettings& config, float devThreshold, DNATranslation* complMap) {
    Q_UNUSED(config);
    assert(len == config.windowSize);
    bool complement = complMap!=NULL;
    QByteArray complMapper = complement ? complMap->getOne2OneMapper() : QByteArray();
    float pSum = 0.0f;
    float sdevDeltasSum = 0.0f;
    for (int i = 0; i < len-1; i++) { 
        char c1 = seq[i];
        char c2 = seq[i+1];
        if (complement) {
            char origC2 = c2; c2 = complMapper[uchar(c1)]; c1 = complMapper[uchar(origC2)];
        }
        const PositionStats& posProps = complement ? normalizedMatrix[len-1 - 1 - i] : normalizedMatrix[i];
        assert(posProps.size() == config.props.size());
        for (int k=0, nprops = posProps.size(); k < nprops; k++) {
            const DiStat& ds = posProps[k];
            if (ds.sdeviation < devThreshold && ds.weighted) {
                sdevDeltasSum+= 1 / (ds.sdeviation + 0.1f);

                if (c1 == 'N' || c2=='N')  {
                    continue;
                }

                float f = ds.prop->getNormalized(c1, c2);
                float expPart = (ds.average - f) / (ds.sdeviation + 0.1f);
                float p = exp((-1) * expPart * expPart) / (ds.sdeviation + 0.1f);
                pSum+=p;
            }
        }
    }

    if (sdevDeltasSum == 0.0f) {
        assert(pSum == 0.0f);
        return 0;
    }
    pSum /= sdevDeltasSum;
    return pSum;
}



QVector<float> SiteconAlgorithm::calculateFirstTypeError(const MultipleSequenceAlignment& ma, const SiteconBuildSettings& s, TaskStateInfo& ts) {
    float devThresh = critchi(s.chisquare, s.numSequencesInAlignment - 2) / (s.numSequencesInAlignment - 1);

    QVector<float> scores;
    // 1. Exclude 1 sequence for MSA and create Dis/Ave matrix on whats left in MSA
    // 2. Calculate score for excluded sequence
    // 3. Distribute percentage for all scores

    U2OpStatus2Log os;
    int maLen = ma->getLength();
    for (int i=0; i < ma->getNumRows() && !ts.cancelFlag; i++) {
        const MultipleSequenceAlignmentRow row = ma->getMsaRow(i);
        MultipleSequenceAlignment subMA = ma->getCopy();
        subMA->removeRow(i, os);
        QVector<PositionStats> matrix = calculateDispersionAndAverage(subMA, s, ts);
        QVector<PositionStats> normalizedMatrix = normalize(matrix, s);
        calculateWeights(subMA, normalizedMatrix, s, true, ts);
        float p = calculatePSum(row->toByteArray(os, maLen), maLen, normalizedMatrix, s, devThresh);
        scores.append(p);
    }
    QVector<float> res(100, 0);
    if (ts.cancelFlag) {
        return res;
    }
    for (int i = 0; i < 100; i++) {
        int numScoresLowerThanI = 0;
        foreach(float score, scores)  {
            if (score * 100 < i) {
                numScoresLowerThanI++;
            }
        }
        res[i] = numScoresLowerThanI / float(scores.size());
    }
    return res;
}


QVector<float> SiteconAlgorithm::calculateSecondTypeError(const QVector<PositionStats>& matrix, 
                                                          const SiteconBuildSettings& settings, 
                                                          TaskStateInfo& si) {
    float devThresh = (float)critchi(settings.chisquare, settings.numSequencesInAlignment - 1) / settings.numSequencesInAlignment;
    
    qsrand(settings.randomSeed);
    QByteArray randomSequence = generateRandomSequence(settings.acgtContent, settings.secondTypeErrorCalibrationLen, si);
    
    int dProgress = 100-si.progress;
    int nuclsPerProgress = randomSequence.size() / dProgress;
    int progressI = nuclsPerProgress;

    QVector<PositionStats> normalizedMatrix = normalize(matrix, settings);
    QVector<int> hitsPerScore(100, 0);
    const char* seq = randomSequence.constData();
    for (int i = 0; i < randomSequence.size() - (settings.windowSize-1) && !si.cancelFlag; i++) {
        const char* subseq = seq + i;
        float psum = calculatePSum(subseq, settings.windowSize, normalizedMatrix, settings, devThresh);
        hitsPerScore[qRound(psum * 100)]++;
        if (--progressI == 0) {
            progressI = nuclsPerProgress;
            si.progress++;
        }
    }

    QVector<float> errorPerScore(100, 0);
    int totalHits = 0;
    for (int i=100; --i>=0;) {
        totalHits += hitsPerScore[i];
        float err = totalHits / (float) (settings.secondTypeErrorCalibrationLen - settings.windowSize + 1);
        assert(err <= 1);
        errorPerScore[i] = err;
    }
    return errorPerScore;
}


QVector<PositionStats> SiteconAlgorithm::normalize(const QVector<PositionStats>& matrix, const SiteconBuildSettings& settings) {
    Q_UNUSED(settings); //TODO: remove this arg

    //calculate scale average and deviation
    //normalize initial model by scale: 
    //    model_ave = (model_ave - scale_ave) / scale_dev
    //    model_dev =  model_dev / scale_dev

    //normalize initial matrix;
    QVector<PositionStats> normMatrix;
    for (int i=0, n = matrix.size(); i < n; i++) {
        const PositionStats& list = matrix[i];
        PositionStats normList;
        for (int j = 0, m = list.size(); j < m; j++) {
            const DiStat& dsModel = list[j];
            DiStat normDS;
            normDS.prop = dsModel.prop;
            normDS.average = (dsModel.average - dsModel.prop->average) / (dsModel.prop->sdeviation);
            normDS.sdeviation = dsModel.sdeviation / (dsModel.prop->sdeviation);
            normDS.weighted = dsModel.weighted;
            normList.append(normDS);
        }
        normMatrix.append(normList);
    }
    return normMatrix;
}


void SiteconAlgorithm::calculateACGTContent(const MultipleSequenceAlignment& ma, SiteconBuildSettings& bs) {
    assert(ma->getAlphabet()->isNucleic());
    bs.acgtContent[0] = bs.acgtContent[1] = bs.acgtContent[2] = bs.acgtContent[3] = 0;
    int maLen = ma->getLength();
    int total = ma->getNumRows() * ma->getLength();
    foreach(const MultipleSequenceAlignmentRow& row, ma->getMsaRows()) {
		for (int i=0; i < maLen; i++) {
            char c = row->charAt(i);
            if (c == 'A') {
                bs.acgtContent[0]++;
            } else if (c == 'C') {
                bs.acgtContent[1]++;
            } else if (c == 'G') {
                bs.acgtContent[2]++;
            } else if (c == 'T') {
                bs.acgtContent[3]++;
            } else {
                total--;
            }
        }
    }
    for (int i=0;i<4;i++) {
        bs.acgtContent[i] = qRound(bs.acgtContent[i] * 100. / total);
    }
}


QByteArray SiteconAlgorithm::generateRandomSequence(const int* acgtContent, int seqLen, TaskStateInfo&)  {
    QByteArray randomSequence;
    randomSequence.reserve(seqLen);

    int aPercentRange = acgtContent[0];
    int cPercentRange = aPercentRange + acgtContent[1];
    int gPercentRange = cPercentRange + acgtContent[2];

    assert(gPercentRange + acgtContent[3] > 0);

    for (int i=0; i < seqLen; i++) {
        int r = qrand();
        float perc = 100 * (float(r) / RAND_MAX);
        char c = 'T';
        if (perc <= aPercentRange) {
            c = 'A';
        } else if (perc <= cPercentRange) {
            c = 'C';
        } else if (perc <= gPercentRange) {
            c = 'G';
        }
        randomSequence.append(c);
    }
    return randomSequence;
}


#define MAX_WEIGHTS_ALG2 6

typedef QVector< QVector<float> > PWVector;

//#define _SITECON_ALG_DEBUG_

#ifdef _SITECON_ALG_DEBUG_

static void dumpWeights(const QString& url, const PWVector& weights, const SiteconBuildSettings& settings) {
    QVector<QByteArray> wght(settings.props.size());
    for(int j = 0; j < settings.props.size(); j++) {
        const DiPropertySitecon* p = settings.props[j];
        QString name = p->keys.value("PV");
        QString index = p->keys.value("MI");
        wght[j].append(index).append(";").append(name);
    }

    wght.resize(settings.props.size());
    for (int i=0; i < weights.size(); i++) {
        const QVector<float>& posWeights = weights[i];
        for(int j = 0; j < settings.props.size(); j++) {
            float w = posWeights[j];
            const char* ali = TextUtils::getLineOfSpaces(wght[j].length() < 25 ? 25 - wght[j].length() : ( 2 - wght[j].length()%2));
            wght[j].append(";").append(ali).append(QString::number(w, 'f', 2));
        }
    }

    QFile file(url);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    foreach(const QByteArray& w, wght) {
        file.write(w);
        file.write("\n");
    }
    file.close();
}
#endif

int SiteconAlgorithm::calculateWeights(const MultipleSequenceAlignment& ma, QVector<PositionStats>& origMatrix, 
                                       const SiteconBuildSettings& settings, bool matrixIsNormalized,
                                       TaskStateInfo& si) 
{
    int   modelSize = settings.windowSize - 1;
    if (settings.weightAlg == SiteconWeightAlg_None) {
        for(int i=0; i < modelSize; i++) {
            PositionStats& ps = origMatrix[i];
            for (int j = 0; j < ps.size(); j++) {
                DiStat& ds = ps[j];
                ds.weighted = true;
            }
        }
        return settings.props.size();
    }

    assert(settings.weightAlg == SiteconWeightAlg_Alg2);

    //to calculate weights by algorithm2
    //1. generate ~modelLen*seqNums + 10 sequence with ACGT content == model content and 
    //1  for every prop calculate average weight
    //2. for every pos select max weight per props only for di-nucls in model
    //3. calculate diff = W2_max - W1_ave
    //4. mark up to 6 props per pos as weighted with max-diffs < chisquare

    assert(ma->getLength() == settings.windowSize);
    assert(origMatrix.size() == settings.windowSize - 1);

    //clear weights data
    for (int i=0; i < origMatrix.size(); i++) {
        PositionStats& ps = origMatrix[i];
        for (int j=0; j < ps.size(); j++) {
            ps[j].weighted = false;
        }
    }

    //normalize matrix if needed
    QVector<PositionStats> normMatrix = origMatrix;
    if (!matrixIsNormalized) {
        normMatrix = normalize(origMatrix, settings);
    }
    
    float devThreshold = (float)critchi(settings.chisquare, settings.numSequencesInAlignment - 1) / settings.numSequencesInAlignment;

    //Part1
    //1. compute props ave on random sequence
    int rndSeqLen = modelSize * ma->getNumRows() + 10;
    QByteArray rndSeqArray = generateRandomSequence(settings.acgtContent, rndSeqLen, si);
    const char* rndSeq = rndSeqArray.constData();
    
    //init weights with default val
    PWVector aveWeightMatrix(modelSize);
    PWVector maxWeightMatrix(modelSize);
    for(int i=0; i < modelSize; i++) {
        aveWeightMatrix[i].fill(0, settings.props.size());
        maxWeightMatrix[i].fill(0, settings.props.size());
    }
    
    //sum all psums for nSamples and create average vals per (pos, prop)
    int nSamples = rndSeqLen - modelSize;
    for (int i=0 ; i < nSamples && !si.cancelFlag; i++) {
        for (int pos = 0; pos < modelSize; pos++) {
            char c1 = rndSeq[i + pos];
            char c2 = rndSeq[i + pos + 1];
            if (c1 == 'N' || c2 == 'N')  {
                continue;
            }
            QVector<float>& posWeights = aveWeightMatrix[pos];
            PositionStats& ps = normMatrix[pos];
            for (int j = 0; j < ps.size(); j++) {
                const DiStat& ds = ps[j];
                if (ds.sdeviation < devThreshold) {
                    float f = ds.prop->getNormalized(c1, c2);
                    float expPart = (ds.average - f) / (ds.sdeviation + 0.1f);
                    float dinuclWeight = exp((-1) * expPart * expPart) / (ds.sdeviation + 0.1f);
                    posWeights[j]+=dinuclWeight;
                }
            }
        }
    }
    //normalize psums by nSamples
    for(int i=0; i < modelSize; i++) {
        QVector<float>& posWeights = aveWeightMatrix[i];
        for(int j=0; j < posWeights.size(); j++) {
            posWeights[j] /= nSamples;
        }
    }
#ifdef _SITECON_ALG_DEBUG_
    dumpWeights("mave.txt", aveWeightMatrix, settings);
#endif    
    if (si.cancelFlag) {
        return 0;
    }

    //Part2
    //2. compute max weights per props for model
    for(int i=0; i < modelSize && !si.cancelFlag; i++) {
        PositionStats& ps = normMatrix[i];
        QVector<float>& posWeights = maxWeightMatrix[i];
        QVector<float>& aveWeights = aveWeightMatrix[i];
        for (int j = 0; j < ps.size(); j++) {
            const DiStat& ds = ps[j];
            float maxProp = 100;
            if (ds.sdeviation < devThreshold) {
                for(int k = 0; k < ma->getNumRows(); k++) {
                    char c1 = ma->charAt(k, i);
                    char c2 = ma->charAt(k, i+1);

                    if (c1 == 'N' || c2=='N')  {
                        continue;
                    }

                    float f = ds.prop->getNormalized(c1, c2);
                    float expPart = (ds.average - f) / (ds.sdeviation + 0.1f);
                    float dinuclWeight = exp((-1) * expPart * expPart) / (ds.sdeviation + 0.1f);
                    maxProp = qMin(maxProp, dinuclWeight); // qMin is used instead of qMax for compatibility with original Sitecon
                }
            } else {
                maxProp = aveWeights[j]; //actually any value is OK -> it will be filtered from weight estimation algorithm
            }
            posWeights[j] = maxProp;
        }
    }

#ifdef _SITECON_ALG_DEBUG_
    dumpWeights("mmax.txt", maxWeightMatrix, settings);
#endif    

    //select MAX_WEIGHTS properties
    for(int i=0; i < modelSize; i++) {
        QVector<float>& avePosWeights = aveWeightMatrix[i];
        QVector<float>& maxPosWeights = maxWeightMatrix[i];
        assert(avePosWeights.size() == maxPosWeights.size());
        QVector<float> diffs;
        for (int j = 0; j < maxPosWeights.size(); j++) {
            float avePropWeight = avePosWeights[j];
            float maxPropWeight = maxPosWeights[j];
            float diff = maxPropWeight - avePropWeight;
            diffs.append(diff);
        }
        
        PositionStats& psNorm = normMatrix[i];
        PositionStats& psOrig = origMatrix[i];
        for (int x = 0; x < MAX_WEIGHTS_ALG2; x++) {
            float maxVal = -110;
            for (int j = 0; j < psNorm.size(); j++) {
                float val = diffs[j];
                const DiStat& ds = psNorm[j];
                if (val > maxVal && ds.sdeviation < devThreshold) {
                    maxVal = val;
                }
            }
            //int nWeightedPerStep = 0;
            for (int j = 0; j < psNorm.size(); j++) {
                float val = diffs[j];
                const DiStat& ds = psNorm[j];
                if (val == maxVal && ds.sdeviation < devThreshold){
                    diffs[j] = -100;
                    psNorm[j].weighted = true;
                    psOrig[j].weighted = true;
                    //nWeightedPerStep++;
                }
            }
            //assert(nWeightedPerStep <= 1);
        }
    }

#ifdef _DEBUG
    /*for(int i=0; i < modelSize; i++) {
        int nWeights = 0;
        const PositionStats& ps = origMatrix[i];
        for (int j = 0; j < ps.size(); j++) {
            const DiStat& ds = ps[j];
            nWeights+=ds.weighted ? 1 : 0;
        }
        assert(nWeights <= MAX_WEIGHTS_ALG2);
    }*/
#endif

    return MAX_WEIGHTS_ALG2;
}


bool SiteconModel::checkState(bool doAssert) const {
    //1 check  settings
    assert(!doAssert || settings.windowSize > 0); Q_UNUSED(doAssert);
    if (settings.windowSize <=0) {
        return false;
    }
    assert(!doAssert || settings.secondTypeErrorCalibrationLen > settings.windowSize);
    if (settings.secondTypeErrorCalibrationLen <= settings.windowSize) {
        return false;
    }
    assert(!doAssert || (settings.chisquare > 0 && settings.chisquare < 1));
    if (settings.chisquare <=0 || settings.chisquare >=1) {
        return false;
    }
    assert(!doAssert || settings.numSequencesInAlignment > 1);
    if (settings.numSequencesInAlignment <= 1) {
        return false;
    }
    
    //2 check model
    assert(!doAssert || matrix.size() == settings.windowSize -1);
    if (matrix.size() != settings.windowSize-1) {
        return false;
    }
    for (int pos = 0; pos < matrix.size(); pos++) {
        const PositionStats& posStat =  matrix[pos];
        assert(!doAssert || posStat.size() == settings.props.size());
        int nWeights = 0;
        for(int i=0; i<posStat.size(); i++) {
            const DiStat& ds = posStat[i];
            if (ds.weighted) {
                nWeights++;
            }
            const DiPropertySitecon* p = settings.props[i];
            assert(!doAssert || ds.prop == p);
            if (ds.prop!=p) {
                return false;
            }
        }
        
        if (settings.weightAlg == SiteconWeightAlg_None) {
            assert(!doAssert || nWeights == settings.props.size());
            if (nWeights != settings.props.size()) {
                return false;
            }
        } else {
            //assert(!doAssert || nWeights <= MAX_WEIGHTS_ALG2);
            if (nWeights > MAX_WEIGHTS_ALG2) {
                algoLog.trace(QString("Number of Algorithm 2 weights %1, pos %2, model name %3").arg(nWeights).arg(pos).arg(modelName));
                //return false;
            }
        }
    }
    for (int i=0;i<100;i++) {
        assert(!doAssert || (err1[i] >= 0 && err1[i] <= 1));
        if (err1[i] < 0 && err1[i] > 1) {
            return false;
        }
        assert(!doAssert || (err2[i] >= 0 && err2[i] <= 1));
        if (err2[i] < 0 && err2[i] > 1) {
            return false;
        }
    }
    return true;
}

} //namespace
