/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include "KrakenBuildPrompter.h"
#include "KrakenBuildTask.h"
#include "KrakenBuildWorkerFactory.h"

namespace U2 {
namespace LocalWorkflow {

KrakenBuildPrompter::KrakenBuildPrompter(Actor *actor)
    : PrompterBase<KrakenBuildPrompter>(actor)
{

}

QString KrakenBuildPrompter::composeRichDoc() {
    if (KrakenBuildTaskSettings::BUILD == getParameter(KrakenBuildWorkerFactory::MODE_ATTR_ID).toString()) {
        const QString newDatabaseUrl = getHyperlink(KrakenBuildWorkerFactory::NEW_DATABASE_NAME_ATTR_ID, getURL(KrakenBuildWorkerFactory::NEW_DATABASE_NAME_ATTR_ID));
        return tr("Use custom data to build %1 Kraken database.").arg(newDatabaseUrl);
    } else {
        const QString inputDatabaseUrl = getHyperlink(KrakenBuildWorkerFactory::INPUT_DATABASE_NAME_ATTR_ID, getURL(KrakenBuildWorkerFactory::INPUT_DATABASE_NAME_ATTR_ID));
        const QString newDatabaseUrl = getHyperlink(KrakenBuildWorkerFactory::NEW_DATABASE_NAME_ATTR_ID, getURL(KrakenBuildWorkerFactory::NEW_DATABASE_NAME_ATTR_ID));
        return tr("Shrink Kraken database %1 to %2.").arg(inputDatabaseUrl).arg(newDatabaseUrl);
    }
}

}   // namespace LocalWorkflow
}   // namespace U2
