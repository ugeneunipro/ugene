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

#include "DiamondClassifyPrompter.h"
#include "DiamondClassifyTask.h"
#include "DiamondClassifyWorkerFactory.h"

namespace U2 {
namespace LocalWorkflow {

DiamondClassifyPrompter::DiamondClassifyPrompter(Actor *actor)
    : PrompterBase<DiamondClassifyPrompter>(actor)
{

}

QString DiamondClassifyPrompter::composeRichDoc() {
    const QString readsProducerName = getProducersOrUnset(DiamondClassifyWorkerFactory::INPUT_PORT_ID, DiamondClassifyWorkerFactory::INPUT_SLOT);
    const QString databaseUrl = getHyperlink(DiamondClassifyWorkerFactory::DATABASE_ATTR_ID, getURL(DiamondClassifyWorkerFactory::DATABASE_ATTR_ID));
    return tr("Classify sequences from <u>%1</u> with DIAMOND, use %2 database.").arg(readsProducerName).arg(databaseUrl);
}

}   // namespace LocalWorkflow
}   // namespace U2
