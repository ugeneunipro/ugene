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

#include <U2Core/AppContext.h>
#include <U2Core/DataPathRegistry.h>
#include <U2Core/L10n.h>

#include <U2Designer/DelegateEditors.h>

#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/WorkflowEnv.h>

#include "DatabaseDelegate.h"

namespace U2 {
namespace LocalWorkflow {

DatabaseDelegate::DatabaseDelegate(const QString &_actorPrototypeId,
                                   const QString &_attributeName,
                                   const QList<StrStrPair> &_dataPathItems,
                                   const QString &_localDomain,
                                   bool _isFolder)
    : URLDelegate("", _localDomain, false, _isFolder, false),
      actorPrototypeId(_actorPrototypeId),
      attributeName(_attributeName),
      dataPathItems(_dataPathItems)
{
    tags()->set(DelegateTags::PLACEHOLDER_TEXT, L10N::required());
}

DatabaseDelegate::DatabaseDelegate(const QString &_actorPrototypeId,
                                   const QString &_attributeName,
                                   const QString &_dataPathDataId,
                                   const QString &_dataPathItemId,
                                   const QString &_localDomain,
                                   bool _isFolder)
    : URLDelegate("", _localDomain, false, _isFolder, false),
      actorPrototypeId(_actorPrototypeId),
      attributeName(_attributeName),
      dataPathItems(QList<StrStrPair>() << StrStrPair(_dataPathDataId, _dataPathItemId))
{
    tags()->set(DelegateTags::PLACEHOLDER_TEXT, L10N::required());
}

void DatabaseDelegate::update() {
    QString dataPathItemId;
    U2DataPath *dataPath = getDataPath(dataPathItemId);
    CHECK(NULL != dataPath && dataPath->isValid() && !dataPathItemId.isEmpty(), );

    Workflow::ActorPrototype *proto = Workflow::WorkflowEnv::getProtoRegistry()->getProto(actorPrototypeId);
    DelegateEditor *editor = qobject_cast<DelegateEditor *>(proto->getEditor());
    if (NULL != editor && NULL != editor->getDelegate(attributeName)) {
        Attribute *attribute = proto->getAttribute(attributeName);
        if (NULL != attribute && attribute->getAttributePureValue().toString().isEmpty()) {
            attribute->setAttributeValue(dataPath->getPathByName(dataPathItemId));
        }
    }
}

U2DataPath *DatabaseDelegate::getDataPath(QString &appropriateDataPathItemId) const {
    appropriateDataPathItemId = QString();

    U2DataPathRegistry* dataPathRegistry = AppContext::getDataPathRegistry();
    SAFE_POINT(dataPathRegistry, "U2DataPathRegistry is NULL", NULL);

    for (int i = 0; i < dataPathItems.size(); i++) {
        U2DataPath* dataPath = dataPathRegistry->getDataPathByName(dataPathItems[i].first);
        if (NULL != dataPath &&
            dataPath->isValid() &&
            !dataPath->getPathByName(dataPathItems[i].second).isEmpty()) {
            appropriateDataPathItemId = dataPathItems[i].second;
            return dataPath;
        }
    }
    return NULL;
}

}   // namesapce LocalWorkflow
}   // namesapce U2
