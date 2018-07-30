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

#ifndef _U2_DATABASE_DELEGATE_H_
#define _U2_DATABASE_DELEGATE_H_

#include <U2Designer/DelegateEditors.h>

#include "NgsReadsClassificationPlugin.h"

namespace U2 {

class U2DataPath;

namespace LocalWorkflow {

class U2NGS_READS_CLASSIFICATION_EXPORT DatabaseDelegate : public URLDelegate {
public:
    DatabaseDelegate(const QString &actorPrototypeId,
                     const QString &attributeName,
                     const QList<StrStrPair> &dataPathItems,
                     const QString &localDomain,
                     bool isFolder);

    DatabaseDelegate(const QString &actorPrototypeId,
                     const QString &attributeName,
                     const QString &dataPathDataId,
                     const QString &dataPathItemId,
                     const QString &localDomain,
                     bool isFolder);

    void update();

private:
    U2DataPath *getDataPath(QString &appropriateDataPathItemId) const;

    const QString actorPrototypeId;
    const QString attributeName;
    const QList<StrStrPair> dataPathItems;
    const QString dataPathDataId;
    const QString dataPathItemId;
};

}   // namesapce LocalWorkflow
}   // namesapce U2

#endif // _U2_DATABASE_DELEGATE_H_
