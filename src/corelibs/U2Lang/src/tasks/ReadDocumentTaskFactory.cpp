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

#include "ReadDocumentTaskFactory.h"

namespace U2 {
namespace Workflow {

const QString ReadFactories::READ_ASSEMBLY("read-assembly-task-factory");

ReadDocumentTask::ReadDocumentTask(const QString &_url, const QString &name, const QString &_datasetName, TaskFlags f)
: Task(name, f), url(_url), datasetName(_datasetName)
{

}

ReadDocumentTask::~ReadDocumentTask() {
    result.clear();
}

QList<SharedDbiDataHandler> ReadDocumentTask::takeResult() {
    QList<SharedDbiDataHandler> ret = result;
    result.clear();

    return ret;
}

const QString & ReadDocumentTask::getUrl() const {
    return url;
}

const QString & ReadDocumentTask::getDatasetName() const {
    return datasetName;
}

const QStringList & ReadDocumentTask::getProducedFiles() const {
    return producedFiles;
}

ReadDocumentTaskFactory::ReadDocumentTaskFactory(const QString &_id)
: id(_id)
{

}

ReadDocumentTaskFactory::~ReadDocumentTaskFactory() {

}

QString ReadDocumentTaskFactory::getId() const {
    return id;
}

} // Workflow
} // U2
