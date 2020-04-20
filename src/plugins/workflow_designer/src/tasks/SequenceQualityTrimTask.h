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

#ifndef _U2_SEQUENCE_QUALITY_TRIM_TASK_H_
#define _U2_SEQUENCE_QUALITY_TRIM_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

namespace U2 {

class DNAChromatogramObject;
class U2SequenceObject;

class SequenceQualityTrimTaskSettings {
public:
    SequenceQualityTrimTaskSettings();

    U2SequenceObject *sequenceObject;

    int qualityTreshold;
    int minSequenceLength;
    bool trimBothEnds;
};

class SequenceQualityTrimTask : public Task {
    Q_OBJECT
public:
    SequenceQualityTrimTask(const SequenceQualityTrimTaskSettings &settings);
    ~SequenceQualityTrimTask();

    U2SequenceObject *takeTrimmedSequence();

private:
    void run();
    QString generateReport() const;

    void cloneObjects();
    void cloneSequence();
    void cloneChromatogram();
    void restoreRelation();
    U2Region trimSequence();
    void trimChromatogram(const U2Region &regionToCrop);

    const SequenceQualityTrimTaskSettings settings;
    U2SequenceObject *trimmedSequenceObject;
    DNAChromatogramObject *trimmedChromatogramObject;
    bool isFilteredOut;
};

}   // namespace U2

#endif // _U2_SEQUENCE_QUALITY_TRIM_TASK_H_
