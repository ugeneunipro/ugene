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
#include <U2Core/AppSettings.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2VariantDbi.h>
#include <U2Core/UserApplicationsSettings.h>

#include "RenameChromosomeInVariationTask.h"

namespace U2 {

RenameChromosomeInVariationTask::RenameChromosomeInVariationTask(const QList<GObject *> &objects,
                                                                 const QStringList &prefixesToReplace,
                                                                 const QString &prefixReplaceWith) :
    Task(tr("Rename task"), TaskFlag_None),
    objects(objects),
    prefixesToReplace(prefixesToReplace),
    prefixReplaceWith(prefixReplaceWith)
{
    SAFE_POINT_EXT(!prefixesToReplace.isEmpty(), setError("Prefixes to replace are not defined"), );
}

void RenameChromosomeInVariationTask::run() {
    CHECK(!objects.isEmpty(), );

    DbiConnection connection(objects.first()->getEntityRef().dbiRef, stateInfo);
    CHECK_OP(stateInfo, );
    SAFE_POINT_EXT(NULL != connection.dbi, setError(L10N::nullPointerError("dbi")), );
    U2VariantDbi *variantDbi = connection.dbi->getVariantDbi();
    SAFE_POINT_EXT(NULL != variantDbi, setError(L10N::nullPointerError("variant dbi")), );

    foreach (GObject *object, objects) {
        U2OpStatusImpl os;
        U2VariantTrack variantTrack = variantDbi->getVariantTrack(object->getEntityRef().entityId, stateInfo);
        if (replaceSequenceName(variantTrack)) {
            variantDbi->updateVariantTrack(variantTrack, os);
        }
    }
}

bool RenameChromosomeInVariationTask::replaceSequenceName(U2VariantTrack &variantTrack) const {
    foreach (const QString &prefix, prefixesToReplace) {
        if (variantTrack.sequenceName.startsWith(prefix)) {
            variantTrack.sequenceName.replace(0, prefix.length(), prefixReplaceWith);
            return true;
        }
    }
    return false;
}

RenameChromosomeInVariationFileTask::RenameChromosomeInVariationFileTask(const QString &srcFileUrl,
                                                                         const QString &dstFileUrl,
                                                                         const QStringList &prefixesToReplace,
                                                                         const QString &prefixReplaceWith) :
    Task(tr("Rename chomosome name file with variations"), TaskFlags_NR_FOSE_COSC),
    srcFileUrl(srcFileUrl),
    dstFileUrl(dstFileUrl),
    prefixesToReplace(prefixesToReplace),
    prefixReplaceWith(prefixReplaceWith),
    loadTask(NULL),
    renameTask(NULL),
    saveTask(NULL)
{
    SAFE_POINT_EXT(!srcFileUrl.isEmpty(), L10N::badArgument("input file URL"), );
    SAFE_POINT_EXT(!dstFileUrl.isEmpty(), L10N::badArgument("input file URL"), );
    SAFE_POINT_EXT(!prefixesToReplace.isEmpty(), setError("Prefixes to replace are not defined"), );
}

QString RenameChromosomeInVariationFileTask::getDstFileUrl() const {
    return saveTask->getURL().getURLString();
}

void RenameChromosomeInVariationFileTask::prepare() {
    DocumentFormat *format = getFormat();
    CHECK_OP(stateInfo, );
    SAFE_POINT_EXT(NULL != format, setError(L10N::nullPointerError("document format")), );

    IOAdapterFactory *ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(srcFileUrl));
    CHECK_EXT(NULL != ioAdapterFactory, setError(L10N::nullPointerError("IO adapter factory")), );

    loadTask = new LoadDocumentTask(format, srcFileUrl, ioAdapterFactory);
    addSubTask(loadTask);
}

QList<Task *> RenameChromosomeInVariationFileTask::onSubTaskFinished(Task *subTask) {
    QList<Task *> res;
    CHECK_OP(stateInfo, res);

    if (loadTask == subTask) {
        res << initRenameTask();
    }

    if (renameTask == subTask) {
        res << initSaveTask();
    }

    return res;
}

Task * RenameChromosomeInVariationFileTask::initRenameTask() {
    renameTask = new RenameChromosomeInVariationTask(getVariantTrackObjects(), prefixesToReplace, prefixReplaceWith);
    return renameTask;
}

Task * RenameChromosomeInVariationFileTask::initSaveTask() {
    IOAdapterFactory *ioAdapterFactory = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    saveTask = new SaveDocumentTask(loadTask->getDocument(), ioAdapterFactory, GUrl(dstFileUrl), SaveDoc_Roll);
    return saveTask;
}

QList<GObject *> RenameChromosomeInVariationFileTask::getVariantTrackObjects() {
    QList<GObject *> objects;

    Document *document = loadTask->getDocument();
    SAFE_POINT_EXT(NULL != document, setError(L10N::nullPointerError("loaded document")), objects);

    const QList<GObject *> variantTrackObjects = document->findGObjectByType(GObjectTypes::VARIANT_TRACK, UOF_LoadedAndUnloaded);
    CHECK_EXT(!variantTrackObjects.isEmpty(), setError(tr("File doesn't contains variant tracks")), objects);

    return variantTrackObjects;
}

DocumentFormat * RenameChromosomeInVariationFileTask::getFormat() {
    DocumentFormatConstraints constraints;
    constraints.supportedObjectTypes << GObjectTypes::VARIANT_TRACK;

    QList<FormatDetectionResult> formatDetectionResults = DocumentUtils::detectFormat(srcFileUrl);
    CHECK_EXT(!formatDetectionResults.isEmpty(), setError(tr("File format is not recognized (%1)").arg(srcFileUrl)), NULL);

    return formatDetectionResults.first().format;
}

}   // namespace U2
