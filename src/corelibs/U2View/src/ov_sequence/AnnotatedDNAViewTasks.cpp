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

#include "AnnotatedDNAViewTasks.h"
#include "AnnotatedDNAView.h"
#include "AnnotatedDNAViewFactory.h"
#include "AnnotatedDNAViewState.h"

#include "GSequenceLineView.h"

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectUtils.h>

#include <U2Core/Log.h>
#include <U2Core/L10n.h>
#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <QApplication>
#include <QMessageBox>
#include <QSet>

static const int SEQ_OBJS_PER_VIEW = 10;

namespace U2 {

//////////////////////////////////////////////////////////////////////////
/// open new view

//opens a single view for all sequence object in the list of sequence objects related to the objects in the list
OpenAnnotatedDNAViewTask::OpenAnnotatedDNAViewTask( const QList<GObject *> &objects )
: ObjectViewTask( AnnotatedDNAViewFactory::ID )
{
    //  remember only sequence objects -> other added automatically
    //  load all objects
    QList<Document *> docsToLoadSet;
    QSet<GObject *> refsAdded;
    int displayedSeqCount = 0;
    foreach ( GObject *obj, objects ) {
        // check the limit count of simultaneously displayed sequences
        if ( SEQ_OBJS_PER_VIEW <= displayedSeqCount ) {
            break;
        }
        Document *doc = obj->getDocument( );
        uiLog.trace( "Object to open sequence view: '" + obj->getGObjectName( ) + "'" );
        if ( !doc->isLoaded( ) ) {
            docsToLoadSet.append( doc );
        }
        populateSeqObjectRefs( obj, docsToLoadSet, refsAdded );
        if ( GObjectUtils::hasType( obj, GObjectTypes::SEQUENCE ) ) {
            ++displayedSeqCount;
        }
    }
    foreach ( Document *doc, docsToLoadSet ) {
        uiLog.trace( "Document to load: '" + doc->getURLString( ) + "'" );
        documentsToLoad.append( doc );
    }
}

void OpenAnnotatedDNAViewTask::populateSeqObjectRefs( GObject *object, QList<Document *> &docsToLoad,
    QSet<GObject *> &refsAdded )
{
    const QList<GObject *> allSequenceObjects = GObjectUtils::findAllObjects( UOF_LoadedAndUnloaded,
        GObjectTypes::SEQUENCE );

    Document *doc = object->getDocument( );
    SAFE_POINT( NULL != doc, "Invalid document detected!", );

    QList<GObject *> objWithSeqRelation;
    if ( GObjectUtils::hasType( object, GObjectTypes::SEQUENCE ) ) {
        QList<GObject *> allAnnotations = GObjectUtils::findAllObjects( UOF_LoadedAndUnloaded,
            GObjectTypes::ANNOTATION_TABLE );
        QList<GObject *> annotations = GObjectUtils::findObjectsRelatedToObjectByRole( object,
            GObjectTypes::ANNOTATION_TABLE, ObjectRole_Sequence, allAnnotations,
            UOF_LoadedAndUnloaded );
        foreach ( GObject* ao, annotations ) {
            objWithSeqRelation.append( ao );
        }

        GObjectReference reference( doc->getURLString( ), object->getGObjectName( ), GObjectTypes::SEQUENCE );
        reference.entityRef = object->getEntityRef( );
        sequenceObjectRefs.append( reference );
        refsAdded.insert( object );
        if ( objWithSeqRelation.isEmpty( ) ) {
            return;
        }
    }
    //look for sequence object using relations
    objWithSeqRelation.append( GObjectUtils::selectRelations( object, GObjectTypes::SEQUENCE,
        ObjectRole_Sequence, allSequenceObjects, UOF_LoadedAndUnloaded ) );

    foreach ( GObject *robj, objWithSeqRelation ) {
        if ( ( !GObjectUtils::hasType( robj, GObjectTypes::SEQUENCE ) && !GObjectUtils::hasType(robj, GObjectTypes::ANNOTATION_TABLE ) )
            || refsAdded.contains( robj ) )
        {
            continue;
        }
        Document *rdoc = robj->getDocument( );
        if ( !rdoc->isLoaded( ) ) {
            docsToLoad.append( rdoc );
        }
        refsAdded.insert( robj );
        GObjectReference reference( rdoc->getURLString( ), robj->getGObjectName( ), GObjectTypes::SEQUENCE );
        reference.entityRef = robj->getEntityRef( );
        sequenceObjectRefs.append( reference );
    }
}

static QString deriveViewName(const QList<U2SequenceObject*>& seqObjects) {
    QString viewName;
    if (seqObjects.size() > 1) {
        bool singleDocument = true;
        Document* doc = seqObjects.first()->getDocument();
        foreach(GObject* obj, seqObjects) {
            if (doc != obj->getDocument()) {
                singleDocument = false;
                break;
            }
        }
        if (singleDocument) {
            viewName = GObjectViewUtils::genUniqueViewName(doc->getName());
        } else {
            viewName = GObjectViewUtils::genUniqueViewName(OpenAnnotatedDNAViewTask::tr("Sequences"));
        }
    } else {
        GObject* obj = seqObjects.first();
        viewName = GObjectViewUtils::genUniqueViewName(obj->getDocument(), obj);
    }
    return viewName;
}

static const int MAX_SEQS_COUNT = 5;
static void showAlphabetWarning(const QList<U2SequenceObject*> &seqObjects) {
    QStringList rawSeqs;
    foreach (U2SequenceObject *seqObj, seqObjects) {
        const DNAAlphabet *alphabet = seqObj->getAlphabet();
        if (DNAAlphabet_RAW == alphabet->getType()) {
            rawSeqs << seqObj->getSequenceName();
        }
    }

    if (!rawSeqs.isEmpty()) {
        QString warning(QObject::tr("The following sequences contain unrecognizable symbols:\n"));
        int seqCount = 0;
        foreach (const QString &seqName, rawSeqs) {
            warning.append(seqName).append("\n");
            seqCount++;
            if (MAX_SEQS_COUNT == seqCount) {
                break;
            }
        }
        if (seqCount < rawSeqs.size()) {
            warning.append(QObject::tr("and others...\n"));
        }

        warning.append(QObject::tr("Some algorithms will not work for these sequences."));
        QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Warning"), warning);
    }

    QSet<Document*> docs;
    foreach(const GObject* obj, seqObjects){
        docs.insert(obj->getDocument());
    }
    foreach(const Document* doc, docs){
        if(!doc->getGHintsMap()[ProjectLoaderHint_MergeMode_DifferentAlphabets].toString().isEmpty()){
            QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Warning"),
               doc->getGHintsMap()[ProjectLoaderHint_MergeMode_DifferentAlphabets].toString());
        }
    }
}

