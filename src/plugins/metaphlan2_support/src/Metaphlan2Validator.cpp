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

#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/DataPathRegistry.h>

#include "Metaphlan2Validator.h"
#include "Metaphlan2WorkerFactory.h"

#include "../ngs_reads_classification/src/NgsReadsClassificationPlugin.h"

namespace U2 {
namespace Workflow {

bool Metaphlan2Validator::validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &) const {
    return validateDatabase(actor, notificationList);
}

bool Metaphlan2Validator::validateDatabase(const Actor *actor, NotificationsList &notificationList) const {
    const QString dbUrl = actor->getParameter(LocalWorkflow::Metaphlan2WorkerFactory::DB_URL)->getAttributeValueWithoutScript<QString>();
    CHECK(!dbUrl.isEmpty(), false);

    bool result = true;
    QDir dbDir(dbUrl);
    QStringList filterPkl = QStringList() << "*.pkl";
    QStringList dbPklEntries = dbDir.entryList(filterPkl);
    if (dbPklEntries.size() != 1) {
        notificationList << WorkflowNotification(tr("The database folder should contain a single \"*.pkl\" file."), actor->getId());
        result = false;
    }

    QStringList filterBt2 = QStringList() << "*.bt2";
    QStringList dbBt2Entries = dbDir.entryList(filterBt2);
    if (dbBt2Entries.size() != 6) {
        notificationList << WorkflowNotification(tr("The database folder should contain six Bowtie2 index files (\"*.bt2\")."), actor->getId());
        result = false;
    }

    return result;
}

}

}