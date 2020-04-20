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

#include <QFile>
#include <QTextStream>
#include <QDomDocument>

#include <U2Core/Log.h>
#include <U2Core/L10n.h>
#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Lang/HRSchemaSerializer.h>

#include "WorkflowIOTasks.h"

namespace U2 {
using namespace Workflow;

/************************************
 * LoadWorkflowTask
 ************************************/
LoadWorkflowTask::LoadWorkflowTask(Schema* s, Workflow::Metadata* m, const QString& u):
Task(tr("Loading workflow"), TaskFlag_None),
    url(u), schema(s), meta(m) {
    assert(schema != NULL);
}

void LoadWorkflowTask::run() {
    ioLog.details(tr("Loading workflow from file: %1").arg(url));

    QFile file(url);
    if (!file.open(QIODevice::ReadOnly)) {
        stateInfo.setError(L10N::errorOpeningFileRead(url));
        return;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    rawData = in.readAll();
    format = detectFormat(rawData);
    if(format == UNKNOWN) {
        setError(tr("Undefined format: plain text or xml expected"));
        return;
    }
}

Task::ReportResult LoadWorkflowTask::report() {
    if(stateInfo.hasError()) {
        return ReportResult_Finished;
    }

    QString err;
    if(format == HR) {
        err = HRSchemaSerializer::string2Schema(rawData, schema, meta, &remap);
    } else if(format == XML) {
        setError(tr("Sorry! XML workflow format is obsolete and not supported. You can create new workflow in GUI mode"
                    " or write it by yourself. Check our documentation for details!"));
        return ReportResult_Finished;
    } else {
        // check in constructor
        assert(false);
    }

    if(!err.isEmpty()) {
        setError(err);
        schema->reset();
        if(meta) {
            meta->reset();
        }
        return ReportResult_Finished;
    }
    if(meta) {
        meta->url = url;
    }
    return ReportResult_Finished;
}

LoadWorkflowTask::FileFormat LoadWorkflowTask::detectFormat(const QString & rawData) {
    if(HRSchemaSerializer::isHeaderLine(rawData.trimmed())) {
        return HR;
    } else if(rawData.trimmed().startsWith("<!DOCTYPE GB2WORKFLOW>")) {
        return XML;
    } else {
        return UNKNOWN;
    }
}

/************************************
 * SaveWorkflowTask
 ************************************/
SaveWorkflowTask::SaveWorkflowTask(Schema* schema, const Metadata& meta, bool copyMode) :
Task(tr("Save workflow task"), TaskFlag_None), url(meta.url) {
    assert(schema != NULL);
    rawData = HRSchemaSerializer::schema2String(*schema, &meta, copyMode);
}

void SaveWorkflowTask::run() {
    if(hasError() || isCanceled()) {
        return;
    }

    QFile file(url);
    if(!file.open(QIODevice::WriteOnly)) {
        setError(L10N::errorOpeningFileWrite(url));
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << rawData;
}

Task::ReportResult SaveWorkflowTask::report() {
    return ReportResult_Finished;
}

}//namespace
