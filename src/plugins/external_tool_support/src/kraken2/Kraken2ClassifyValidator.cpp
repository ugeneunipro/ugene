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

#include "Kraken2ClassifyValidator.h"

#include <QFileInfo>

#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/Configuration.h>

#include "Kraken2ClassifyPrompter.h"
#include "Kraken2ClassifyWorkerFactory.h"

namespace U2 {
namespace Workflow {

bool Kraken2ClassifyValidator::validate(const Actor *actor, NotificationsList &notificationList, const QMap<QString, QString> &) const {
    return validateDatabase(actor, notificationList);
}

bool Kraken2ClassifyValidator::validateDatabase(const Actor *actor, NotificationsList &notificationList) const {
    const QString databaseUrl = actor->getParameter(LocalWorkflow::Kraken2ClassifyWorkerFactory::DATABASE_ATTR_ID)->getAttributeValueWithoutScript<QString>();
    const bool doesDatabaseDirExist = FileAndDirectoryUtils::isDirectoryExistsAndReadable(databaseUrl);
    CHECK_EXT(doesDatabaseDirExist,
              notificationList.append(WorkflowNotification(tr("The database folder \"%1\" doesn't exist.").arg(databaseUrl), actor->getId())),
              false);

    static const QStringList files = { "opts.k2d", "taxo.k2d", "database100mers.kmer_distrib", "database150mers.kmer_distrib", "database200mers.kmer_distrib", 
                                "database250mers.kmer_distrib", "database300mers.kmer_distrib", "database50mers.kmer_distrib", "database75mers.kmer_distrib",
                                "inspect.txt", "ktaxonomy.tsv", "seqid2taxid.map", "hash.k2d", "library_report.tsv"};
    QStringList missedFiles;
    for (const QString &file : files) {
        if (!QFileInfo(databaseUrl + "/" + file).exists()) {
            missedFiles << file;
        }
    }

    for (const QString &missedFile : missedFiles) {
        notificationList.append(WorkflowNotification(tr("The mandatory database file \"%1\" doesn't exist.").arg(databaseUrl + "/" + missedFile), actor->getId()));
    }
    CHECK(missedFiles.isEmpty(), false);

    return true;
}

}  // namespace Workflow
}  // namespace U2
