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

#include <U2Core/AppContext.h>
#include <U2Core/GHints.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/SelectionUtils.h>

#include "QDDocument.h"
#include "QDDocumentFormat.h"
#include "QDSceneIOTasks.h"
#include "QueryViewController.h"

namespace U2 {

 //QDGobject
 //////////////////////////////////////////////////////////////////////////
const GObjectType QDGObject::TYPE("query-obj");

GObject* QDGObject::clone(const U2DbiRef& , U2OpStatus& , const QVariantMap &hints) const {
    GHintsDefaultImpl gHints(getGHintsMap());
    gHints.setAll(hints);

    QDGObject* copy = new QDGObject(getGObjectName(), serializedScene, gHints.getMap());
    return copy;
}

;

//Format
//////////////////////////////////////////////////////////////////////////
QDDocFormat::QDDocFormat(QObject* p)
: TextDocumentFormat(p, DocumentFormatId("QueryDocFormat"), DocumentFormatFlags_W1, QStringList(QUERY_SCHEME_EXTENSION)) {
    formatName = tr("Query Schema");
    formatDescription = tr("QDDoc is a format used for creating/editing/storing/retrieving"
        "query schema with the text file");
    supportedObjectTypes += QDGObject::TYPE;
}

Document* QDDocFormat::createNewLoadedDocument(IOAdapterFactory* io, const GUrl &url, U2OpStatus& os, const QVariantMap& fs ) {
    Document* d = DocumentFormat::createNewLoadedDocument(io, url, os, fs);
    GObject* o = new QDGObject(tr("Query Schema"), "");
    d->addObject(o);
    return d;
}

#define BUFF_SIZE 1024
Document* QDDocFormat::loadTextDocument(IOAdapter* io, const U2DbiRef& targetDb, const QVariantMap& hints, U2OpStatus& os) {
    QByteArray  rawData;
    QByteArray block(BUFF_SIZE, '\0');
    int blockLen = 0;
    while ((blockLen = io->readBlock(block.data(), BUFF_SIZE)) > 0) {
        rawData.append(block.data(), blockLen);
        os.setProgress(io->getProgress());
    }
    CHECK_EXT(!io->hasError(), os.setError(io->errorString()), NULL);

    if (checkRawData(rawData).score != FormatDetection_Matched) {
        os.setError(tr("Invalid header. %1 expected").arg(QDDocument::HEADER_LINE));
        rawData.clear();
        return NULL;
    }

    QList<GObject*> objects;
    QString data = QString::fromUtf8(rawData.data(), rawData.size());
    objects.append(new QDGObject(tr("Query Schema"), data));
    return new Document(this, io->getFactory(), io->getURL(), targetDb, objects, hints);
}

void QDDocFormat::storeDocument(Document* d, IOAdapter* io, U2OpStatus& ) {
    assert(d->getDocumentFormat() == this);
    assert(d->getObjects().size() ==1);

    QDGObject* wo = qobject_cast<QDGObject*>(d->getObjects().first());
    assert(wo && wo->getScene());

    QByteArray rawData = QDSceneSerializer::scene2doc(wo->getScene())->toByteArray();
    int nWritten = 0;
    int nTotal = rawData.size();
    while(nWritten < nTotal) {
        int d = io->writeBlock(rawData.data() + nWritten, nTotal - nWritten);
        assert(d > 0);
        nWritten+= d;
    }
    wo->setSceneRawData(rawData);
}

FormatCheckResult QDDocFormat::checkRawTextData( const QByteArray& rawData, const GUrl&) const {
    const QString& data = rawData;
    if(QDDocument::isHeaderLine(data.trimmed())) {
        return FormatDetection_Matched;
    }
    return FormatDetection_NotMatched;
}

//Factory
//////////////////////////////////////////////////////////////////////////
const GObjectViewFactoryId QDViewFactory::ID("query-view-factory");

bool QDViewFactory::canCreateView( const MultiGSelection& multiSelection ) {
    foreach(GObject* go, SelectionUtils::findObjects(QDGObject::TYPE, &multiSelection, UOF_LoadedOnly)) {
        if (!qobject_cast<QDGObject*>(go)->getScene()) {
            return true;
        }
    }
    return false;
}

Task* QDViewFactory::createViewTask( const MultiGSelection& multiSelection, bool single /* = false*/ ) {
    QSet<Document*> documents = SelectionUtils::findDocumentsWithObjects(QDGObject::TYPE, &multiSelection, UOF_LoadedAndUnloaded, true);
    if (documents.size() == 0) {
        return NULL;
    }
    Task* result = (single || documents.size() == 1) ? NULL : new Task(tr("Open multiple views"), TaskFlag_NoRun);
    foreach(Document* d, documents) {
        Task* t = new OpenQDViewTask(d);
        if (result == NULL) {
            return t;
        }
        result->addSubTask(t);
    }
    return result;
}

//OpenViewTask
//////////////////////////////////////////////////////////////////////////
OpenQDViewTask::OpenQDViewTask( Document* doc ) : ObjectViewTask(QDViewFactory::ID), document(doc){
    if (!doc->isLoaded()) {
        documentsToLoad.append(doc);
    } else {
        foreach(GObject* go, doc->findGObjectByType(QDGObject::TYPE)) {
            selectedObjects.append(go) ;
        }
        assert(!selectedObjects.isEmpty());
    }
}

void OpenQDViewTask::open() {
    if (stateInfo.hasError()) {
        return;
    }
    if (!documentsToLoad.isEmpty()) {
        foreach(GObject* go, documentsToLoad.first()->findGObjectByType(QDGObject::TYPE)) {
            selectedObjects.append(go);
        }
    }
    foreach(QPointer<GObject> po, selectedObjects) {
        QDGObject* o = qobject_cast<QDGObject*>(po);
        assert(o && !o->getScene());
        QueryViewController* view = new QueryViewController;
        view->loadScene(o->getSceneRawData());
        view->setSchemeUri(document->getURL().getURLString());
        AppContext::getMainWindow()->getMDIManager()->addMDIWindow(view);
        AppContext::getMainWindow()->getMDIManager()->activateWindow(view);
    }
}
}//namespace
