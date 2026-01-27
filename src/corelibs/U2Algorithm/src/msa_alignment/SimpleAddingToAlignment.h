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

#include <U2Algorithm/AlignSequencesToAlignmentTaskSettings.h>
#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>

#include <U2Core/Msa.h>
#include <U2Core/MsaUtils.h>

namespace U2 {

class SimpleAddToAlignmentTask : public AbstractAlignmentTask {
    Q_OBJECT
public:
    SimpleAddToAlignmentTask(const AlignSequencesToAlignmentTaskSettings& settings);
    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

    ReportResult report() override;

private:
    AlignSequencesToAlignmentTaskSettings settings;
    QMap<QString, int> sequencePositions;

    Msa inputMsa;
};

class BestPositionFindTask : public Task {
    Q_OBJECT
public:
    BestPositionFindTask(const Msa& alignment, const U2EntityRef& sequenceRef, const QString& sequenceId, int referenceRowId);
    void run() override;

    int getPosition() const;
    const QString& getSequenceId() const;

private:
    const Msa& inputMsa;
    U2EntityRef sequenceRef;
    QString sequenceId;
    int bestPosition;
    int referenceRowId;
};

class SimpleAddToAlignmentTaskFactory : public AbstractAlignmentTaskFactory {
public:
    AbstractAlignmentTask* getTaskInstance(AbstractAlignmentTaskSettings* _settings) const override;
};

class SimpleAddToAlignmentAlgorithm : public AlignmentAlgorithm {
public:
    SimpleAddToAlignmentAlgorithm();
};

}  // namespace U2
