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

#include "KrakenBuildPrompter.h"
#include "KrakenBuildValidator.h"
#include "KrakenBuildWorkerFactory.h"
#include "../ngs_reads_classification/src/NgsReadsClassificationUtils.h"

namespace U2 {
namespace Workflow {

bool KrakenBuildValidator::validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &) const {
    const bool isMinimizerLengthValid = validateMinimizerLength(actor, notificationList);
    const bool isTaxonomyValid = validateTaxonomy(actor, notificationList);
    return isMinimizerLengthValid && isTaxonomyValid;
}

bool KrakenBuildValidator::validateMinimizerLength(const Actor *actor, NotificationsList &notificationList) const {
    const int minimizerLength = actor->getParameter(LocalWorkflow::KrakenBuildWorkerFactory::MINIMIZER_LENGTH_ATTR_ID)->getAttributeValueWithoutScript<int>();
    const int kMerLength = actor->getParameter(LocalWorkflow::KrakenBuildWorkerFactory::K_MER_LENGTH_ATTR_ID)->getAttributeValueWithoutScript<int>();
    if (minimizerLength >= kMerLength) {
        notificationList << WorkflowNotification(tr("Minimizer length has to be less than K-mer length"), actor->getId());
        return false;
    }

    return true;
}

bool KrakenBuildValidator::validateTaxonomy(const Actor *actor, NotificationsList &notificationList) const {
    U2DataPath *taxonomyDataPath = AppContext::getDataPathRegistry()->getDataPathByName(NgsReadsClassificationPlugin::TAXONOMY_DATA_ID);
    CHECK_EXT(NULL != taxonomyDataPath && taxonomyDataPath->isValid(),
              notificationList << WorkflowNotification(tr("Taxonomy classification data from NCBI are not available."), actor->getId()), false);

    bool isValid = true;
    const QString missingFileMessage = tr("Taxonomy classification data from NCBI are not full: file '%1' is missing.");

    const QStringList neccessaryItems = QStringList() << NgsReadsClassificationPlugin::TAXON_NODES_ITEM_ID
                                                      << NgsReadsClassificationPlugin::TAXON_NAMES_ITEM_ID
                                                      << NgsReadsClassificationPlugin::TAXON_NUCL_EST_ACCESSION_2_TAXID_ITEM_ID
                                                      << NgsReadsClassificationPlugin::TAXON_NUCL_GB_ACCESSION_2_TAXID_ITEM_ID
                                                      << NgsReadsClassificationPlugin::TAXON_NUCL_GSS_ACCESSION_2_TAXID_ITEM_ID
                                                      << NgsReadsClassificationPlugin::TAXON_NUCL_WGS_ACCESSION_2_TAXID_ITEM_ID;

    foreach (const QString &neccessaryItem, neccessaryItems) {
        if (taxonomyDataPath->getPathByName(neccessaryItem).isEmpty()) {
            notificationList << WorkflowNotification(missingFileMessage.arg(neccessaryItem), actor->getId());
            isValid = false;
        }
    }

    return isValid;
}

}   // namespace Workflow
}   // namespace U2
