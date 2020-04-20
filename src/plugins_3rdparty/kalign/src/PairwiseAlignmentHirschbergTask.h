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

#ifndef _U2_PAIRWISE_ALIGNMENT_HIRSCHBERG_TASK_H_
#define _U2_PAIRWISE_ALIGNMENT_HIRSCHBERG_TASK_H_

#include <U2Algorithm/PairwiseAlignmentTask.h>

#include <U2Core/DNATranslation.h>
#include <U2Core/MultipleSequenceAlignment.h>

namespace U2 {

class DNAAlphabet;
class KalignGObjectRunFromSchemaTask;
class KalignTask;
class Project;

class PairwiseAlignmentHirschbergTaskSettings : public PairwiseAlignmentTaskSettings {
public:
    PairwiseAlignmentHirschbergTaskSettings(const PairwiseAlignmentTaskSettings &s);
    virtual ~PairwiseAlignmentHirschbergTaskSettings();

    virtual bool convertCustomSettings();

    //all settings except translationTable must be set up through customSettings and then must be converted by convertCustomSettings().
    int gapOpen;
    int gapExtd;
    int gapTerm;
    int bonusScore;

    static const QString PA_H_GAP_OPEN;
    static const QString PA_H_GAP_EXTD;
    static const QString PA_H_GAP_TERM;
    static const QString PA_H_BONUS_SCORE;
    static const QString PA_H_REALIZATION_NAME;
    static const QString PA_H_DEFAULT_RESULT_FILE_NAME;
};


class PairwiseAlignmentHirschbergTask : public PairwiseAlignmentTask
{
public:
    PairwiseAlignmentHirschbergTask(PairwiseAlignmentHirschbergTaskSettings* _settings);
    ~PairwiseAlignmentHirschbergTask();

    virtual QList<Task*> onSubTaskFinished(Task *subTask);
    virtual ReportResult report();

protected:
    void changeGivenUrlIfDocumentExists(QString & givenUrl, const Project * curProject);

protected:
    PairwiseAlignmentHirschbergTaskSettings* settings;
    KalignTask* kalignSubTask;
    KalignGObjectRunFromSchemaTask* workflowKalignSubTask;
    MultipleSequenceAlignment ma;
    const DNAAlphabet* alphabet;
};

}   //namespace

#endif // _U2_PAIRWISE_ALIGNMENT_HIRSCHBERG_TASK_H_
