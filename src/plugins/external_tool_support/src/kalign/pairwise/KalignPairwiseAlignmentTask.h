/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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
#include <U2Core/Msa.h>

#include "../KalignSupportTask.h"

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

    bool isValid() const override;

    Kalign3Settings kalign3Settings;

    static const QString GAP_OPEN_PENALTY_KEY;
    static const QString GAP_EXTENSION_PENALTY_KEY;
    static const QString TERMINAL_GAP_EXTENSION_PENALTY_KEY;
};

class KalignPairwiseAlignmentTask : public PairwiseAlignmentTask {
public:
    KalignPairwiseAlignmentTask(KalignPairwiseAlignmentTaskSettings* _settings);
    ~KalignPairwiseAlignmentTask() override;

    QList<Task*> onSubTaskFinished(Task* subTask) override;
    ReportResult report() override;

protected:
    void makeUniqueUrl(QString& givenUrl, const Project* curProject);

protected:
    KalignPairwiseAlignmentTaskSettings* settings = nullptr;
    Kalign3SupportTask* kalignSubTask = nullptr;
    Msa ma;
    const DNAAlphabet* alphabet = nullptr;
};

}  // namespace U2
