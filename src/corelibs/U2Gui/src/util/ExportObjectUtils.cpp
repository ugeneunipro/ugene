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

#include <QApplication>
#include <QMessageBox>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/L10n.h>
#include <U2Core/GObjectUtils.h>

#include <U2Gui/ExportAnnotations2CSVTask.h>
#include <U2Gui/ExportAnnotationsDialog.h>
#include <U2Gui/ExportDocumentDialogController.h>
#include <U2Gui/LastUsedDirHelper.h>

#include "ExportObjectUtils.h"

namespace U2 {

void ExportObjectUtils::exportAnnotations(const AnnotationTableObject *aObj, const GUrl &dstUrl) {
    QList<Annotation *> annotations = aObj->getAnnotations(); // copy for further modification
    if (annotations.isEmpty()) {
        QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Export annotations..."),
            QObject::tr("Selected object doesn't have annotations"));
        return;
    }

    QString fileName = GUrlUtils::rollFileName(dstUrl.dirPath() + "/" + dstUrl.baseFileName()
        + "_annotations.csv", DocumentUtils::getNewDocFileNameExcludesHint());

    QObjectScopedPointer<ExportAnnotationsDialog> d = new ExportAnnotationsDialog(fileName, QApplication::activeWindow());

    const int dialogResult = d->exec();
    CHECK(!d.isNull(), );

    if (QDialog::Accepted != dialogResult) {
        return;
    }

    // TODO: lock documents or use shared-data objects
    // same as in ADVExportContext::sl_saveSelectedAnnotations()
    qStableSort(annotations.begin(), annotations.end(), Annotation::annotationLessThan);

    // run task
    Task * t = NULL;
    if (ExportAnnotationsDialog::CSV_FORMAT_ID == d->fileFormat()) {
        QString seqName;
        QByteArray seqData;
        Project* project = AppContext::getProject();
        if (project != NULL) {
            QList<GObjectRelation> rels = aObj->findRelatedObjectsByRole(ObjectRole_Sequence);
            if (!rels.isEmpty()) {
                const GObjectRelation& rel = rels.first();
                seqName = rel.ref.objName;
                Document* seqDoc = project->findDocumentByURL(rel.ref.docUrl);
                if (seqDoc != NULL && seqDoc->isLoaded()) {
                    GObject* obj = seqDoc->findGObjectByName(rel.ref.objName);
                    if (obj != NULL && obj->getGObjectType() == GObjectTypes::SEQUENCE) {
                        U2SequenceObject* seqObj = qobject_cast<U2SequenceObject*>(obj);
                        U2OpStatusImpl os;
                        seqData = seqObj->getWholeSequenceData(os);
                        CHECK_OP_EXT(os, QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), os.getError()), );
                    }
                }
            }
        }
        t = new ExportAnnotations2CSVTask(annotations, seqData, seqName, NULL, d->exportSequence(), d->exportSequenceNames(), d->filePath());
    } else {
        t = saveAnnotationsTask(d->filePath(), d->fileFormat(), annotations, d->addToProject());
    }
    SAFE_POINT(NULL != t, "Invalid task detected!",);

    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

void ExportObjectUtils::exportObject2Document(GObject *object, const QString &url,
    bool tracePath)
{
    if (NULL == object || object->isUnloaded()) {
        return;
    }
    QObjectScopedPointer<ExportDocumentDialogController> dialog = new ExportDocumentDialogController(object, QApplication::activeWindow(), url);
    export2Document(dialog.data(), tracePath);
}

