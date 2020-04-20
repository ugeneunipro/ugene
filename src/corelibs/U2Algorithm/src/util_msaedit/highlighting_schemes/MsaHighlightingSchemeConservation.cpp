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

#include <QColor>

#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include "MsaHighlightingSchemeConservation.h"

namespace U2 {

MsaHighlightingSchemeConservation::MsaHighlightingSchemeConservation(QObject *parent, const MsaHighlightingSchemeFactory *factory, MultipleAlignmentObject *maObj)
    : MsaHighlightingScheme(parent, factory, maObj),
      threshold(50),
      lessThenThreshold(false)
{
    connect(maObj, SIGNAL(si_alignmentChanged(const MultipleAlignment &, const MaModificationInfo &)), SLOT(sl_resetMap()));
}

void MsaHighlightingSchemeConservation::process(const char refChar, char &seqChar, QColor &color, bool &highlight, int refCharColumn, int refCharRow) const {
    if (!msaCharCountMap.contains(refCharColumn)) {
        calculateStatisticForColumn(refCharColumn);
    }

    int neededThr = (int)((float)(threshold * maObj->getNumRows())/100.0 + 0.5);
    if (lessThenThreshold) {
        highlight = (msaCharCountMap[refCharColumn][seqChar] <= neededThr);
    } else {
        highlight = (msaCharCountMap[refCharColumn][seqChar] >= neededThr);
    }
    if (!highlight) {
        color = QColor();
    }
    MsaHighlightingScheme::process(refChar, seqChar, color, highlight, refCharColumn, refCharRow);
}

void MsaHighlightingSchemeConservation::applySettings(const QVariantMap &settings){
    QVariant thresholdQVar = settings.value(THRESHOLD_PARAMETER_NAME);
    if (!thresholdQVar.isNull()) {
        bool ok;
        int  convertedThreshold = thresholdQVar.toInt(&ok);
        CHECK(ok, );
        threshold = convertedThreshold;
    }
    lessThenThreshold = settings.value(LESS_THAN_THRESHOLD_PARAMETER_NAME, lessThenThreshold).toBool();
}

QVariantMap MsaHighlightingSchemeConservation::getSettings() const {
    QVariantMap settings;
    settings.insert(THRESHOLD_PARAMETER_NAME, threshold);
    settings.insert(LESS_THAN_THRESHOLD_PARAMETER_NAME, lessThenThreshold);
    return settings;
}

void MsaHighlightingSchemeConservation::sl_resetMap() {
    msaCharCountMap.clear();
}

void MsaHighlightingSchemeConservation::calculateStatisticForColumn(int refCharColumn) const {
    CHECK(!msaCharCountMap.contains(refCharColumn), );
    CharCountMap columnStatistic;
    const MultipleAlignment ma = maObj->getMultipleAlignment();
    for (int row = ma->getNumRows() - 1; row >= 0; row--) {
        char seqChar = ma->charAt(row, refCharColumn);
        if (columnStatistic.contains(seqChar)) {
            columnStatistic[seqChar] += 1;
        } else {
            columnStatistic[seqChar] = 1;
        }
    }
    msaCharCountMap[refCharColumn] = columnStatistic;
}

MsaHighlightingSchemeConservationFactory::MsaHighlightingSchemeConservationFactory(QObject *parent, const QString &id, const QString &name, const AlphabetFlags &supportedAlphabets)
    : MsaHighlightingSchemeFactory(parent, id, name, supportedAlphabets, true, true)
{

}

MsaHighlightingScheme * MsaHighlightingSchemeConservationFactory::create(QObject *parent, MultipleAlignmentObject *maObj) const {
    return new MsaHighlightingSchemeConservation(parent, this, maObj);
}

}   // namespace U2
