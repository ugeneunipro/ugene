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

#pragma once

#include <QRandomGenerator>
#include <QVector>

#include <U2Core/Msa.h>
#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

namespace U2 {

class DiPropertySitecon;

/** Average and deviation for one property. */
class DiStat {
public:
    DiStat(DiPropertySitecon* p, qreal d, qreal a)
        : prop(p), sdeviation(d), average(a), weighted(false) {
    }
    DiStat()
        : prop(nullptr), sdeviation(-1), average(-1), weighted(false) {
    }

    DiPropertySitecon* prop;
    qreal sdeviation;
    qreal average;
    bool weighted;
};
typedef QVector<DiStat> PositionStats;

enum SiteconWeightAlg {
    SiteconWeightAlg_None,
    SiteconWeightAlg_Alg2
};

class SiteconBuildSettings {
public:
    SiteconBuildSettings()
        : windowSize(0), randomSeed(0), secondTypeErrorCalibrationLen(100 * 1000),
          chisquare(0.95f), numSequencesInAlignment(0), weightAlg(SiteconWeightAlg_None) {
        acgtContent[0] = acgtContent[1] = acgtContent[2] = acgtContent[3] = 25;
    }

    int windowSize;
    int randomSeed;
    int secondTypeErrorCalibrationLen;
    qreal chisquare;
    int numSequencesInAlignment;
    SiteconWeightAlg weightAlg;
    int acgtContent[4];
    QList<DiPropertySitecon*> props;
};

class SiteconModel {
public:
    SiteconModel() {
        deviationThresh = -1;
    }
    QString aliURL;
    QString modelName;
    QString description;
    SiteconBuildSettings settings;
    QVector<PositionStats> matrix;
    QVector<qreal> err1;
    QVector<qreal> err2;
    qreal deviationThresh;
    bool checkState(bool doAssert = true) const;
    bool operator!=(const SiteconModel& model) const;
};

class DNATranslation;
class SiteconAlgorithm : public QObject {
    Q_OBJECT
public:
    static QVector<PositionStats> calculateDispersionAndAverage(const Msa& ma, const SiteconBuildSettings& s, TaskStateInfo& ts);

    static qreal calculatePSum(const char* seq, int len, const QVector<PositionStats>& normalizedMatrix, const SiteconBuildSettings& settings, qreal devThreshold, DNATranslation* complMap = nullptr);

    static QVector<qreal> calculateFirstTypeError(const Msa& ma, const SiteconBuildSettings& s, TaskStateInfo& ts);

    static QVector<qreal> calculateSecondTypeError(const QVector<PositionStats>& matrix, const SiteconBuildSettings& s, TaskStateInfo& ts);

    static QVector<PositionStats> normalize(const QVector<PositionStats>& matrix);

    static int calculateWeights(const Msa& ma, QVector<PositionStats>& matrix, const SiteconBuildSettings& settings, bool matrixIsNormalized, TaskStateInfo& s);

    static void calculateACGTContent(const Msa& ma, SiteconBuildSettings& bs);

    static QByteArray generateRandomSequence(const int* actgContent, int seqLen, QRandomGenerator& rnd, TaskStateInfo& ts);
};

}  // namespace U2

