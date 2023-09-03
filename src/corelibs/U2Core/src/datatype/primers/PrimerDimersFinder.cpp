/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "PrimerDimersFinder.h"

#include <QMap>

#include <U2Core/AppContext.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

#include "PrimerStatistics.h"

namespace U2 {

/************************************************************************/
/* DimerFinderResult */
/************************************************************************/
QString DimerFinderResult::getFullReport() const {
    return QString("<b>Delta</b> G: %1 kcal/mole <b>Base Pairs:</b> %2").arg(deltaG).arg(baseCounts) + "<pre>" + dimersOverlap + "</pre>";
}
QString DimerFinderResult::getShortReport() const {
    return QString("Delta G: %1 kcal/mole<br>Base Pairs: %2").arg(deltaG).arg(baseCounts);
}

/************************************************************************/
/* DrimersFinder */
/************************************************************************/

const QMap<QByteArray, qreal> BaseDimersFinder::ENERGY_MAP = {
    {"AA", -1.9},
    {"TT", -1.9},

    {"AT", -1.5},
    {"TA", -1.0},

    {"CA", -2.0},
    {"TG", -2.0},

    {"AC", -1.3},
    {"GT", -1.3},

    {"CT", -1.6},
    {"AG", -1.6},
    {"GA", -1.6},
    {"TC", -1.6},

    {"CG", -3.6},
    {"GC", -3.1},

    {"GG", -3.1},
    {"CC", -3.1}};

BaseDimersFinder::BaseDimersFinder(const QByteArray& forwardPrimer, const QByteArray& reversePrimer, double energyThreshold)
    : forwardPrimer(forwardPrimer), reversePrimer(reversePrimer), energyThreshold(energyThreshold), maximumDeltaG(0) {
    DNATranslationRegistry* tr = AppContext::getDNATranslationRegistry();
    DNATranslation* dnaTranslation = tr->lookupTranslation(BaseDNATranslationIds::NUCL_DNA_DEFAULT_COMPLEMENT);

    if (dnaTranslation != nullptr) {
        int bufSize = reversePrimer.size();
        reverseComplementSequence.resize(bufSize);
        reverseComplementSequence.fill(0);
        dnaTranslation->translate(reversePrimer, bufSize, reverseComplementSequence.data(), bufSize);
        TextUtils::reverse(reverseComplementSequence.data(), bufSize);
    }

    resHomologousRegion.resize(qMax(forwardPrimer.size(), reverseComplementSequence.size()));
    resHomologousRegion.fill(' ');
}

void BaseDimersFinder::fillResultsForCurrentIteration(const QByteArray& homologousBases, int overlapStartPos) {
    double freeEnergy = 0.0;
    int startPos = 0;
    for (int i = 0; i < homologousBases.size() - 1; i++) {
        QByteArray curArray;
        curArray.append(homologousBases.at(i));
        curArray.append(homologousBases.at(i + 1));
        bool homologousRegionEnded = !ENERGY_MAP.contains(curArray);
        if (!homologousRegionEnded) {
            freeEnergy += ENERGY_MAP[curArray];
        }
        if (homologousRegionEnded || i == homologousBases.size() - 2) {
            if (freeEnergy < maximumDeltaG) {
                maximumDeltaG = freeEnergy;
                resHomologousRegion = homologousBases;
                overlappingRegion.startPos = startPos;
                overlappingRegion.length = i - startPos + 2;
                dimersOverlap = getDimersOverlapping(overlapStartPos);
            }
            freeEnergy = 0.0;
            startPos = i + 1;
        }
    }
}

DimerFinderResult BaseDimersFinder::getResult() const {
    DimerFinderResult result;
    result.dimersOverlap = dimersOverlap;
    result.dimer = resHomologousRegion.mid(overlappingRegion.startPos, overlappingRegion.length);
    result.baseCounts = overlappingRegion.length;
    result.deltaG = maximumDeltaG;
    result.canBeFormed = maximumDeltaG < energyThreshold;
    return result;
}

/************************************************************************/
/* DrimersFinder */
/************************************************************************/
SelfDimersFinder::SelfDimersFinder(const QByteArray& _forwardPattern, const qreal energyThreshold)
    : BaseDimersFinder(_forwardPattern, _forwardPattern, energyThreshold) {
    for (int sequenceShift = 1; sequenceShift < forwardPrimer.size(); sequenceShift++) {
        int index = forwardPrimer.size() - 3 - sequenceShift;
        QByteArray homologousRegion(forwardPrimer.size(), ' ');
        for (int pos = sequenceShift; pos >= 0; pos--) {
            if (index < 0) {
                break;
            }
            if (forwardPrimer.at(index) == reverseComplementSequence.at(pos)) {
                homologousRegion[index] = forwardPrimer.at(index);
            }
            index--;
        }

        fillResultsForCurrentIteration(homologousRegion, sequenceShift);
        homologousRegion.fill(' ');
    }
}

QString SelfDimersFinder::getDimersOverlapping(int dimerFormationPos) {
    QString drimerInfo;

    int corDimerFormationPos = dimerFormationPos + 1;
    int forwardFormationPos = forwardPrimer.size() - dimerFormationPos - 2;
    int patternLength = qMax(forwardFormationPos, corDimerFormationPos);
    int indent = corDimerFormationPos - forwardFormationPos;

    int index = 0;
    for (int i = 0; i < patternLength; i++) {
        if (i >= indent) {
            drimerInfo.append(forwardPrimer.at(index));
            index++;
        } else {
            drimerInfo.append(' ');
        }
    }
    drimerInfo.append("\n");
    index = 0;
    drimerInfo.append("<font color='red'>");
    for (int i = 0; i < patternLength; i++) {
        if (i >= indent) {
            if (resHomologousRegion.at(index) == ' ') {
                drimerInfo.append(' ');
            } else {
                drimerInfo.append('|');
            }
            index++;
        } else {
            drimerInfo.append(' ');
        }
    }
    drimerInfo.append("</font>");
    drimerInfo.append(forwardPrimer.at(index));

    drimerInfo.append("\n");
    index = forwardPrimer.size() - 1;
    for (int i = 0; i < patternLength; i++) {
        if (i >= -indent) {
            drimerInfo.append(forwardPrimer.at(index));
            index--;
        } else {
            drimerInfo.append(' ');
        }
    }
    return drimerInfo;
}

/************************************************************************/
/* HeteroDimersFinder */
/************************************************************************/
HeteroDimersFinder::HeteroDimersFinder(const QByteArray& _forwardPattern, const QByteArray& reversePattern, const qreal energyThreshold)
    : BaseDimersFinder(_forwardPattern, reversePattern, energyThreshold) {
    for (int sequenceShift = -forwardPrimer.size(); sequenceShift < forwardPrimer.size(); sequenceShift++) {
        QByteArray homologousRegion(qMin(forwardPrimer.size(), reversePattern.size()), ' ');
        for (int reverseIndex = 0; reverseIndex < reverseComplementSequence.size(); reverseIndex++) {
            int forwardIndex = sequenceShift + reverseIndex;
            if (forwardIndex < 0) {
                continue;
            }
            if (forwardIndex >= forwardPrimer.size() - 1) {
                break;
            }
            if (reverseIndex >= homologousRegion.size()) {
                break;
            }
            if (forwardPrimer.at(forwardIndex) == reverseComplementSequence.at(reverseIndex)) {
                homologousRegion[reverseIndex] = reverseComplementSequence.at(reverseIndex);
            }
        }

        fillResultsForCurrentIteration(homologousRegion, sequenceShift);
        homologousRegion.fill(' ');
    }
}

QString HeteroDimersFinder::getDimersOverlapping(int dimerFormationPos) {
    QString drimerInfo;
    for (int i = 0; i < -dimerFormationPos; i++) {
        drimerInfo.append(' ');
    }
    foreach (char curChar, forwardPrimer) {
        drimerInfo.append(curChar);
    }
    drimerInfo.append("\n");

    for (int i = 0; i < dimerFormationPos; i++) {
        drimerInfo.append(' ');
    }
    drimerInfo.append("<font color='red'>");
    for (int i = 0; i < resHomologousRegion.size(); i++) {
        if (resHomologousRegion.at(i) == ' ') {
            drimerInfo.append(' ');
        } else if (overlappingRegion.contains(i)) {
            drimerInfo.append('|');
        } else {
            drimerInfo.append(':');
        }
    }
    drimerInfo.append("</font>");
    drimerInfo.append("\n");

    for (int i = 0; i < dimerFormationPos; i++) {
        drimerInfo.append(' ');
    }
    for (int i = reversePrimer.size() - 1; i >= 0; i--) {
        drimerInfo.append(reversePrimer.at(i));
    }

    return drimerInfo;
}

}  // namespace U2
