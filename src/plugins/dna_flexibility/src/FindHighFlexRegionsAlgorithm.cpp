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

#include "FindHighFlexRegionsAlgorithm.h"
#include "HighFlexSettings.h"

#include <U2Core/U2SafePoints.h>


namespace U2 {


void FindHighFlexRegionsAlgorithm::find(
        FindHighFlexRegionsListener* resultsListener,
        const HighFlexSettings& settings,
        const QByteArray sequence,
        const int seqLength,
        int& stopFlag,
        int& percentsCompleted)
{
    Q_UNUSED(stopFlag); Q_UNUSED(percentsCompleted); //TODO: fix it

    SAFE_POINT(settings.windowSize <= seqLength,
        "Internal error: a user is not allowed to input such window size",);
    SAFE_POINT(settings.windowStep <= seqLength-2,
        "Internal error: a user is not allowed to input such window step",)

    int windowLeft = 0;
    bool previousWindowIsHighFlex = false;
    int areaHighFlexLeft = 0;
    int areaHighFlexWindowsNumber = 0;
    double areaTotalThreshold = 0.0;


    while (windowLeft <= seqLength - settings.windowSize)
    {
        // Calculating the threshold in the current window
        double windowThreshold = 0;
        for (int i = windowLeft; i < windowLeft + settings.windowSize - 1; ++i)
        {
            windowThreshold += flexibilityAngle(sequence[i], sequence[i + 1]);
        }
        windowThreshold /= (settings.windowSize - 1);

        // If the current window is a high flex window
        if (windowThreshold >= settings.threshold)
        {
            // Enlarging the total threshold
            // (to later calculate the average threshold i.e. "area")
            areaTotalThreshold += windowThreshold;

            // Look for the previous windows
            if (!previousWindowIsHighFlex)
            {
                // Remembering that the current window is high flex window
                previousWindowIsHighFlex = true;
                areaHighFlexLeft = windowLeft;
            }
            else
            {
                // The current and previous windows are high flex windows
                areaHighFlexWindowsNumber++;
            }
        }

        // If the current window is "normal" or it is the last window
        if ((windowThreshold < settings.threshold) ||
            (windowLeft + settings.windowSize >= seqLength))
        {
            if (!previousWindowIsHighFlex)
            {
                // Do nothing: we are not in the high flex region
            }
            else
            {
                // If the area contains more than 1 window, than create the result
                if (areaHighFlexWindowsNumber > 0)
                {
                    // Calculating the area length == right coordinate of the previous window - left coordinate
                    //
                    int areaLength;

                    // If this is the last window and the threshold is high
                    if (windowThreshold >= settings.threshold)
                    {
                        areaLength = windowLeft + (settings.windowSize * 2)
                            - settings.windowStep - areaHighFlexLeft;
                    }
                    else
                    {
                        // The area has been finished on the previous window
                        areaLength = windowLeft + settings.windowSize
                            - settings.windowStep - areaHighFlexLeft;
                    }

                    // Creating the region
                    U2Region resultRegion(
                        areaHighFlexLeft,  // left coordinate of the area
                        areaLength);

                    // Calculating the area average threshold
                    double areaAverageThreshold
                        = areaTotalThreshold / (areaHighFlexWindowsNumber + 1); // "+1" as it starts from 0

                    // Adding the result found to the results
                    HighFlexResult result(resultRegion,
                        areaAverageThreshold,
                        areaHighFlexWindowsNumber + 1,
                        areaTotalThreshold);
                    addToResults(resultsListener, result);
                }

                // Clearing the area variables for new results
                areaHighFlexWindowsNumber = 0;
                previousWindowIsHighFlex = false;
                areaTotalThreshold = 0.0;
            }
        }

        // Shifting the window with the specified step
        windowLeft += settings.windowStep;
    };
}


double FindHighFlexRegionsAlgorithm::flexibilityAngle(char firstNucleotide, char secondNucleotide)
{
    if (firstNucleotide == 'A')
    {
        if (secondNucleotide == 'A') {return 7.6; }
        if (secondNucleotide == 'C') {return 14.6; }
        if (secondNucleotide == 'G') {return 8.2; }
        if (secondNucleotide == 'T') {return 25; }
        return 7.6; // if second is N or -, choose minimum
    }

    if (firstNucleotide == 'C')
    {
        if (secondNucleotide == 'A') {return 10.9; }
        if (secondNucleotide == 'C') {return 7.2; }
        if (secondNucleotide == 'G') {return 8.9; }
        if (secondNucleotide == 'T') {return 8.2; }
        return 7.2; // if second is N or -, choose minimum
    }

    if (firstNucleotide == 'G')
    {
        if (secondNucleotide == 'A') {return 8.8; }
        if (secondNucleotide == 'C') {return 11.1; }
        if (secondNucleotide == 'G') {return 7.2; }
        if (secondNucleotide == 'T') {return 14.6; }
        return 7.2; // if second is N or -, choose minimum
    }

    if (firstNucleotide == 'T')
    {
        if (secondNucleotide == 'A') {return 12.5; }
        if (secondNucleotide == 'C') {return 8.8; }
        if (secondNucleotide == 'G') {return 10.9; }
        if (secondNucleotide == 'T') {return 7.6; }
        return 7.6; // if second is N or -, choose minimum
    }

    if (firstNucleotide == 'N' || firstNucleotide == '-')
    {
        if (secondNucleotide == 'A') {return 7.6; } // minimum of AA, CA, GA, TA
        if (secondNucleotide == 'C') {return 7.2; } // minimum of AC, CC, GC, TC
        if (secondNucleotide == 'G') {return 7.2; } // minimum of AG, CG, GG, TG
        if (secondNucleotide == 'T') {return 7.6; } // minimum of AT, CT, GT, TT
        return 7.2; // if second is N or -, choose minimum
    }

    FAIL("Illegal combination of nucleotides. The standard DNA alphabet is only supported.", 0);
}


void FindHighFlexRegionsAlgorithm::addToResults(
    FindHighFlexRegionsListener* resultsListener,
    const HighFlexResult& result)
{
    SAFE_POINT(resultsListener, "FindHighFlexRegionsListener is not available!",);

    resultsListener->onResult(result);
}


} // namespace
