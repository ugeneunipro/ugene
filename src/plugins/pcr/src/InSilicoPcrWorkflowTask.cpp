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

#include "InSilicoPcrWorkflowTask.h"

#include <U2Core/DocumentModel.h>

namespace U2 {

InSilicoPcrWorkflowTask::InSilicoPcrWorkflowTask(InSilicoPcrTaskSettings* pcrSettings, const ExtractProductSettings& _productSettings)
    : Task(tr("In silico PCR workflow task"), TaskFlags_NR_FOSE_COSC), productSettings(_productSettings), temperatureCalculator(pcrSettings->temperatureCalculator) {
    pcrTask = new InSilicoPcrTask(pcrSettings);
    addSubTask(pcrTask);
    pcrTask->setSubtaskProgressWeight((float)0.7);
}

QList<Task*> InSilicoPcrWorkflowTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> result;
    CHECK(subTask != nullptr, result);
    CHECK(!subTask->getStateInfo().isCoR(), result);

    if (subTask == pcrTask) {
        foreach (const InSilicoPcrProduct& product, pcrTask->getResults()) {
            QVariantMap hints;
            hints[DocumentRemovalMode_Synchronous] = true;
            auto productTask = new ExtractProductTask(product, productSettings, hints);
            productTask->setSubtaskProgressWeight(0.3 / pcrTask->getResults().size());
            result << productTask;
            productTasks << productTask;
        }
    }
    return result;
}

QList<InSilicoPcrWorkflowTask::Result> InSilicoPcrWorkflowTask::takeResult() {
    QList<Result> result;
    foreach (ExtractProductTask* productTask, productTasks) {
        Result pcrResult;
        pcrResult.doc = productTask->takeResult();
        pcrResult.product = productTask->getProduct();
        result << pcrResult;
    }
    return result;
}

const InSilicoPcrTaskSettings* InSilicoPcrWorkflowTask::getPcrSettings() const {
    return pcrTask->getSettings();
}

}  // namespace U2