void ExportObjectUtils::export2Document(const QObjectScopedPointer<ExportDocumentDialogController> &dialog, bool tracePath) {
    const int result = dialog->exec();
    CHECK(!dialog.isNull(), );

    if (result != QDialog::Accepted) {
        return;
    }

    if (tracePath) {
        LastUsedDirHelper h;
        h.url = dialog->getDocumentURL();
    }
    QString dstUrl = dialog->getDocumentURL();
    if (dstUrl.isEmpty()) {
        return;
    }

    Project *project = AppContext::getProject();
    if (NULL != project && project->findDocumentByURL(dstUrl)) {
        QMessageBox::critical(QApplication::activeWindow(), QObject::tr("Error"),
            QObject::tr("Document with the same URL is added to the project.\n"
            "Remove it from the project first."));
        return;
    }
    bool addToProject = dialog->getAddToProjectFlag();

    IOAdapterRegistry *ioar = AppContext::getIOAdapterRegistry();
    SAFE_POINT(NULL != ioar, "Invalid I/O environment!",);
    IOAdapterFactory *iof = ioar->getIOAdapterFactoryById(IOAdapterUtils::url2io(dstUrl));
    CHECK_EXT(NULL != iof,
        coreLog.error(QObject::tr("Unable to create I/O factory for ") + dstUrl),);
    DocumentFormatRegistry *dfr =  AppContext::getDocumentFormatRegistry();
    DocumentFormatId formatId = dialog->getDocumentFormatId();
    DocumentFormat *df = dfr->getFormatById(formatId);
    CHECK_EXT(NULL != df,
        coreLog.error(QObject::tr("Unknown document format I/O factory: ") + formatId),);

    U2OpStatusImpl os;
    Document *srcDoc = dialog->getSourceDoc();
    Document *dstDoc = NULL;
    if (NULL == srcDoc) {
        dstDoc = df->createNewLoadedDocument(iof, dstUrl, os);
        dstDoc->addObject(dialog->getSourceObject());
    } else {
        dstDoc = srcDoc->getSimpleCopy(df, iof, dstUrl);
    }

    SaveDocFlags flags = SaveDocFlags(SaveDoc_Roll) | SaveDoc_DestroyButDontUnload;
    if (addToProject) {
        flags |= SaveDoc_OpenAfter;
    }
    SaveDocumentTask *t = new SaveDocumentTask(dstDoc, iof, dstUrl, flags);
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
}

Task * ExportObjectUtils::saveAnnotationsTask(const QString &filepath, const DocumentFormatId &format, const QList<Annotation *> &annList, bool addToProject) {
    SaveDocFlags fl(SaveDoc_Roll);
    if (addToProject) {
        fl |= SaveDoc_OpenAfter;
    } else {
        fl |= SaveDoc_DestroyAfter;
    }
    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(
        IOAdapterUtils::url2io(filepath));
    CHECK_EXT(NULL != iof,
        coreLog.error(QObject::tr("Unable to create I/O factory for ") + filepath), NULL);
    DocumentFormat *df = AppContext::getDocumentFormatRegistry()->getFormatById(format);
    CHECK_EXT(NULL != df,
        coreLog.error(QObject::tr("Unknown document format I/O factory: ") + format), NULL);
    U2OpStatus2Log os;
    QVariantMap hints;
    hints.insert(DocumentReadingMode_DontMakeUniqueNames, QVariant(true));
    Document *doc = df->createNewLoadedDocument(iof, filepath, os, hints);
    CHECK_OP(os, NULL);

    // object and annotations will be deleted when savedoc task will delete doc
    QMap<U2DataId, AnnotationTableObject *> annTables;
    QMap<AnnotationTableObject *, QMap<QString, QList<SharedAnnotationData> > > annTable2Anns;

    foreach (Annotation *a, annList) {
        const AnnotationTableObject *parentObject = a->getGObject();
        if (parentObject != NULL) {
            U2DataId objId = parentObject->getRootFeatureId();

            AnnotationTableObject *att = NULL;
            if (annTables.contains(objId)) {
                att = annTables.value(objId);
            } else {
                const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
                CHECK_OP(os, NULL);
                att = new AnnotationTableObject(parentObject->getGObjectName(), dbiRef);
                foreach (GObjectRelation objRel, parentObject->getObjectRelations()) {
                    att->addObjectRelation(objRel);
                }
                doc->addObject(att);
                annTables.insert(objId, att);
            }
            annTable2Anns[att][a->getGroup()->getName()].append(a->getData());
        }
    }

    foreach (AnnotationTableObject *ato, annTable2Anns.keys()) {
        foreach (const QString &groupName, annTable2Anns[ato].keys()) {
            ato->addAnnotations(annTable2Anns[ato][groupName], groupName);
        }
    }

    foreach (AnnotationTableObject *att, annTables.values()) {
        att->setModified(false);
    }
    return new SaveDocumentTask(doc, fl, DocumentUtils::getNewDocFileNameExcludesHint());
}

} // namespace U2
