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

#ifndef _U2_MRBAYES_TASK_H
#define _U2_MRBAYES_TASK_H

#include "MrBayesSupport.h"
#include "utils/ExportTasks.h"

#include <U2View/CreatePhyTreeDialogController.h>
#include <U2Algorithm/PhyTreeGenerator.h>
#include <U2Algorithm/PhyTreeGeneratorTask.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/PhyTreeObject.h>

namespace U2 {

class LoadDocumentTask;

class MrBayesPrepareDataForCalculation :public Task{
    Q_OBJECT
public:
    MrBayesPrepareDataForCalculation(const MultipleSequenceAlignment& _ma, const CreatePhyTreeSettings& s, const QString& url);
    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);
    QString getInputFileUrl() {return inputFileForMrBayes;}
private:
    const MultipleSequenceAlignment&           ma;
    CreatePhyTreeSettings       settings;
    QString                     tmpDirUrl;
    SaveAlignmentTask*          saveDocumentTask;
    QString                     inputFileForMrBayes;
};

class MrBayesLogParser : public ExternalToolLogParser {
public:
    MrBayesLogParser(int _nchains);
    int getProgress();
    void parseOutput(const QString& partOfLog);
    void parseErrOutput(const QString& partOfLog);
private:
    QString lastLine;
    QString lastErrLine;
    int nchains;
    bool isMCMCRunning;
    int curProgress;

};

class MrBayesGetCalculatedTreeTask: public Task{
    Q_OBJECT
public:
    MrBayesGetCalculatedTreeTask(const QString& url);
    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);
    PhyTreeObject* getPhyObject(){return phyObject;}
private:
    QString                     baseFileName;
    LoadDocumentTask*           loadTmpDocumentTask;
    PhyTreeObject*              phyObject;

};

class MrBayesSupportTask : public PhyTreeGeneratorTask{
    Q_OBJECT
public:
    MrBayesSupportTask(const MultipleSequenceAlignment& _ma, const CreatePhyTreeSettings& s);
    void prepare();
    Task::ReportResult report();
    QList<Task*> onSubTaskFinished(Task* subTask);
private:
    QString                             tmpDirUrl;
    QString                             tmpNexusFile;
    MrBayesPrepareDataForCalculation*   prepareDataTask;
    ExternalToolRunTask*                mrBayesTask;
    MrBayesGetCalculatedTreeTask*       getTreeTask;
};



}//namespace

#endif // _U2_MRBAYES_TASK_H
