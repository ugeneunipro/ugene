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

#ifndef _U2_RAW_DATA_CHECK_H_
#define _U2_RAW_DATA_CHECK_H_

#include <QVariantMap>

namespace U2 {

// Result of the format detection algorithm
// Note: High/Very High, Low/Very low selection is the result of the quality of detection algorithm
// For example if detection algorithm is not advanced enough it must not use VeryHigh rating
enum FormatDetectionScore {
    FormatDetection_NotMatched = -10, // format is not matched and can't be parsed at all
    FormatDetection_VeryLowSimilarity = 1, // very low similarity found. Parsing is allowed
    FormatDetection_LowSimilarity = 2, // save as very low, but slightly better, used as extra step in cross-formats differentiation
    FormatDetection_AverageSimilarity = 3, //see above
    FormatDetection_HighSimilarity = 4,//see above
    FormatDetection_VeryHighSimilarity = 5,//see above
    FormatDetection_Matched = 10 // here we 100% sure that we deal with a known and supported format.
};


/** The result of the document format detection: score and additional info that was parsed during raw data check */
class FormatCheckResult {

public:
    FormatCheckResult() : score(FormatDetection_NotMatched){}
    FormatCheckResult(FormatDetectionScore _score) : score(_score){}

    /** Score of the detection */
    int score;

    QVariantMap properties;
};

}//namespace

#endif
