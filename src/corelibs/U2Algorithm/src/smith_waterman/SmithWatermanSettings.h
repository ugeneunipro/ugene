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

#ifndef _U2_SMITH_WATERMAN_SETTINGS_H_
#define _U2_SMITH_WATERMAN_SETTINGS_H_

#include <U2Core/U2Region.h>
#include <U2Core/DNATranslation.h>

#include <U2Core/SMatrix.h>
#include <U2Algorithm/SmithWatermanReportCallback.h>
#include <U2Algorithm/SmithWatermanResultFilters.h>
#include <U2Algorithm/SmithWatermanResult.h>
#include <U2Algorithm/SmithWatermanResult.h>
#include <U2Core/SequenceWalkerTask.h>

#include <QHash>

static const char * ANNOTATION_RESULT_VIEW = "Annotations";
static const char * MULTIPLE_ALIGNMENT_RESULT_VIEW = "Multiple alignment";
const qint32 STRING_HAS_NO_KEY_MESSAGE = -1;

namespace U2 {

static inline bool isComplement(StrandOption strand) {
    return StrandOption_Both== strand || StrandOption_ComplementOnly == strand;
}
static inline bool isDirect(StrandOption strand) {
        return StrandOption_Both== strand || StrandOption_DirectOnly == strand;
}

struct GapModel {
    int scoreGapOpen;
    int scoreGapExtd;
};

struct SmithWatermanSettings {
    enum SWResultView {
        ANNOTATIONS = 1, MULTIPLE_ALIGNMENT
    };

    SmithWatermanSettings()
        : searchCircular(false), percentOfScore(0), complTT( NULL ), aminoTT( NULL ),
        resultListener( NULL ), resultFilter( NULL ),
        resultCallback( NULL ), resultView(ANNOTATIONS), includePatternContent( false )
        /* @resultView is initialized with SWResultView::ANNOTATIONS
            for successful tests passing's sake, so when using
            in code @resultView should be assigned explicitly*/
    {
        getResultViewNames(ANNOTATION_RESULT_VIEW, ANNOTATIONS);
        getResultViewNames(MULTIPLE_ALIGNMENT_RESULT_VIEW, MULTIPLE_ALIGNMENT);
    }

    QByteArray          ptrn;
    QByteArray          sqnc;
    bool                searchCircular;

    U2Region            globalRegion;
    StrandOption        strand;

    float               percentOfScore;
    GapModel            gapModel;
    SMatrix             pSm;

    DNATranslation*     complTT;
    DNATranslation*     aminoTT;

    SmithWatermanResultListener* resultListener;
    SmithWatermanResultFilter*   resultFilter;
    SmithWatermanReportCallback* resultCallback;

    SWResultView resultView;
    bool includePatternContent; // specifies whether the pattern subsequences appropriate
                                // to reference subsequences are to be included to algorithm
                                // results in case of annotation result view

    bool operator==(const SmithWatermanSettings& op) const {
        return  ptrn == op.ptrn &&
                sqnc == op.sqnc &&
                searchCircular == op.searchCircular &&
                globalRegion == op.globalRegion &&
                strand == op.strand &&
                gapModel.scoreGapExtd == op.gapModel.scoreGapExtd &&
                gapModel.scoreGapOpen == op.gapModel.scoreGapOpen;
    }

    qint32 getResultViewKeyForString(const QString & value) {
        foreach(const char * name, getResultViewNames().values()) {
            QByteArray understandableName(name);
            if(understandableName == value.toLocal8Bit()) {
                return getResultViewNames().key(name);
            }
        }
        return STRING_HAS_NO_KEY_MESSAGE;
    }

    static QHash<SWResultView, const char *> & getResultViewNames(const char * newResultName = NULL, SWResultView key = static_cast<SWResultView>(0)) {
        static QHash<SWResultView, const char *> resultViewNames;

        if(NULL != newResultName && static_cast<SWResultView>(0) != key) {
            if(!resultViewNames.contains(key)) {
                resultViewNames[key] = newResultName;
            }
        }
        return resultViewNames;
    }
};

} // namespace

#endif
