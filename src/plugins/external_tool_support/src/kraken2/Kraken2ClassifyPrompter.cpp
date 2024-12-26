/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "Kraken2ClassifyPrompter.h"

#include <U2Lang/BaseSlots.h>

#include "Kraken2ClassifyTask.h"
#include "Kraken2ClassifyWorkerFactory.h"

namespace U2 {
namespace LocalWorkflow {

Kraken2ClassifyPrompter::Kraken2ClassifyPrompter(Actor *actor)
    : PrompterBase<Kraken2ClassifyPrompter>(actor) {
}

QString Kraken2ClassifyPrompter::composeRichDoc() {
    const QString readsProducerName = getProducersOrUnset(Kraken2ClassifyWorkerFactory::IN_PORT_DESCR_SINGLE, Kraken2ClassifyWorkerFactory::INPUT_SLOT);
    const QString databaseUrl = getHyperlink(Kraken2ClassifyWorkerFactory::DATABASE_ATTR_ID, getURL(Kraken2ClassifyWorkerFactory::DATABASE_ATTR_ID));

    if (getParameter(Kraken2ClassifyWorkerFactory::INPUT_DATA_ATTR_ID).toString() == Kraken2ClassifyTaskSettings::SINGLE_END) {
        return tr("Classify sequences from <u>%1</u> with Kraken 2, use %2 database.").arg(readsProducerName).arg(databaseUrl);
    } else {
        const QString pairedReadsProducerName = getProducersOrUnset(Kraken2ClassifyWorkerFactory::IN_PORT_DESCR_PAIRED, Kraken2ClassifyWorkerFactory::PAIRED_INPUT_SLOT);
        return tr("Classify paired-end reads from <u>%1</u> and <u>%2</u> with Kraken 2, use %3 database.")
            .arg(readsProducerName).arg(pairedReadsProducerName).arg(databaseUrl);
    }
}

}  // namespace LocalWorkflow
}  // namespace U2
