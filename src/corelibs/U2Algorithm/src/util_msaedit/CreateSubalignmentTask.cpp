/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

CreateSubalignmentTask::CreateSubalignmentTask(MultipleSequenceAlignmentObject *maObj, const CreateSubalignmentSettings &settings)
    : DocumentProviderTask(tr("Create sub-alignment: %1").arg(maObj->getDocument()->getName()), TaskFlags_NR_FOSCOE),
      origMAObj(maObj), resultMAObj(nullptr), cfg(settings) {
    origDoc = maObj->getDocument();
    createCopy = cfg.url != origDoc->getURL() || cfg.url.isEmpty();
}

void CreateSubalignmentTask::prepare() {
    DocumentFormatRegistry *dfr = AppContext::getDocumentFormatRegistry();
    DocumentFormat *dfd = dfr->getFormatById(cfg.formatIdToSave);

    IOAdapterFactory *iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(cfg.url));
    if (createCopy) {
        QVariantMap hints = origDoc->getGHintsMap();
        if (hints.value(DocumentReadingMode_SequenceAsAlignmentHint, false).toBool()) {
            hints[DocumentReadingMode_SequenceAsAlignmentHint] = false;
        }
        resultDocument = dfd->createNewLoadedDocument(iof, cfg.url, stateInfo, hints);
        CHECK_OP(stateInfo, );

        MultipleSequenceAlignment msa = origMAObj->getMsaCopy();
        resultMAObj = MultipleSequenceAlignmentImporter::createAlignment(resultDocument->getDbiRef(), msa, stateInfo);
        CHECK_OP(stateInfo, );
        resultMAObj->setGHints(new GHintsDefaultImpl(origMAObj->getGHintsMap()));

        resultDocument->addObject(resultMAObj);
        GObjectUtils::updateRelationsURL(resultMAObj, origDoc->getURL(), cfg.url);
        QList<GObjectRelation> phyTreeRelations = resultMAObj->findRelatedObjectsByRole(ObjectRole_PhylogeneticTree);
        foreach (GObjectRelation phyTreeRel, phyTreeRelations) {
            resultMAObj->removeObjectRelation(phyTreeRel);
        }
    } else {
        CHECK_EXT(origDoc->isStateLocked(), setError(tr("Document is locked: %1").arg(origDoc->getURLString())), );
        resultDocument = origDoc;
        resultMAObj = origMAObj;
        docOwner = false;
    }

    //TODO: add "remove empty rows and columns" flag to crop function
    if (cfg.rowIds.isEmpty()) {
        resultMAObj->crop(cfg.window, cfg.sequenceNames.toSet());
    } else {
        QList<qint64> resultRowIdList;    // Maps original row ids to the result row ids by index.
        if (createCopy) {
            // Remap old object row ids into new object row ids before calling 'crop'.
            for (qint64 origRowId : qAsConst(cfg.rowIds)) {
                int rowIndex = origMAObj->getRowPosById(origRowId);
                SAFE_POINT(rowIndex >= 0, "Failed to find row by id: " + QString::number(origRowId), );
                MultipleAlignmentRow row = resultMAObj->getRow(rowIndex);
                resultRowIdList << row->getRowId();
            }
        } else {
            resultRowIdList = cfg.rowIds;
        }
        resultMAObj->crop(cfg.window, resultRowIdList);
    }

    if (cfg.saveImmediately) {
        addSubTask(new SaveDocumentTask(resultDocument, iof));
    }
}

}    // namespace U2
