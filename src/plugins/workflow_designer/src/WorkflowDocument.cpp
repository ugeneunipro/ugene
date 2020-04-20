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

#include <U2Core/AppContext.h>
#include <U2Core/GHints.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/SelectionUtils.h>
#include <U2Core/Task.h>
#include <U2Core/U2DbiUtils.h>

#include <U2Gui/MainWindow.h>

#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/WorkflowIOTasks.h>
#include <U2Lang/WorkflowUtils.h>

#include "WorkflowDocument.h"
#include "WorkflowViewController.h"

/* TRANSLATOR U2::IOAdapter */

namespace U2 {
using namespace WorkflowSerialize;

const GObjectType WorkflowGObject::TYPE("workflow-obj");
const GObjectViewFactoryId WorkflowViewFactory::ID("workflow-view-factory");

const DocumentFormatId WorkflowDocFormat::FORMAT_ID = "WorkflowDocFormat";

bool WorkflowGObject::isTreeItemModified () const {
    if (view) {
        return view->getScene()->isModified();
    }
    return GObject::isItemModified();
}

void WorkflowGObject::setView(WorkflowView* _view) {
    view = _view;
}

void WorkflowGObject::setSceneRawData(const QString & data) {
    assert(view != NULL);
    assert(!view->getScene()->isModified());
    serializedScene = data;
}

GObject* WorkflowGObject::clone(const U2DbiRef&, U2OpStatus&, const QVariantMap &hints) const {
    GHintsDefaultImpl gHints(getGHintsMap());
    gHints.setAll(hints);

    WorkflowGObject* copy = new WorkflowGObject(getGObjectName(), serializedScene, gHints.getMap());
    assert(!view);
    return copy;
}

//////////////////////////////////////////////////////////////////////////
/// Workflow document format

WorkflowDocFormat::WorkflowDocFormat(QObject* p)
: TextDocumentFormat(p, WorkflowDocFormat::FORMAT_ID, DocumentFormatFlags_W1, QStringList(WorkflowUtils::WD_FILE_EXTENSIONS) << WorkflowUtils::WD_XML_FORMAT_EXTENSION) {
    formatName = tr("Workflow");
    supportedObjectTypes += WorkflowGObject::TYPE;
    formatDescription = tr("WorkflowDoc is a format used for creating/editing/storing/retrieving"
        "workflow with the text file");
}

Document* WorkflowDocFormat::createNewLoadedDocument(IOAdapterFactory* io, const GUrl &url, U2OpStatus& os, const QVariantMap& fs) {
    Document* d = DocumentFormat::createNewLoadedDocument(io, url, os, fs);
    GObject* o = new WorkflowGObject(tr("Workflow"), "");
    d->addObject(o);
    return d;
}

#define BUFF_SIZE 1024

Document* WorkflowDocFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& targetDb, const QVariantMap& hints, U2OpStatus& os) {
    QByteArray  rawData;
    QByteArray block(BUFF_SIZE, '\0');
    int blockLen = 0;
    while ((blockLen = io->readBlock(block.data(), BUFF_SIZE)) > 0) {
        rawData.append(block.data(), blockLen);
        os.setProgress(io->getProgress());
    }
    CHECK_EXT(!io->hasError(), os.setError(io->errorString()), NULL);

    if (checkRawData(rawData).score != FormatDetection_Matched) {
        os.setError(tr("Invalid header. %1 expected").arg(Constants::HEADER_LINE));
        rawData.clear();
        return NULL;
    }
    //todo: check file-readonly status?

    QList<GObject*> objects;
    QString data = QString::fromUtf8(rawData.data(), rawData.size());
    objects.append(new WorkflowGObject(tr("Workflow"), data));
    return new Document(this, io->getFactory(), io->getURL(), targetDb, objects, hints);
}

void WorkflowDocFormat::storeDocument( Document* d, IOAdapter* io, U2OpStatus& ) {
    assert(d->getDocumentFormat() == this);
    assert(d->getObjects().size() ==1);

    WorkflowGObject* wo = qobject_cast<WorkflowGObject*>(d->getObjects().first());
    assert(wo && wo->getView());

    const Metadata &meta = wo->getView()->getMeta();
    const Schema *schema = wo->getView()->getSchema();
    QByteArray rawData = HRSchemaSerializer::schema2String(*schema, &meta).toUtf8();
    int nWritten = 0;
    int nTotal = rawData.size();
    while(nWritten < nTotal) {
        int d = io->writeBlock(rawData.data() + nWritten, nTotal - nWritten);
        assert(d > 0);
        nWritten+= d;
    }
    wo->getView()->getScene()->setModified(false);
    wo->setSceneRawData(rawData);
}

FormatCheckResult WorkflowDocFormat::checkRawTextData(const QByteArray& data, const GUrl&) const {
    LoadWorkflowTask::FileFormat format = LoadWorkflowTask::detectFormat(data);
    bool ok  = format == LoadWorkflowTask::HR || format == LoadWorkflowTask::XML;
    return ok ? FormatDetection_Matched  : FormatDetection_NotMatched;
}

bool WorkflowViewFactory::canCreateView(const MultiGSelection& multiSelection) {
    foreach(GObject* go, SelectionUtils::findObjects(WorkflowGObject::TYPE, &multiSelection, UOF_LoadedOnly)) {
        if (!qobject_cast<WorkflowGObject*>(go)->getView()) {
            return true;
        }
    }
    return false;
}

Task* WorkflowViewFactory::createViewTask(const MultiGSelection& multiSelection, bool single) {
    QSet<Document*> documents = SelectionUtils::findDocumentsWithObjects(WorkflowGObject::TYPE, &multiSelection, UOF_LoadedAndUnloaded, true);
    if (documents.size() == 0) {
        return NULL;
    }
    Task* result = (single || documents.size() == 1) ? NULL : new Task(tr("Open multiple views"), TaskFlag_NoRun);
    foreach(Document* d, documents) {
        Task* t = new OpenWorkflowViewTask(d);
        if (result == NULL) {
            return t;
        }
        result->addSubTask(t);
    }
    return result;
}

OpenWorkflowViewTask::OpenWorkflowViewTask(Document* doc)
: ObjectViewTask(WorkflowViewFactory::ID)
{
    if (!doc->isLoaded()) {
        documentsToLoad.append(doc);
    } else {
        foreach(GObject* go, doc->findGObjectByType(WorkflowGObject::TYPE)) {
            selectedObjects.append(go) ;
        }
        assert(!selectedObjects.isEmpty());
    }
}

void OpenWorkflowViewTask::open() {
    if (stateInfo.hasError()) {
        return;
    }
    if (!documentsToLoad.isEmpty()) {
        foreach(GObject* go, documentsToLoad.first()->findGObjectByType(WorkflowGObject::TYPE)) {
            selectedObjects.append(go) ;
        }
    }
    foreach(QPointer<GObject> po, selectedObjects) {
        WorkflowGObject* o = qobject_cast<WorkflowGObject*>(po);
        assert(o && !o->getView());
        WorkflowView::openWD(o);
    }
}

}//namespace
