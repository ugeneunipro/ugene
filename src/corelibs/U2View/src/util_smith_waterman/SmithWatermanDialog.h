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

#ifndef _U2_SMITH_WATERMAN_DIALOG_H_
#define _U2_SMITH_WATERMAN_DIALOG_H_

#include <U2Algorithm/SmithWatermanSettings.h>

namespace U2 {

class ADVSequenceObjectContext;
class CreateAnnotationWidgetController;

#define DEF_PERCENT_OF_SCORE    90
#define DEF_GAP_OPEN_SCORE      (-10)
#define DEF_GAP_EXTD_SCORE      (-1)

enum SmithWatermanRangeType {
    SmithWatermanRangeType_noRange,
    SmithWatermanRangeType_wholeSequence,
    SmithWatermanRangeType_selectedRange,
    SmithWatermanRangeType_customRange
};

enum SmithWatermanSearchType {
    SmithWatermanSearchType_noSearch,
    SmithWatermanSearchType_inSequence,
    SmithWatermanSearchType_inTranslation
};

struct SWDialogConfig {
    SWDialogConfig() {
        ptrn = QByteArray();
        algVersion = QString("");
        scoringMatrix = QString("");

        gm.scoreGapOpen = DEF_GAP_OPEN_SCORE;
        gm.scoreGapExtd = DEF_GAP_EXTD_SCORE;

        resultFilter = QString("");

        minScoreInPercent = DEF_PERCENT_OF_SCORE;

        searchType  = SmithWatermanSearchType_noSearch;
        strand      = StrandOption_DirectOnly;
        rangeType   = SmithWatermanRangeType_noRange;

        resultView = SmithWatermanSettings::ANNOTATIONS;

        alignFilesNamesTmpl = QString("");
        refSubseqNamesTmpl = QString("");
        ptrnSubseqNamesTmpl = QString("");
        patternSequenceName = QString("");

        enableAdvancedMASettings = false;
        countOfLaunchesAlgorithm = 1;
        addPatternSubsequenceAsQualifier = false;
    }
    QByteArray ptrn;

    QString algVersion;
    QString scoringMatrix;
    GapModel gm;
    QString resultFilter;

    int minScoreInPercent;      // c {0, 1, ..., 100}

    SmithWatermanSearchType searchType;
    StrandOption strand;
    SmithWatermanRangeType  rangeType;

    SmithWatermanSettings::SWResultView resultView;

    QString alignFilesNamesTmpl;
    QString refSubseqNamesTmpl;
    QString ptrnSubseqNamesTmpl;
    QString patternSequenceName;

    bool enableAdvancedMASettings;
    int countOfLaunchesAlgorithm;
    bool addPatternSubsequenceAsQualifier;
};

class U2VIEW_EXPORT SmithWatermanDialogController {
public:
    static void run(QWidget* p, ADVSequenceObjectContext* ctx, SWDialogConfig* dialogConfig);
};

} // namespace

#endif

