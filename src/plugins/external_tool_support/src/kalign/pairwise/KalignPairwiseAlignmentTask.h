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

#pragma once

#include <U2Algorithm/PairwiseAlignmentTask.h>

#include <U2Core/DNATranslation.h>
#include <U2Core/MultipleSequenceAlignment.h>

namespace U2 {

class Project;
class Kalign3SupportTask;

class KalignPairwiseAlignmentTaskFactory : public AbstractAlignmentTaskFactory {
public:
    AbstractAlignmentTask* getTaskInstance(AbstractAlignmentTaskSettings* _settings) const override;
};

class KalignPairwiseAlignmentTaskSettings : public PairwiseAlignmentTaskSettings {
public:
    KalignPairwiseAlignmentTaskSettings(const PairwiseAlignmentTaskSettings& s);

    bool convertCustomSettings() override;

    // all settings except translationTable must be set up through customSettings and then must be converted by convertCustomSettings().
    int gapOpen;
    int gapExtd;
    int gapTerm;

    static const QString PA_H_GAP_OPEN;
    static const QString PA_H_GAP_EXTD;
    static const QString PA_H_GAP_TERM;
    static const QString PA_H_BONUS_SCORE;
    static const QString PA_H_REALIZATION_NAME;
};

class KalignPairwiseAlignmentTask : public PairwiseAlignmentTask {
public:
    KalignPairwiseAlignmentTask(KalignPairwiseAlignmentTaskSettings* _settings);
    ~KalignPairwiseAlignmentTask() override;

    QList<Task*> onSubTaskFinished(Task* subTask) override;
    ReportResult report() override;

protected:
    void changeGivenUrlIfDocumentExists(QString& givenUrl, const Project* curProject);

protected:
    KalignPairwiseAlignmentTaskSettings* settings;
    Kalign3SupportTask* kalignSubTask;
    MultipleSequenceAlignment ma;
    const DNAAlphabet* alphabet;
};

}  // namespace U2

