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

#include <U2Algorithm/AlignSequencesToAlignmentTaskSettings.h>
#include <U2Algorithm/AlignmentAlgorithmsRegistry.h>

#include <U2Core/DNASequence.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultipleAlignment.h>

#include "MAFFTSupportTask.h"
#include "MafftAddToAlignmentTask.h"

namespace U2 {

class U2UseCommonUserModStep;

class MafftAddToAlignmentTask : public AbstractAlignmentTask {
    Q_OBJECT
public:
    MafftAddToAlignmentTask(const AlignSequencesToAlignmentTaskSettings& settings);

    void prepare() override;
    void run() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

    ReportResult report() override;

private:
    bool useMemsaveOption() const;

    AlignSequencesToAlignmentTaskSettings settings;

    MultipleAlignment inputMsa;

    QSharedPointer<Document> tmpDoc;
    QString url;
    MAFFTLogParser* logParser;

    SaveDocumentTask* saveSequencesDocumentTask;
    SaveMSA2SequencesTask* saveAlignmentDocumentTask;
    ExternalToolRunTask* mafftTask;
    LoadDocumentTask* loadTmpDocumentTask;
    U2UseCommonUserModStep* modStep;
    QMap<QString, QString> uniqueIdsToNames;

    QString tmpDirUrl;
    QString resultFilePath;
};

class MafftAddToAlignmentTaskFactory : public AbstractAlignmentTaskFactory {
public:
    virtual AbstractAlignmentTask* getTaskInstance(AbstractAlignmentTaskSettings* _settings) const;
};

class MafftAlignSequencesToAlignmentAlgorithm : public AlignmentAlgorithm {
public:
    MafftAlignSequencesToAlignmentAlgorithm(const AlignmentAlgorithmType& type);

    bool isAlgorithmAvailable() const override;
};

}  // namespace U2
