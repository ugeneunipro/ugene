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

#include "Metaphlan2Prompter.h"
#include "Metaphlan2WorkerFactory.h"

namespace U2 {
namespace LocalWorkflow {

Metaphlan2Prompter::Metaphlan2Prompter(Actor *actor)
    : PrompterBase<Metaphlan2Prompter>(actor) {}

QString Metaphlan2Prompter::composeRichDoc() {
    const QString readsProducerName = getProducersOrUnset(Metaphlan2WorkerFactory::INPUT_PORT_ID,
                                                          Metaphlan2WorkerFactory::INPUT_SLOT);
    const QString databaseUrl = getHyperlink(Metaphlan2WorkerFactory::DB_URL,
                                             getURL(Metaphlan2WorkerFactory::DB_URL));
    return tr("Classify sequences from <u>%1</u> with MetaPhlAn2, "
              "use %2 database.").arg(readsProducerName).arg(databaseUrl);
}

}   // namespace LocalWorkflow
}   // namespace U2
