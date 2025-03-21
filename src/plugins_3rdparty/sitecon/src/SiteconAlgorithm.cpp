/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <QFile>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/Log.h>
#include <U2Core/Msa.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include "DIPropertiesSitecon.h"
#include "SiteconMath.h"

namespace U2 {

bool SiteconModel::operator!=(const SiteconModel& model) const {
    bool eq = true;
    if (matrix.size() != model.matrix.size()) {
        eq = false;
    }
    for (int i = 0; i < matrix.size() && eq; i++) {
        PositionStats pos = matrix[i];
        if (pos.size() != model.matrix[i].size()) {
            eq = false;
        }
        for (int j = 0; j < pos.size() && eq; j++) {
            DiStat ds1 = pos[j];
            DiStat ds2 = model.matrix[i][j];
            for (int k = 0; k < 16 && eq; k++) {
                eq = ds1.prop->original[k] == ds2.prop->original[k] && ds1.prop->normalized[k] == ds2.prop->normalized[k];
            }
            eq = eq && ds1.average == ds2.average && ds1.sdeviation == ds2.sdeviation;
            eq = eq && ds1.weighted == ds2.weighted && ds1.prop->average == ds2.prop->average && ds1.prop->sdeviation == ds2.prop->sdeviation;
            eq = eq && ds1.prop->keys == ds2.prop->keys;
        }
    }
    for (int i = 0; i < err1.size() && eq; i++) {
        eq = err1[i] == model.err1[i] && err2[i] == model.err2[i];
    }
    return !eq;
}

QVector<PositionStats> SiteconAlgorithm::calculateDispersionAndAverage(const Msa& ma, const SiteconBuildSettings& config, TaskStateInfo& ts) {
    const QList<DiPropertySitecon*>& props = config.props;
    assert(!props.isEmpty());
    QVector<PositionStats> matrix;
    int N = ma->getRowCount();
    for (int i = 0, n = ma->getLength() - 1; i < n; i++) {  // for every di-nucl
        CHECK(!ts.isCoR(), {});
        PositionStats posResult;
        for (DiPropertySitecon* p : qAsConst(props)) {  // for every property
            qreal average = 0;  // average in a column
            foreach (const MsaRow& row, ma->getRows()) {  // collect di-position stat for all sequence in alignment
                average += p->getOriginal(row->charAt(i), row->charAt(i + 1));
            }
            average /= N;

            qreal dispersion = 0;  // dispersion in a column
            const QVector<MsaRow>& msaRows = ma->getRows();
            for (int j = 0; j < msaRows.size(); j++) {  // collect di-position stat for all sequence in alignment
                CHECK(!ts.isCoR(), {});
                const MsaRow& row = msaRows[j];
                char c1 = row->charAt(i);
                char c2 = row->charAt(i + 1);
                qreal v = p->getOriginal(c1, c2);
                dispersion += (average - v) * (average - v);
            }
            dispersion /= (N - 1);
            qreal sdeviation = sqrt(dispersion);
            posResult.append(DiStat(p, sdeviation, average));
        }
        matrix.append(posResult);
    }
    CHECK(!ts.isCoR(), {});
    assert(matrix.size() == ma->getLength() - 1);
    return matrix;
}

qreal SiteconAlgorithm::calculatePSum(const char* seq,
                                      int len,
                                      const QVector<PositionStats>& normalizedMatrix,
                                      const SiteconBuildSettings& config,
                                      qreal devThreshold,
                                      DNATranslation* complMap) {
    SAFE_POINT(len == config.windowSize, "config.windowsSize != len", 0);
    bool complement = complMap != nullptr;
    QByteArray complMapper = complement ? complMap->getOne2OneMapper() : QByteArray();
    qreal pSum = 0.0f;
    qreal sdevDeltasSum = 0.0f;
    for (int i = 0; i < len - 1; i++) {
        char c1 = seq[i];
        char c2 = seq[i + 1];
        if (complement) {
            char origC2 = c2;
            c2 = complMapper[uchar(c1)];
            c1 = complMapper[uchar(origC2)];
        }
        const PositionStats& posProps = complement ? normalizedMatrix[len - 1 - 1 - i] : normalizedMatrix[i];
        assert(posProps.size() == config.props.size());
        for (int k = 0, nprops = posProps.size(); k < nprops; k++) {
            const DiStat& ds = posProps[k];
            if (ds.sdeviation < devThreshold && ds.weighted) {
                sdevDeltasSum += 1.0 / (ds.sdeviation + 0.1);

                if (c1 == 'N' || c2 == 'N') {
                    continue;
                }

                qreal f = ds.prop->getNormalized(c1, c2);
                qreal expPart = (ds.average - f) / (ds.sdeviation + 0.1f);
                qreal p = exp((-1) * expPart * expPart) / (ds.sdeviation + 0.1f);
                pSum += p;
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

QVector<qreal> SiteconAlgorithm::calculateFirstTypeError(const Msa& ma, const SiteconBuildSettings& s, TaskStateInfo& ts) {
    QVector<qreal> res(100, 0);

    qreal devThresh = critchi(s.chisquare, s.numSequencesInAlignment - 2) / (s.numSequencesInAlignment - 1);
    CHECK(!ts.isCoR(), res);

    QVector<qreal> scores;
    // 1. Exclude 1 sequence for MSA and create Dis/Ave matrix on whats left in MSA
    // 2. Calculate score for excluded sequence
    // 3. Distribute percentage for all scores

    U2OpStatus2Log os;
    int maLen = ma->getLength();
    for (int i = 0; i < ma->getRowCount(); i++) {
        CHECK(!ts.isCoR(), res);
        const MsaRow& row = ma->getRow(i);
        Msa subMA = ma->getCopy();
        subMA->removeRow(i, os);
        CHECK(!ts.isCoR(), res);
        QVector<PositionStats> matrix = calculateDispersionAndAverage(subMA, s, ts);
        CHECK(!ts.isCoR(), res);
        QVector<PositionStats> normalizedMatrix = normalize(matrix);
        CHECK(!ts.isCoR(), res);
        calculateWeights(subMA, normalizedMatrix, s, true, ts);
        CHECK(!ts.isCoR(), res);
        qreal p = calculatePSum(row->toByteArray(os, maLen), maLen, normalizedMatrix, s, devThresh);
        scores.append(p);
    }
    CHECK(!ts.isCoR(), res);
    for (int i = 0; i < 100; i++) {
        int numScoresLowerThanI = 0;
        for (qreal score : qAsConst(scores)) {
            if (score * 100 < i) {
                numScoresLowerThanI++;
            }
        }
        CHECK(!ts.isCoR(), res);
        res[i] = numScoresLowerThanI / qreal(scores.size());
    }
    return res;
}

QVector<qreal> SiteconAlgorithm::calculateSecondTypeError(const QVector<PositionStats>& matrix,
                                                          const SiteconBuildSettings& settings,
                                                          TaskStateInfo& si) {
    QVector<qreal> errorPerScore(100, 0);
    qreal devThresh = critchi(settings.chisquare, settings.numSequencesInAlignment - 1) / settings.numSequencesInAlignment;
    CHECK(!si.isCoR(), errorPerScore);

    QRandomGenerator rnd(settings.randomSeed);
    QByteArray randomSequence = generateRandomSequence(settings.acgtContent, settings.secondTypeErrorCalibrationLen, rnd, si);

    int dProgress = 100 - si.progress;
    int nuclsPerProgress = randomSequence.size() / dProgress;
    int progressI = nuclsPerProgress;

    QVector<PositionStats> normalizedMatrix = normalize(matrix);
    CHECK(!si.isCoR(), errorPerScore);
    QVector<int> hitsPerScore(100, 0);
    const char* seq = randomSequence.constData();
    for (int i = 0; i < randomSequence.size() - (settings.windowSize - 1); i++) {
        CHECK(!si.isCoR(), errorPerScore);
        const char* subseq = seq + i;
        qreal psum = calculatePSum(subseq, settings.windowSize, normalizedMatrix, settings, devThresh);
        CHECK(!si.isCoR(), errorPerScore);
        hitsPerScore[qRound(psum * 100)]++;
        if (--progressI == 0) {
            progressI = nuclsPerProgress;
            si.progress++;
        }
    }
    CHECK(!si.isCoR(), errorPerScore);
    int totalHits = 0;
    for (int i = 100; --i >= 0;) {
        totalHits += hitsPerScore[i];
        qreal err = totalHits / (qreal)(settings.secondTypeErrorCalibrationLen - settings.windowSize + 1);
        assert(err <= 1);
        errorPerScore[i] = err;
    }
    return errorPerScore;
}

QVector<PositionStats> SiteconAlgorithm::normalize(const QVector<PositionStats>& matrix) {
    // calculate scale average and deviation
    // normalize initial model by scale:
    //     model_ave = (model_ave - scale_ave) / scale_dev
    //     model_dev =  model_dev / scale_dev

    // normalize initial matrix;
    QVector<PositionStats> normMatrix;
    for (int i = 0, n = matrix.size(); i < n; i++) {
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

void SiteconAlgorithm::calculateACGTContent(const Msa& ma, SiteconBuildSettings& bs) {
    assert(ma->getAlphabet()->isNucleic());
    bs.acgtContent[0] = bs.acgtContent[1] = bs.acgtContent[2] = bs.acgtContent[3] = 0;
    int maLen = ma->getLength();
    int total = ma->getRowCount() * ma->getLength();
    foreach (const MsaRow& row, ma->getRows()) {
        for (int i = 0; i < maLen; i++) {
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
    for (int i = 0; i < 4; i++) {
        bs.acgtContent[i] = qRound(bs.acgtContent[i] * 100. / total);
    }
}

QByteArray SiteconAlgorithm::generateRandomSequence(const int* acgtContent, int seqLen, QRandomGenerator& rnd, TaskStateInfo&) {
    QByteArray randomSequence;
    randomSequence.reserve(seqLen);

    int aPercentRange = acgtContent[0];
    int cPercentRange = aPercentRange + acgtContent[1];
    int gPercentRange = cPercentRange + acgtContent[2];

    assert(gPercentRange + acgtContent[3] > 0);

    for (int i = 0; i < seqLen; i++) {
        int r = rnd.generate();
        qreal perc = 100 * (r / (qreal)RAND_MAX);
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

typedef QVector<QVector<qreal>> PWVector;

// #define _SITECON_ALG_DEBUG_

#ifdef _SITECON_ALG_DEBUG_

static void dumpWeights(const QString& url, const PWVector& weights, const SiteconBuildSettings& settings) {
    QVector<QByteArray> wght(settings.props.size());
    for (int j = 0; j < settings.props.size(); j++) {
        const DiPropertySitecon* p = settings.props[j];
        QString name = p->keys.value("PV");
        QString index = p->keys.value("MI");
        wght[j].append(index).append(";").append(name);
    }

    wght.resize(settings.props.size());
    for (int i = 0; i < weights.size(); i++) {
        const QVector<qreal>& posWeights = weights[i];
        for (int j = 0; j < settings.props.size(); j++) {
            qreal w = posWeights[j];
            const char* ali = TextUtils::getLineOfSpaces(wght[j].length() < 25 ? 25 - wght[j].length() : (2 - wght[j].length() % 2));
            wght[j].append(";").append(ali).append(QString::number(w, 'f', 2));
        }
    }

    QFile file(url);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    foreach (const QByteArray& w, wght) {
        file.write(w);
        file.write("\n");
    }
    file.close();
}
#endif

int SiteconAlgorithm::calculateWeights(const Msa& ma, QVector<PositionStats>& origMatrix, const SiteconBuildSettings& settings, bool matrixIsNormalized, TaskStateInfo& si) {
    CHECK(!si.isCoR(), 0);
    int modelSize = settings.windowSize - 1;
    if (settings.weightAlg == SiteconWeightAlg_None) {
        for (int i = 0; i < modelSize; i++) {
            PositionStats& ps = origMatrix[i];
            for (int j = 0; j < ps.size(); j++) {
                DiStat& ds = ps[j];
                ds.weighted = true;
            }
        }
        return settings.props.size();
    }

    CHECK(!si.isCoR(), 0);
    assert(settings.weightAlg == SiteconWeightAlg_Alg2);

    // to calculate weights by algorithm2
    // 1. generate ~modelLen*seqNums + 10 sequence with ACGT content == model content and
    // 1  for every prop calculate average weight
    // 2. for every pos select max weight per props only for di-nucls in model
    // 3. calculate diff = W2_max - W1_ave
    // 4. mark up to 6 props per pos as weighted with max-diffs < chisquare

    assert(ma->getLength() == settings.windowSize);
    assert(origMatrix.size() == settings.windowSize - 1);

    // clear weights data
    for (int i = 0; i < origMatrix.size(); i++) {
        PositionStats& ps = origMatrix[i];
        for (int j = 0; j < ps.size(); j++) {
            ps[j].weighted = false;
        }
    }

    // normalize matrix if needed
    QVector<PositionStats> normMatrix = origMatrix;
    if (!matrixIsNormalized) {
        normMatrix = normalize(origMatrix);
    }

    qreal devThreshold = critchi(settings.chisquare, settings.numSequencesInAlignment - 1) / settings.numSequencesInAlignment;

    // Part1
    // 1. compute props ave on random sequence
    int rndSeqLen = modelSize * ma->getRowCount() + 10;
    QRandomGenerator rnd(settings.randomSeed);
    QByteArray rndSeqArray = generateRandomSequence(settings.acgtContent, rndSeqLen, rnd, si);
    const char* rndSeq = rndSeqArray.constData();

    // init weights with default val
    PWVector aveWeightMatrix(modelSize);
    PWVector maxWeightMatrix(modelSize);
    for (int i = 0; i < modelSize; i++) {
        aveWeightMatrix[i].fill(0, settings.props.size());
        maxWeightMatrix[i].fill(0, settings.props.size());
    }

    // sum all psums for nSamples and create average vals per (pos, prop)
    int nSamples = rndSeqLen - modelSize;
    for (int i = 0; i < nSamples && !si.cancelFlag; i++) {
        for (int pos = 0; pos < modelSize; pos++) {
            char c1 = rndSeq[i + pos];
            char c2 = rndSeq[i + pos + 1];
            if (c1 == 'N' || c2 == 'N') {
                continue;
            }
            QVector<qreal>& posWeights = aveWeightMatrix[pos];
            PositionStats& ps = normMatrix[pos];
            for (int j = 0; j < ps.size(); j++) {
                const DiStat& ds = ps[j];
                if (ds.sdeviation < devThreshold) {
                    qreal f = ds.prop->getNormalized(c1, c2);
                    qreal expPart = (ds.average - f) / (ds.sdeviation + 0.1f);
                    qreal dinuclWeight = exp((-1) * expPart * expPart) / (ds.sdeviation + 0.1f);
                    posWeights[j] += dinuclWeight;
                }
            }
        }
    }
    // normalize psums by nSamples
    for (int i = 0; i < modelSize; i++) {
        QVector<qreal>& posWeights = aveWeightMatrix[i];
        for (int j = 0; j < posWeights.size(); j++) {
            posWeights[j] /= nSamples;
        }
    }
#ifdef _SITECON_ALG_DEBUG_
    dumpWeights("mave.txt", aveWeightMatrix, settings);
#endif
    if (si.cancelFlag) {
        return 0;
    }

    // Part2
    // 2. compute max weights per props for model
    for (int i = 0; i < modelSize; i++) {
        CHECK(!si.isCoR(), 0);
        PositionStats& ps = normMatrix[i];
        QVector<qreal>& posWeights = maxWeightMatrix[i];
        QVector<qreal>& aveWeights = aveWeightMatrix[i];
        for (int j = 0; j < ps.size(); j++) {
            const DiStat& ds = ps[j];
            qreal maxProp = 100;
            if (ds.sdeviation < devThreshold) {
                for (int k = 0; k < ma->getRowCount(); k++) {
                    char c1 = ma->charAt(k, i);
                    char c2 = ma->charAt(k, i + 1);

                    if (c1 == 'N' || c2 == 'N') {
                        continue;
                    }

                    qreal f = ds.prop->getNormalized(c1, c2);
                    qreal expPart = (ds.average - f) / (ds.sdeviation + 0.1f);
                    qreal dinuclWeight = exp((-1) * expPart * expPart) / (ds.sdeviation + 0.1f);
                    maxProp = qMin(maxProp, dinuclWeight);  // qMin is used instead of qMax for compatibility with original Sitecon
                }
            } else {
                maxProp = aveWeights[j];  // actually any value is OK -> it will be filtered from weight estimation algorithm
            }
            posWeights[j] = maxProp;
        }
    }

#ifdef _SITECON_ALG_DEBUG_
    dumpWeights("mmax.txt", maxWeightMatrix, settings);
#endif

    // select MAX_WEIGHTS properties
    for (int i = 0; i < modelSize; i++) {
        CHECK(!si.isCoR(), 0);
        QVector<qreal>& avePosWeights = aveWeightMatrix[i];
        QVector<qreal>& maxPosWeights = maxWeightMatrix[i];
        assert(avePosWeights.size() == maxPosWeights.size());
        QVector<qreal> diffs;
        for (int j = 0; j < maxPosWeights.size(); j++) {
            qreal avePropWeight = avePosWeights[j];
            qreal maxPropWeight = maxPosWeights[j];
            qreal diff = maxPropWeight - avePropWeight;
            diffs.append(diff);
        }

        PositionStats& psNorm = normMatrix[i];
        PositionStats& psOrig = origMatrix[i];
        for (int x = 0; x < MAX_WEIGHTS_ALG2; x++) {
            qreal maxVal = -110;
            for (int j = 0; j < psNorm.size(); j++) {
                qreal val = diffs[j];
                const DiStat& ds = psNorm[j];
                if (val > maxVal && ds.sdeviation < devThreshold) {
                    maxVal = val;
                }
            }
            // int nWeightedPerStep = 0;
            for (int j = 0; j < psNorm.size(); j++) {
                qreal val = diffs[j];
                const DiStat& ds = psNorm[j];
                if (val == maxVal && ds.sdeviation < devThreshold) {
                    diffs[j] = -100;
                    psNorm[j].weighted = true;
                    psOrig[j].weighted = true;
                    // nWeightedPerStep++;
                }
            }
            // assert(nWeightedPerStep <= 1);
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
    // 1 check  settings
    assert(!doAssert || settings.windowSize > 0);
    Q_UNUSED(doAssert);
    if (settings.windowSize <= 0) {
        return false;
    }
    assert(!doAssert || settings.secondTypeErrorCalibrationLen > settings.windowSize);
    if (settings.secondTypeErrorCalibrationLen <= settings.windowSize) {
        return false;
    }
    assert(!doAssert || (settings.chisquare > 0 && settings.chisquare < 1));
    if (settings.chisquare <= 0 || settings.chisquare >= 1) {
        return false;
    }
    assert(!doAssert || settings.numSequencesInAlignment > 1);
    if (settings.numSequencesInAlignment <= 1) {
        return false;
    }

    // 2 check model
    assert(!doAssert || matrix.size() == settings.windowSize - 1);
    if (matrix.size() != settings.windowSize - 1) {
        return false;
    }
    for (int pos = 0; pos < matrix.size(); pos++) {
        const PositionStats& posStat = matrix[pos];
        assert(!doAssert || posStat.size() == settings.props.size());
        int nWeights = 0;
        for (int i = 0; i < posStat.size(); i++) {
            const DiStat& ds = posStat[i];
            if (ds.weighted) {
                nWeights++;
            }
            const DiPropertySitecon* p = settings.props[i];
            assert(!doAssert || ds.prop == p);
            if (ds.prop != p) {
                return false;
            }
        }

        if (settings.weightAlg == SiteconWeightAlg_None) {
            assert(!doAssert || nWeights == settings.props.size());
            if (nWeights != settings.props.size()) {
                return false;
            }
        } else {
            // assert(!doAssert || nWeights <= MAX_WEIGHTS_ALG2);
            if (nWeights > MAX_WEIGHTS_ALG2) {
                algoLog.trace(QString("Number of Algorithm 2 weights %1, pos %2, model name %3").arg(nWeights).arg(pos).arg(modelName));
                // return false;
            }
        }
    }
    for (int i = 0; i < 100; i++) {
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

}  // namespace U2
