/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "MaEditorTasks.h"

#include <QSet>

#include <U2Algorithm/MsaConsensusAlgorithm.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/MsaObject.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TextObject.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceUtils.h>
#include <U2Core/UnloadedObject.h>

#include "MaEditorFactory.h"
#include "MaEditorState.h"
#include "MsaEditor.h"
#include "MsaEditorConsensusArea.h"

namespace U2 {

/* TRANSLATOR U2::MSAEditor */
/* TRANSLATOR U2::ObjectViewTask */

//////////////////////////////////////////////////////////////////////////
/// open new view

OpenMaEditorTask::OpenMaEditorTask(MsaObject* _obj, GObjectViewFactoryId fid, GObjectType type)
    : ObjectViewTask(fid),
      type(type),
      maObject(_obj) {
    assert(!maObject.isNull());
}

OpenMaEditorTask::OpenMaEditorTask(UnloadedObject* _obj, GObjectViewFactoryId fid, GObjectType type)
    : ObjectViewTask(fid),
      type(type),
      unloadedReference(_obj) {
    assert(_obj->getLoadedObjectType() == type);
    documentsToLoad.append(_obj->getDocument());
}

OpenMaEditorTask::OpenMaEditorTask(Document* doc, GObjectViewFactoryId fid, GObjectType type)
    : ObjectViewTask(fid),
      type(type),
      maObject(nullptr) {
    assert(!doc->isLoaded());
    documentsToLoad.append(doc);
}

void OpenMaEditorTask::open() {
    if (stateInfo.hasError() || (maObject.isNull() && documentsToLoad.isEmpty())) {
        return;
    }
    if (maObject.isNull()) {
        Document* doc = documentsToLoad.first();
        if (!doc) {
            stateInfo.setError(tr("Document removed from project"));
            return;
        }
        if (unloadedReference.isValid()) {
            GObject* obj = GObjectUtils::selectObjectByReference(unloadedReference, UOF_LoadedOnly);
            if (obj != nullptr && obj->getGObjectType() == type) {
                maObject = qobject_cast<MsaObject*>(obj);
            }
        } else {
            QList<GObject*> objects = doc->findGObjectByType(type, UOF_LoadedAndUnloaded);
            maObject = objects.isEmpty() ? nullptr : qobject_cast<MsaObject*>(objects.first());
        }
        if (maObject.isNull()) {
            stateInfo.setError(tr("Multiple alignment object not found"));
            return;
        }
    }
    viewName = GObjectViewUtils::genUniqueViewName(maObject->getDocument(), maObject);
    uiLog.details(tr("Opening MSA editor for object: %1").arg(maObject->getGObjectName()));

    MaEditor* maEditor = getEditor(viewName, maObject);
    CHECK_OP(stateInfo, );

    auto objectViewWindow = new GObjectViewWindow(maEditor, viewName, false);
    MWMDIManager* mdiManager = AppContext::getMainWindow()->getMDIManager();
    mdiManager->addMDIWindow(objectViewWindow);
}

void OpenMaEditorTask::updateTitle(MsaEditor* msaEd) {
    const QString& oldViewName = msaEd->getName();
    GObjectViewWindow* w = GObjectViewUtils::findViewByName(oldViewName);
    if (w != nullptr) {
        MsaObject* msaObject = msaEd->getMaObject();
        QString newViewName = GObjectViewUtils::genUniqueViewName(msaObject->getDocument(), msaObject);
        msaEd->setName(newViewName);
        w->setWindowTitle(newViewName);
    }
}

OpenMsaEditorTask::OpenMsaEditorTask(MsaObject* obj)
    : OpenMaEditorTask(obj, MsaEditorFactory::ID, GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) {
}

OpenMsaEditorTask::OpenMsaEditorTask(UnloadedObject* obj)
    : OpenMaEditorTask(obj, MsaEditorFactory::ID, GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) {
}

OpenMsaEditorTask::OpenMsaEditorTask(Document* doc)
    : OpenMaEditorTask(doc, MsaEditorFactory::ID, GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT) {
}

MaEditor* OpenMsaEditorTask::getEditor(const QString& viewName, GObject* obj) {
    return MsaEditorFactory().getEditor(viewName, obj, stateInfo);
}

OpenMcaEditorTask::OpenMcaEditorTask(MsaObject* obj)
    : OpenMaEditorTask(obj, McaEditorFactory::ID, GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT) {
}

OpenMcaEditorTask::OpenMcaEditorTask(UnloadedObject* obj)
    : OpenMaEditorTask(obj, McaEditorFactory::ID, GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT) {
}

OpenMcaEditorTask::OpenMcaEditorTask(Document* doc)
    : OpenMaEditorTask(doc, McaEditorFactory::ID, GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT) {
}

MaEditor* OpenMcaEditorTask::getEditor(const QString& viewName, GObject* obj) {
    QList<GObjectRelation> relations = obj->findRelatedObjectsByRole(ObjectRole_ReferenceSequence);
    SAFE_POINT(relations.size() <= 1, "Wrong amount of reference sequences", nullptr);
    return McaEditorFactory().getEditor(viewName, obj, stateInfo);
}

//////////////////////////////////////////////////////////////////////////
// open view from state

OpenSavedMaEditorTask::OpenSavedMaEditorTask(GObjectType type, MaEditorFactory* factory, const QString& viewName, const QVariantMap& stateData)
    : ObjectViewTask(factory->getId(), viewName, stateData),
      type(type),
      factory(factory) {
    MaEditorState state(stateData);
    GObjectReference ref = state.getMaObjectRef();
    Document* doc = AppContext::getProject()->findDocumentByURL(ref.docUrl);
    if (doc == nullptr) {
        doc = createDocumentAndAddToProject(ref.docUrl, AppContext::getProject(), stateInfo);
        CHECK_OP_EXT(stateInfo, stateIsIllegal = true, );
    }
    if (!doc->isLoaded()) {
        documentsToLoad.append(doc);
    }
}

void OpenSavedMaEditorTask::open() {
    CHECK_OP(stateInfo, );

    MaEditorState state(stateData);
    GObjectReference ref = state.getMaObjectRef();
    Document* doc = AppContext::getProject()->findDocumentByURL(ref.docUrl);
    if (doc == nullptr) {
        stateIsIllegal = true;
        stateInfo.setError(L10N::errorDocumentNotFound(ref.docUrl));
        return;
    }
    GObject* obj = nullptr;
    // TODO: this methods does not work! UGENE-4904
    //        obj = doc->findGObjectByName(ref.objName);
    QList<GObject*> objs = doc->findGObjectByType(ref.objType);
    foreach (GObject* curObj, objs) {
        if (curObj->getGObjectName() == ref.objName) {
            obj = curObj;
            break;
        }
    }

    if (obj == nullptr || obj->getGObjectType() != type) {
        stateIsIllegal = true;
        stateInfo.setError(tr("Alignment object not found: %1").arg(ref.objName));
        return;
    }
    auto maObject = qobject_cast<MsaObject*>(obj);
    assert(maObject != nullptr);

    MaEditor* maEditor = factory->getEditor(viewName, maObject, stateInfo);
    CHECK_OP(stateInfo, );
    SAFE_POINT(maEditor != nullptr, "MaEditor is null!", );

    auto w = new GObjectViewWindow(maEditor, viewName, true);
    MWMDIManager* mdiManager = AppContext::getMainWindow()->getMDIManager();
    mdiManager->addMDIWindow(w);

    updateRanges(stateData, maEditor);
}

void OpenSavedMaEditorTask::updateRanges(const QVariantMap& stateData, MaEditor* ctx) {
    Q_UNUSED(ctx);
    MaEditorState state(stateData);

    QFont f = state.getFont();
    if (!f.isCopyOf(QFont())) {
        ctx->setFont(f);
    }

    ctx->setFirstVisiblePosSeq(state.getFirstPos(), state.getFirstSeq());
    ctx->setZoomFactor(state.getZoomFactor());
}

//////////////////////////////////////////////////////////////////////////
// update
UpdateMaEditorTask::UpdateMaEditorTask(GObjectViewController* v, const QString& stateName, const QVariantMap& stateData)
    : ObjectViewTask(v, stateName, stateData) {
}

void UpdateMaEditorTask::update() {
    if (view.isNull() || (view->getFactoryId() != MsaEditorFactory::ID && view->getFactoryId() != McaEditorFactory::ID)) {
        return;  // view was closed;
    }

    auto maView = qobject_cast<MaEditor*>(view.data());
    SAFE_POINT_EXT(maView != nullptr, setError("MaEditor is NULL"), );

    OpenSavedMaEditorTask::updateRanges(stateData, maView);
}

ExportMaConsensusTask::ExportMaConsensusTask(const ExportMaConsensusTaskSettings& s)
    : DocumentProviderTask(tr("Export consensus"),
                           (TaskFlags(TaskFlag_NoRun) | TaskFlag_FailOnSubtaskError | TaskFlag_CancelOnSubtaskCancel)),
      settings(s),
      extractConsensus(nullptr) {
    setVerboseLogMode(true);
    SAFE_POINT_EXT(s.ma != nullptr, setError("Given msa pointer is NULL"), );
}

void ExportMaConsensusTask::prepare() {
    extractConsensus = new ExtractConsensusTask(settings.keepGaps, settings.ma, settings.algorithm);
    addSubTask(extractConsensus);
}

const QString& ExportMaConsensusTask::getConsensusUrl() const {
    return settings.url;
}

QList<Task*> ExportMaConsensusTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> taskList;
    if (subTask != extractConsensus || isCanceled() || hasError()) {
        return taskList;
    }
    Document* consensusDocument = createDocument();
    CHECK_OP(stateInfo, taskList);
    auto saveTask = new SaveDocumentTask(consensusDocument, consensusDocument->getIOAdapterFactory(), consensusDocument->getURL());
    taskList << saveTask;

