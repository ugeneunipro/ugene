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

#include "MsaColorSchemePercentageIdententityColored.h"

#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const QList<char> MsaColorSchemePercentageIdententityColored::NUCLEOTIDE_LIST     = { 'T', 'U', 'G', 'C', 'A', 'B', 'D', 'H', 'K', 'M', 'R', 'S', 'V', 'W', 'Y', 'N'};
const QList<QColor> MsaColorSchemePercentageIdententityColored::BACKGROUND_COLORS = { Qt::white, Qt::yellow, Qt::green, Qt::cyan };
const QList<QColor> MsaColorSchemePercentageIdententityColored::FONT_COLORS       = { Qt::black, Qt::red,    Qt::black, Qt::blue };

MsaColorSchemePercentageIdententityColored::MsaColorSchemePercentageIdententityColored(QObject *parent, const MsaColorSchemeFactory *factory, MultipleAlignmentObject *maObj)
                                            : MsaColorScheme(parent, factory, maObj),
                                              alignmentChanged(false),
                                              threshold(50.0)
{
    connect(maObj, SIGNAL(si_alignmentChanged(const MultipleAlignment&, const MaModificationInfo&)), this, SLOT(sl_alignmentChanged()));
}

QColor MsaColorSchemePercentageIdententityColored::getBackgroundColor(int rowNum, int columnNum, char c) const {
    Q_UNUSED(rowNum);

    updateCache(columnNum);
    SAFE_POINT(cachedData.keys().contains(columnNum), "Column data is absent", QColor());

    int backgroundColorIndex = getColorIndex(columnNum, c);
    QColor backgroundColor = BACKGROUND_COLORS.value(backgroundColorIndex, QColor());

    return backgroundColor;
}

QColor MsaColorSchemePercentageIdententityColored::getFontColor(int rowNum, int columnNum, char c) const {
    Q_UNUSED(rowNum);

    updateCache(columnNum);
    SAFE_POINT(cachedData.keys().contains(columnNum), "Column data is absent", QColor());

    int fontColorIndex = getColorIndex(columnNum, c);
    QColor fontColor = FONT_COLORS.value(fontColorIndex, QColor());

    return fontColor;
}

void MsaColorSchemePercentageIdententityColored::applySettings(const QVariantMap& settings) {
    threshold = settings.value(THRESHOLD_PARAMETER_NAME).toDouble();
}

void MsaColorSchemePercentageIdententityColored::sl_alignmentChanged() {
    alignmentChanged = true;
}

void MsaColorSchemePercentageIdententityColored::updateCache(const int columnNum) const {
    if (alignmentChanged) {
        cachedData.clear();
        alignmentChanged = false;
    } else if (cachedData.keys().contains(columnNum)) {
        return;
    }

    SAFE_POINT(columnNum < maObj->getLength(), "Unexpected column number", );

    ColumnCharsCounter currentRowCounter;
    foreach(const MultipleAlignmentRow& row, maObj->getRows()) {
        char ch = row.data()->charAt(columnNum);
        if (NUCLEOTIDE_LIST.contains(ch)) {
            currentRowCounter.addNucleotide(ch);
        } else if (ch == U2Msa::GAP_CHAR) {
            currentRowCounter.addGap();
        } else {
            currentRowCounter.addNonAlphabetCharacter();
        }
    }
    currentRowCounter.sortNucleotideList();

    cachedData.insert(columnNum, currentRowCounter);
}

int MsaColorSchemePercentageIdententityColored::getColorIndex(const int columnNum, const char c) const {
    int index = 0;
    const ColumnCharsCounter currentColumnData = cachedData.value(columnNum);
    QList<Nucleotide> currentNucleotideList = currentColumnData.getNucleotideList();
    const int size = currentNucleotideList.size();
    CHECK(size > 0, index);

    const bool hasGaps = currentColumnData.hasGaps();
    const bool hasNonAlphabetCharsNumber = currentColumnData.hasNonAlphabetCharsNumber();
    const bool hasPercentageMoreThenThreshold = currentColumnData.hasPercentageMoreThen(threshold);
    if (size == 1 && !hasGaps && !hasNonAlphabetCharsNumber) {
        index = 1;
    } else if (size == 2 && !hasNonAlphabetCharsNumber &&
        currentNucleotideList[0].frequency == currentNucleotideList[1].frequency &&
        currentNucleotideList[0].character == c) {
        index = 2;
    } else if (hasPercentageMoreThenThreshold &&
        currentNucleotideList[0].character == c) {
        index = 3;
    }

    return index;
}

}   // namespace U2
