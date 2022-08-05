/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "CreateSubalignmentTask.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LocalFileAdapter.h>
#include <U2Core/Log.h>
#include <U2Core/MultipleSequenceAlignmentImporter.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

namespace U2 {

CreateSubalignmentSettings::CreateSubalignmentSettings(const QList<qint64>& _rowIds,
                                                       const U2Region& _columnRange,
                                                       const GUrl& _url,
                                                       bool _saveImmediately,
                                                       bool _addToProject,
                                                       const DocumentFormatId& _formatIdToSave)
    : rowIds(_rowIds), columnRange(_columnRange), url(_url), saveImmediately(_saveImmediately), addToProject(_addToProject), formatIdToSave(_formatIdToSave) {
}

CreateSubalignmentTask::CreateSubalignmentTask(MultipleSequenceAlignmentObject* maObj, const CreateSubalignmentSettings& settings)
    : DocumentProviderTask(tr("Create sub-alignment: %1").arg(maObj->getDocument()->getName()), TaskFlags_NR_FOSCOE),
      origMAObj(maObj), resultMAObj(nullptr), cfg(settings) {
    origDoc = maObj->getDocument();
    createCopy = cfg.url != origDoc->getURL() || cfg.url.isEmpty();
}

void CreateSubalignmentTask::prepare() {
    DocumentFormatRegistry* dfr = AppContext::getDocumentFormatRegistry();
    DocumentFormat* dfd = dfr->getFormatById(cfg.formatIdToSave);

    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(cfg.url));
    QList<qint64> resultRowIds;
    if (createCopy) {
        QVariantMap hints = origDoc->getGHintsMap();
        if (hints.value(DocumentReadingMode_SequenceAsAlignmentHint, false).toBool()) {
            hints[DocumentReadingMode_SequenceAsAlignmentHint] = false;
        }
        resultDocument = dfd->createNewLoadedDocument(iof, cfg.url, stateInfo, hints);
        CHECK_OP(stateInfo, );

        // TODO: do not copy whole object. Copy only cfg.rowIds.
        MultipleSequenceAlignment msa = origMAObj->getMsaCopy();
        resultMAObj = MultipleSequenceAlignmentImporter::createAlignment(resultDocument->getDbiRef(), msa, stateInfo);
        CHECK_OP(stateInfo, );
        resultMAObj->setGHints(new GHintsDefaultImpl(origMAObj->getGHintsMap()));

        resultDocument->addObject(resultMAObj);
        GObjectUtils::updateRelationsURL(resultMAObj, origDoc->getURL(), cfg.url);
        QList<GObjectRelation> phyTreeRelations = resultMAObj->findRelatedObjectsByRole(ObjectRole_PhylogeneticTree);
        for (const GObjectRelation& phyTreeRel : qAsConst(phyTreeRelations)) {
            resultMAObj->removeObjectRelation(phyTreeRel);
        }
        // Remap row ids.
        QMap<qint64, qint64> rowIdRemap;
        for (int i = 0; i < origMAObj->getRowCount() && i < resultMAObj->getRowCount(); i++) {
            qint64 oldRowId = origMAObj->getRow(i)->getRowId();
            qint64 resultRowId = resultMAObj->getRow(i)->getRowId();
            rowIdRemap[oldRowId] = resultRowId;
        }
        for (const qint64 oldRowId : qAsConst(cfg.rowIds)) {
            if (rowIdRemap.contains(oldRowId)) {
                resultRowIds << rowIdRemap[oldRowId];
            }
        }
    } else {
        CHECK_EXT(origDoc->isStateLocked(), setError(tr("Document is locked: %1").arg(origDoc->getURLString())), );
        resultDocument = origDoc;
        resultMAObj = origMAObj;
        resultRowIds = cfg.rowIds;
        docOwner = false;
    }

    // TODO: add "remove empty rows and columns" flag to crop function
    resultMAObj->crop(resultRowIds, cfg.columnRange);
    resultMAObj->updateRowsOrder(stateInfo, resultRowIds);

    if (cfg.saveImmediately) {
        addSubTask(new SaveDocumentTask(resultDocument, iof));
    }
}

}  // namespace U2