    Project* proj = AppContext::getProject();
    if (proj == nullptr || proj->findDocumentByURL(consensusDocument->getURL()) == nullptr) {
        saveTask->addFlag(SaveDoc_OpenAfter);
        if (settings.format == BaseDocumentFormats::RAW_DNA_SEQUENCE || settings.format == BaseDocumentFormats::PLAIN_TEXT) {
            // Instruct UGENE to load the file content as a RAW sequence.
            QVariantMap hints;
            hints[ProjectLoaderHint_DocumentFormat] = BaseDocumentFormats::RAW_DNA_SEQUENCE;
            saveTask->setOpenDocumentWithProjectHints(hints);
        }
    }
    return taskList;
}

Document* ExportMaConsensusTask::createDocument() {
    filteredConsensus = extractConsensus->getExtractedConsensus();
    CHECK_EXT(!filteredConsensus.isEmpty(), setError("Consensus is empty!"), nullptr);
    QString fullPath = GUrlUtils::prepareFileLocation(settings.url, stateInfo);
    CHECK_OP(stateInfo, nullptr);
    GUrl url(fullPath);

    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(settings.url));
    DocumentFormat* df = AppContext::getDocumentFormatRegistry()->getFormatById(settings.format);
    CHECK_EXT(df, setError("Document format is NULL!"), nullptr);
    GObject* obj = nullptr;
    QScopedPointer<Document> doc(df->createNewLoadedDocument(iof, fullPath, stateInfo));
    CHECK_OP(stateInfo, nullptr);
    if (df->getFormatId() == BaseDocumentFormats::PLAIN_TEXT) {
        obj = TextObject::createInstance(filteredConsensus, settings.name, doc->getDbiRef(), stateInfo);
    } else {
        DNASequence dna(settings.name, filteredConsensus);
        U2EntityRef ref = U2SequenceUtils::import(stateInfo, doc->getDbiRef(), U2ObjectDbi::ROOT_FOLDER, dna);
        obj = new U2SequenceObject(dna.getName(), ref);
    }
    CHECK_OP(stateInfo, nullptr);
    doc->addObject(obj);
    return doc.take();
}

