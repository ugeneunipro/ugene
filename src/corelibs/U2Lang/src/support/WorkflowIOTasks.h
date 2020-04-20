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

#ifndef _U2_WORKFLOW_IO_TASK_H_
#define _U2_WORKFLOW_IO_TASK_H_

#include <U2Core/Task.h>
#include <U2Lang/ActorModel.h>
#include <U2Lang/Schema.h>

#include <QPointer>

class QDomDocument;

namespace U2 {
using namespace Workflow;

class U2LANG_EXPORT LoadWorkflowTask : public Task {
    Q_OBJECT
public:
    LoadWorkflowTask(Schema* schema, Metadata* meta, const QString& url);
    virtual void run();
    Task::ReportResult report();
    Schema* getSchema() const {return schema;}
    QString getURL() const {return url;}
    Metadata * getMetadata() {return meta;}
    QMap<ActorId, ActorId> getRemapping() {return remap;}

    enum FileFormat {
        HR,
        XML,
        UNKNOWN
    };
    static FileFormat detectFormat(const QString & rawData);

protected:
    const QString url;
    Schema* schema;
    Metadata* meta;
    QString rawData;
    FileFormat format;
    QMap<ActorId, ActorId> remap;

}; // LoadWorkflowTask

class U2LANG_EXPORT SaveWorkflowTask : public Task {
    Q_OBJECT
public:
    SaveWorkflowTask(Schema* schema, const Metadata& meta, bool copyMode);
    Task::ReportResult report();
    virtual void run();

private:
    QString rawData;
    QString url;

}; // SaveWorkflowTask

} //namespace
#endif
