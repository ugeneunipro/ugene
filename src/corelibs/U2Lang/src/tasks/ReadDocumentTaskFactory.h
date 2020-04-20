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

#ifndef _U2_READ_DOCUMENT_TASK_FACTORY_
#define _U2_READ_DOCUMENT_TASK_FACTORY_

#include <U2Core/Task.h>

#include <U2Lang/DbiDataHandler.h>

namespace U2 {
namespace Workflow {

class WorkflowContext;

class U2LANG_EXPORT ReadFactories {
public:
    static const QString READ_ASSEMBLY;
};

class U2LANG_EXPORT ReadDocumentTask : public Task {
    Q_OBJECT
public:
    ReadDocumentTask(const QString &url, const QString &name, const QString &datasetName, TaskFlags f);
    virtual ~ReadDocumentTask();

    virtual QList<SharedDbiDataHandler> takeResult();
    virtual const QString & getUrl() const;
    virtual const QString & getDatasetName() const;

    const QStringList & getProducedFiles() const;

protected:
    QList<SharedDbiDataHandler> result;
    QString url;
    QString datasetName;
    QStringList producedFiles;
};

class U2LANG_EXPORT ReadDocumentTaskFactory {
public:
    ReadDocumentTaskFactory(const QString &id);
    virtual ~ReadDocumentTaskFactory();
    virtual ReadDocumentTask *createTask(const QString &url, const QVariantMap &hints, WorkflowContext *ctx) = 0;
    QString getId() const;
private:
    QString id;
};

} // Workflow
} // U2

#endif // _U2_READ_DOCUMENT_TASK_FACTORY_