void OpenAnnotatedDNAViewTask::open() {
    if (stateInfo.hasError() || sequenceObjectRefs.isEmpty()) {
        return;
    }
    QList<U2SequenceObject*> seqObjects;
    QList<GObject*> allSequenceObjects = GObjectUtils::findAllObjects(UOF_LoadedOnly, GObjectTypes::SEQUENCE);
    foreach(const GObjectReference& r, sequenceObjectRefs) {
        GObject* obj = GObjectUtils::selectObjectByReference(r, allSequenceObjects, UOF_LoadedOnly);
        U2SequenceObject* seqObj = qobject_cast<U2SequenceObject*>(obj);
        if (seqObj!=NULL) {
            U2OpStatusImpl status;
            seqObj->isValidDbiObject(status);
            CHECK_OP_EXT(status, stateInfo.setError(tr("Error reading sequence object from dbi! URL: '%1'', name: '%2', error: %3").arg(r.docUrl).arg(r.objName).arg(status.getError())), );

            seqObjects.append(seqObj);
            if (seqObjects.size() > SEQ_OBJS_PER_VIEW) {
                uiLog.details(tr("Maximum number of objects per view reached: %1").arg(SEQ_OBJS_PER_VIEW));
                break;
            }
        } else {
            uiLog.details(tr("Sequence object not available! URL %1, name %2").arg(r.docUrl).arg(r.objName));
        }
    }
    if (seqObjects.isEmpty()) { //object was removed asynchronously with the task
        if(0 == stateInfo.cancelFlag) {
            stateInfo.setError(tr("No sequence objects found"));
        }
        return;
    }
    QString viewName = deriveViewName(seqObjects);
    AnnotatedDNAView* v = new AnnotatedDNAView(viewName, seqObjects);
    GObjectViewWindow* w = new GObjectViewWindow(v, viewName, false);
    MWMDIManager* mdiManager =  AppContext::getMainWindow()->getMDIManager();
    mdiManager->addMDIWindow(w);

    showAlphabetWarning(seqObjects);
}

