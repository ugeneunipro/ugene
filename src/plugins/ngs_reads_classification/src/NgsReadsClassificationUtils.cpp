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

#include <QFileInfo>

#include <U2Core/GUrlUtils.h>

#include "NgsReadsClassificationUtils.h"

namespace U2 {

const QString NgsReadsClassificationUtils::CLASSIFICATION_SUFFIX = "classification";

QString NgsReadsClassificationUtils::getBaseFileNameWithSuffixes(const QString &sourceFileUrl,
                                                             const QStringList &suffixes,
                                                             const QString &extension,
                                                             bool truncate) {
    QString pairedName = GUrlUtils::getPairedFastqFilesBaseName(sourceFileUrl, truncate);
    QString result = pairedName;
    foreach(const QString& suffix, suffixes) {
        result += QString("_%1").arg(suffix);
    }
    if (pairedName.isEmpty()) {
        result = result.right(result.size() - 1);
    }
    result += QString(".%1").arg(extension);
    return result;
}

QString NgsReadsClassificationUtils::getBaseFileNameWithPrefixes(const QString &sourceFileUrl,
                                                             const QStringList &prefixes,
                                                             const QString &extension,
                                                             bool truncate) {
    QString pairedName = GUrlUtils::getPairedFastqFilesBaseName(sourceFileUrl, truncate);
    QString result = "";
    foreach(const QString& prefix, prefixes) {
        result += QString("%1_").arg(prefix);
    }
    result += pairedName;
    if (pairedName.isEmpty()) {
        result.chop(1);
    }
    result += QString(".%1").arg(extension);
    return result;
}

int NgsReadsClassificationUtils::countClassified(const LocalWorkflow::TaxonomyClassificationResult& classification) {
    LocalWorkflow::TaxonomyClassificationResult::const_iterator it;
    int classifiedCount = 0;
    for (it = classification.constBegin(); it != classification.constEnd(); ++it) {
        if (it.value() != LocalWorkflow::TaxonomyTree::UNCLASSIFIED_ID && it.value() != LocalWorkflow::TaxonomyTree::UNDEFINED_ID) {
            classifiedCount++;
        }
    }

    return classifiedCount;
}

}   // namespace U2
