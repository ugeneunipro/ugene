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

#pragma once

#include <QString>
#include <QString>
#include <QObject>

#include <U2Lang/Datatype.h>
#include <U2Lang/Descriptor.h>

namespace U2 {
namespace LocalWorkflow {

typedef uint TaxID;
typedef QMap<QString, TaxID> TaxonomyClassificationResult;

class TaxonomySupport : public QObject {
    Q_OBJECT
public:
    static const QString TAXONOMY_CLASSIFICATION_SLOT_ID;
    static const Descriptor TAXONOMY_CLASSIFICATION_SLOT();
    static DataTypePtr TAXONOMY_CLASSIFICATION_TYPE();
};

class TaxonomyTree {
public:
    /** Id to mark unknown or undefined value (e.g. search result or default initialization). */
    static const TaxID UNDEFINED_ID;
    /** Id to mark unclassified reads in results. */
    static const TaxID UNCLASSIFIED_ID;
};

}

class NgsReadsClassificationUtils {
public:
    static QString getBaseFileNameWithSuffixes(const QString &sourceFileUrl,
                                               const QStringList &suffixes,
                                               const QString &extension,
                                               bool truncate);
    static QString getBaseFileNameWithPrefixes(const QString &sourceFileUrl,
                                               const QStringList &prefixes,
                                               const QString &extension,
                                               bool truncate);
    static int countClassified(const LocalWorkflow::TaxonomyClassificationResult &classification);

    static const QString CLASSIFICATION_SUFFIX;
};

}    // namespace U2

Q_DECLARE_METATYPE(U2::LocalWorkflow::TaxID)
Q_DECLARE_METATYPE(U2::LocalWorkflow::TaxonomyClassificationResult)
