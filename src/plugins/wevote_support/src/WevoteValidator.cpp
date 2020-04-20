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
#include <U2Core/DataPathRegistry.h>

#include "WevotePrompter.h"
#include "WevoteValidator.h"
#include "../ngs_reads_classification/src/NgsReadsClassificationPlugin.h"

namespace U2 {
namespace Workflow {

bool WevoteValidator::validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &) const {
    return validateTaxonomy(actor, notificationList);
}

bool WevoteValidator::validateTaxonomy(const Actor *actor, NotificationsList &notificationList) const {
    bool isValid = true;

    U2DataPathRegistry *dataPathRegistry = AppContext::getDataPathRegistry();
    SAFE_POINT_EXT(NULL != dataPathRegistry, notificationList.append(WorkflowNotification("U2DataPathRegistry is NULL", actor->getId())), false);

    U2DataPath *taxonomyDataPath = dataPathRegistry->getDataPathByName(NgsReadsClassificationPlugin::TAXONOMY_DATA_ID);
    CHECK_EXT(NULL != taxonomyDataPath && taxonomyDataPath->isValid(),
              notificationList << WorkflowNotification(tr("Taxonomy classification data from NCBI are not available."), actor->getId()), false);

    const QString missingFileMessage = tr("Taxonomy classification data from NCBI are not full: file '%1' is missing.");
    if (taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_NODES_ITEM_ID).isEmpty()) {
        notificationList << WorkflowNotification(missingFileMessage.arg(NgsReadsClassificationPlugin::TAXON_NODES_ITEM_ID), actor->getId());
        isValid = false;
    }

    if (taxonomyDataPath->getPathByName(NgsReadsClassificationPlugin::TAXON_NAMES_ITEM_ID).isEmpty()) {
        notificationList << WorkflowNotification(missingFileMessage.arg(NgsReadsClassificationPlugin::TAXON_NAMES_ITEM_ID), actor->getId());
        isValid = false;
    }

    return isValid;
}

}   // namespace Workflow
}   // namespace U2
