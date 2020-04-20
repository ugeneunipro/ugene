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

#ifndef _U2_PEAK2GENE_SUPPORT_TASK_
#define _U2_PEAK2GENE_SUPPORT_TASK_

#include <U2Core/AnnotationData.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/Task.h>

#include <U2Lang/DbiDataStorage.h>

#include "Peak2GeneSettings.h"

namespace U2 {

class AnnotationTableObject;
class Document;
class LoadDocumentTask;
class SaveDocumentTask;

class Peak2GeneTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    Peak2GeneTask(const Peak2GeneSettings& settings, Workflow::DbiDataStorage *storage, const QList<Workflow::SharedDbiDataHandler>& treatAnn);
    virtual ~Peak2GeneTask();

    virtual void prepare();
    virtual QList<Task*> onSubTaskFinished(Task* subTask);
    virtual void run();
    virtual void cleanup();

    const Peak2GeneSettings & getSettings() const;

    AnnotationTableObject * getGenes() const;
    AnnotationTableObject * getPeaks() const;
    const QString & getGenesUrl() const;
    const QString & getPeaksUrl() const;

private:
    Peak2GeneSettings settings;
    QString workingDir;

    Workflow::DbiDataStorage *storage;
    QList<Workflow::SharedDbiDataHandler> treatAnn;

    Document *treatDoc;

    AnnotationTableObject *genesAto;
    AnnotationTableObject *peaksAto;

    QString genesUrl;
    QString peaksUrl;

    SaveDocumentTask *treatTask;

    ExternalToolRunTask *etTask;

    static const QString BASE_DIR_NAME;
    static const QString BASE_SUBDIR_NAME;
    static const QString TREAT_NAME;

private:
    Document* createDoc(const QList<Workflow::SharedDbiDataHandler> &annData, const QString& name);
};

} // U2

#endif // _U2_PEAK2GENE_SUPPORT_TASK_