void OpenAnnotatedDNAViewTask::updateTitle(AnnotatedDNAView* v) {
    const QString& oldViewName = v->getName();
    GObjectViewWindow* w = GObjectViewUtils::findViewByName(oldViewName);
    if (w != NULL) {
        QString newViewName = deriveViewName(v->getSequenceObjectsWithContexts());
        v->setName(newViewName);
        w->setWindowTitle(newViewName);
    }
}

//////////////////////////////////////////////////////////////////////////
// open view from state
static QSet<Document*> selectDocuments(Project* p, const QList<GObjectReference>& refs, U2OpStatus& os) {
    QSet<Document*> res;
    foreach(const GObjectReference& r, refs) {
        Document* doc = p->findDocumentByURL(r.docUrl);
        if (doc != NULL) {
            res.insert(doc);
        } else {
            doc = ObjectViewTask::createDocumentAndAddToProject(r.docUrl, p, os);
            if (doc) {
                res.insert(doc);
            }
        }
    }
    return res;
}

OpenSavedAnnotatedDNAViewTask::OpenSavedAnnotatedDNAViewTask(const QString& viewName, const QVariantMap& stateData)
: ObjectViewTask(AnnotatedDNAViewFactory::ID, viewName, stateData)
{
    AnnotatedDNAViewState state(stateData);
    QList<GObjectReference> refs = state.getSequenceObjects();
    if (refs.isEmpty()) {
        stateIsIllegal = true;
        stateInfo.setError(ObjectViewTask::tr("No sequence info found!"));
        return;
    }
    foreach(const GObjectReference& ref, refs) {
        Document* doc = AppContext::getProject()->findDocumentByURL(ref.docUrl);
        if (doc == NULL) {
            doc = createDocumentAndAddToProject(ref.docUrl, AppContext::getProject(), stateInfo);
            if (!doc) {
                stateIsIllegal = true;
                stateInfo.setError(L10N::errorDocumentNotFound(ref.docUrl));
                return;
            }
        }

        if (!doc->isLoaded()) {
            documentsToLoad.append(doc);
        }
    }

    QSet<Document*> adocs = selectDocuments(AppContext::getProject(), state.getAnnotationObjects(), stateInfo);
    foreach(Document* adoc, adocs) {
        if (!adoc->isLoaded()) {
            documentsToLoad.append(adoc);
        }
    }
}

void OpenSavedAnnotatedDNAViewTask::open() {
    if (stateInfo.hasError()) {
        return;
    }
    AnnotatedDNAViewState state(stateData);
    QList<U2SequenceObject*> sequenceObjects;
    foreach(const GObjectReference& ref, state.getSequenceObjects()) {
        Document* doc = AppContext::getProject()->findDocumentByURL(ref.docUrl);
        if (doc == NULL) {
            stateIsIllegal = true;
            stateInfo.setError(L10N::errorDocumentNotFound(ref.docUrl));
            return;
        }
        GObject* obj = NULL;
        if (doc->isDatabaseConnection() && ref.entityRef.isValid()) {
            obj = doc->getObjectById(ref.entityRef.entityId);
        } else {
            obj = doc->findGObjectByName(ref.objName);
        }
        if (obj == NULL || obj->getGObjectType() != GObjectTypes::SEQUENCE) {
            stateIsIllegal = true;
            stateInfo.setError(tr("DNA sequence object not found: %1").arg(ref.objName));
            return;
        }
        U2SequenceObject* dnaObj= qobject_cast<U2SequenceObject*>(obj);
        sequenceObjects.append(dnaObj);
    }
    AnnotatedDNAView* v = new AnnotatedDNAView(viewName, sequenceObjects);
    GObjectViewWindow* w = new GObjectViewWindow(v, viewName, true);
    MWMDIManager* mdiManager =  AppContext::getMainWindow()->getMDIManager();
    mdiManager->addMDIWindow(w);
    v->updateState(state);
}


//////////////////////////////////////////////////////////////////////////
// update
UpdateAnnotatedDNAViewTask::UpdateAnnotatedDNAViewTask(AnnotatedDNAView* v, const QString& stateName, const QVariantMap& stateData)
: ObjectViewTask(v, stateName, stateData)
{
}

void UpdateAnnotatedDNAViewTask::update() {
    if (view.isNull()) {
        return; //view was closed;
    }

    AnnotatedDNAView* aview = qobject_cast<AnnotatedDNAView*>(view.data());
    assert(aview!=NULL);

    AnnotatedDNAViewState state(stateData);
    aview->updateState(state);
}

} // namespace
