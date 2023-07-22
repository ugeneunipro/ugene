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

#include <U2Algorithm/PhyTreeGenerator.h>
#include <U2Algorithm/PhyTreeGeneratorTask.h>

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/PhyTreeObject.h>

#include <U2View/CreatePhyTreeDialogController.h>

#include "PhyMLSupport.h"
#include "utils/ExportTasks.h"

namespace U2 {

class LoadDocumentTask;

class PhyMLPrepareDataForCalculation : public Task {
    Q_OBJECT
public:
    PhyMLPrepareDataForCalculation(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& s, const QString& url);
    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    const QString& getInputFileUrl() const {
        return inputFileForPhyML;
    }

private:
    const MultipleSequenceAlignment& ma;
    CreatePhyTreeSettings settings;
    QString tmpDirUrl;
    SaveAlignmentTask* saveDocumentTask = nullptr;
    QString inputFileForPhyML;
};

class PhyMLSupportTask;
class PhyMLLogParser : public ExternalToolLogParser {
    Q_OBJECT
public:
    PhyMLLogParser(PhyMLSupportTask* parentTask, int sequencesNumber);
    int getProgress() override;
    void parseOutput(const QString& partOfLog) override;
    void parseErrOutput(const QString& partOfLog) override;

private:
    PhyMLSupportTask* parentTask;
    QString lastLine;
    QString lastErrLine;
    int processedBranches;
    int sequencesNumber;
};

class PhyMLGetCalculatedTreeTask : public Task {
    Q_OBJECT
public:
    PhyMLGetCalculatedTreeTask(const QString& url);
    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    PhyTreeObject* getPhyObject() {
        return phyObject;
    }

private:
    QString baseFileName;
    LoadDocumentTask* loadTmpDocumentTask;
    PhyTreeObject* phyObject;
};

class PhyMLSupportTask : public PhyTreeGeneratorTask {
    Q_OBJECT
public:
    PhyMLSupportTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& s);
    void prepare() override;
    Task::ReportResult report() override;
    void onExternalToolFailed(const QString& err);
    QList<Task*> onSubTaskFinished(Task* subTask) override;

    static const QString TMP_FILE_NAME;
    static const QString RESULT_TREE_EXT;

private:
    QString tmpDirUrl;
    QString tmpPhylipFile;
    PhyMLPrepareDataForCalculation* prepareDataTask;
    ExternalToolRunTask* phyMlTask;
    PhyMLGetCalculatedTreeTask* getTreeTask;
    int sequencesNumber;
};

}  // namespace U2
