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

#include "MsaColorSchemeWeakSimilarities.h"

#include <U2Core/MultipleAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const QList<QPair<QColor, QColor>> MsaColorSchemeWeakSimilarities::colorPairsByFrequence = {QPair<QColor, QColor>(QColor("#0000FF"), QColor("#00FFFF")), 
                                                                                            QPair<QColor, QColor>(QColor("#FF00FF"), QColor("#FFFFFF")), 
                                                                                            QPair<QColor, QColor>(QColor("#000000"), QColor("#FFFFFF")), 
                                                                                            QPair<QColor, QColor>(QColor("#000000"), QColor("#C0C0C0")), 
                                                                                            QPair<QColor, QColor>(QColor("#FF6600"), QColor("#FFFFFF"))};

const QPair<QColor, QColor> MsaColorSchemeWeakSimilarities::gapColorPair = QPair<QColor, QColor>(QColor("#000000"), QColor("#FFFFFF"));

MsaColorSchemeWeakSimilarities::MsaColorSchemeWeakSimilarities(QObject *parent, const MsaColorSchemeFactory *factory, MultipleAlignmentObject *maObj)
    : MsaColorSchemePercentageIdententityColored(parent, factory, maObj) {
 }

QColor MsaColorSchemeWeakSimilarities::getBackgroundColor(int seq, int pos, char c) const {
    Q_UNUSED(seq);
    if (c == U2Msa::GAP_CHAR) {
        return gapColorPair.second;
    }

    updateCache(pos);
    SAFE_POINT(cachedData.keys().contains(pos), "Column data is absent", gapColorPair.second);

    int fontColorIndex = getColorIndex(pos, c);
    return colorPairsByFrequence[fontColorIndex].second;
 }

QColor MsaColorSchemeWeakSimilarities::getFontColor(int seq, int pos, char c) const {
     Q_UNUSED(seq);

     if (c == U2Msa::GAP_CHAR) {
         return gapColorPair.first;
     }

     updateCache(pos);
     SAFE_POINT(cachedData.keys().contains(pos), "Column data is absent", QColor());

     int fontColorIndex = getColorIndex(pos, c);
     return colorPairsByFrequence[fontColorIndex].first;
 }

int MsaColorSchemeWeakSimilarities::getColorIndex(const int columnNum, const char c) const {
     int index = 0;
     const ColumnCharsCounter currentColumnData = cachedData.value(columnNum);
     QList<Nucleotide> currentNucleotideList = currentColumnData.getNucleotideList();
     const int size = currentNucleotideList.size();
     CHECK(size > 0, index);
     foreach(const Nucleotide & n, currentNucleotideList) {
         if (n.character == c || index == 4) {
             break;
         }
         index++;
     }
     return index;
 }


 MsaColorSchemeWeakSimilaritiesFactory::MsaColorSchemeWeakSimilaritiesFactory(QObject *parent, const QString &id, const QString &name, const AlphabetFlags &supportedAlphabets)
     : MsaColorSchemeFactory(parent, id, name, supportedAlphabets) {
 }

MsaColorScheme *MsaColorSchemeWeakSimilaritiesFactory::create(QObject *parent, MultipleAlignmentObject *maObj) const {
    return new MsaColorSchemeWeakSimilarities(parent, this, maObj);
 }

 }    // namespace U2