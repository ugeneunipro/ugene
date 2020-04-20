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

#include <QDomElement>

#include <U2Core/LoadRemoteDocumentTask.h>

#include "LoadRemoteDocumentTests.h"

#define DB_ATTR "database"
#define DOC_ID_ATTR "document_id"
#define EXPECTED_DOC_ATTR "expected_document"

namespace U2 {

    //////////////////////////////////////////////////////////////////////////

    void GTest_LoadRemoteDocumentTask::init(XMLTestFormat *tf, const QDomElement& el){
        Q_UNUSED(tf);

        dbName.clear();
        docId.clear();
        expectedDoc.clear();
        t = NULL;

        QString tmp = el.attribute(DB_ATTR);

        if(tmp.isEmpty()){
            failMissingValue(DB_ATTR);
            return;
        }
        dbName = tmp;

        tmp = el.attribute(DOC_ID_ATTR);
        if(tmp.isEmpty()){
            failMissingValue(DB_ATTR);
            return;
        }
        docId = tmp;

        tmp = el.attribute(EXPECTED_DOC_ATTR);
        if(tmp.isEmpty()){
            failMissingValue(EXPECTED_DOC_ATTR);
            return;
        }
        expectedDoc = env->getVar("COMMON_DATA_DIR") + "/" + tmp;
    }

    void GTest_LoadRemoteDocumentTask::prepare(){
        RemoteDBRegistry& registry = RemoteDBRegistry::getRemoteDBRegistry();
        const QList<QString> dataBases = registry.getDBs();
        bool checked = false;
        foreach(const QString& db, dataBases) {
            if(dbName == db){
                checked = true;
            }
        }
        if(!checked){
            setError(tr("Given database name %1 not present in database registry").arg(dbName));
            return;
        }
        t = new LoadRemoteDocumentTask(docId, dbName);
        addSubTask(t);
    }

    Task::ReportResult GTest_LoadRemoteDocumentTask::report(){
        if(t != NULL){
            if(!t->hasError()){
                QFile expectedFile(expectedDoc), actualFile(t->getLocalUrl());
                expectedFile.open(QIODevice::ReadOnly), actualFile.open(QIODevice::ReadOnly);
                QByteArray expectedContent(expectedFile.readAll()), actualContent(actualFile.readAll());
                if(expectedContent != actualContent){
                    stateInfo.setError(GTest::tr("File %1 content not equal with expected").arg(t->getLocalUrl()));
                }
                expectedFile.close(), actualFile.close();
            }
            return ReportResult_Finished;
        }
        return ReportResult_Finished;
    }

    QList<XMLTestFactory*> LoadRemoteDocumentTests::createTestFactories(){
        QList<XMLTestFactory*> res;
        res.append(GTest_LoadRemoteDocumentTask::createFactory());
        return res;
    }

} //namespace