ExtractConsensusTask::ExtractConsensusTask(bool keepGaps_, MaEditor* ma_, MsaConsensusAlgorithm* algorithm_)
    : Task(tr("Extract consensus"), TaskFlags(TaskFlag_None)),
      keepGaps(keepGaps_),
      ma(ma_),
      algorithm(algorithm_) {
    setVerboseLogMode(true);
    SAFE_POINT_EXT(ma != nullptr, setError("Given ma pointer is NULL"), );
}

ExtractConsensusTask::~ExtractConsensusTask() {
    delete algorithm;
}

void ExtractConsensusTask::run() {
    CHECK(ma->getLineWidget(0), );
    CHECK(ma->getLineWidget(0)->getConsensusArea(), );
    CHECK(ma->getLineWidget(0)->getConsensusArea()->getConsensusCache(), );

    const Msa alignment = ma->getMaObject()->getAlignment()->getCopy();
    for (int i = 0, n = alignment->getLength(); i < n; i++) {
        if (stateInfo.isCoR()) {
            return;
        }
        int count = 0;
        int nSeq = alignment->getRowCount();
        SAFE_POINT(0 != nSeq, "No sequences in alignment", );

        QChar c = algorithm->getConsensusCharAndScore(alignment, i, count);
        if (c == MsaConsensusAlgorithm::INVALID_CONS_CHAR) {
            c = U2Msa::GAP_CHAR;
        }
        if (c != U2Msa::GAP_CHAR || keepGaps) {
            filteredConsensus.append(c.toLatin1());
        }
    }
}

const QByteArray& ExtractConsensusTask::getExtractedConsensus() const {
    return filteredConsensus;
}

ExportMaConsensusTaskSettings::ExportMaConsensusTaskSettings()
    : keepGaps(true),
      ma(nullptr),
      format(BaseDocumentFormats::PLAIN_TEXT),
      algorithm(nullptr) {
}

}  // namespace U2
