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
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/ActorModel.h>
#include <U2Lang/ActorPrototypeRegistry.h>
#include <U2Lang/WorkflowEnv.h>

#include "CistromeDelegate.h"
#include "utils/ExternalToolUtils.h"

namespace U2 {
namespace LocalWorkflow {

void CistromeComboBoxWithUrlsDelegate::update() {
    updateUgeneSettings();
    updateValues(getDataPathName(), getAttributeName(), getDefaultValue());
}

void CistromeComboBoxWithUrlsDelegate::updateValues(const QString &dataPathName, const QString &attributeName, const QString &defaultValue) {
    items.clear();

    U2DataPathRegistry* dpr = AppContext::getDataPathRegistry();
    SAFE_POINT(dpr, "U2DataPathRegistry is NULL", );

    U2DataPath* dataPath = dpr->getDataPathByName(dataPathName);
    CHECK(dataPath && dataPath->isValid(), );

    QVariant newValue;
    if (dataPath) {
        items = dataPath->getDataItemsVariantMap();
        if (!items.isEmpty()) {
            if (!defaultValue.isEmpty()) {
                newValue = items.value(defaultValue, items.values().first());
            } else {
                newValue = items.values().first();
            }
        }
    }

    QMap<Descriptor, QList<Workflow::ActorPrototype*> > protos = Workflow::WorkflowEnv::getProtoRegistry()->getProtos();
    foreach (QList<Workflow::ActorPrototype*> list, protos.values()) {
        foreach (Workflow::ActorPrototype* proto, list) {
            DelegateEditor* editor = dynamic_cast<DelegateEditor*>(proto->getEditor());
            if (editor && editor->getDelegate(attributeName)) {
                proto->getAttribute(attributeName)->setAttributeValue(newValue);
            }
        }
    }
}

void CistromeComboBoxWithUrlsDelegate::updateDataPath(const QString &dataPathName, const QString &dirName, bool folders) {
    QString cistromeDataPath = AppContext::getSettings()->getValue(ExternalToolUtils::CISTROME_DATA_DIR).toString();
    QString dirPath = cistromeDataPath + QDir::separator() + dirName;

    U2DataPathRegistry* dpr = AppContext::getDataPathRegistry();
    SAFE_POINT(dpr, "U2DataPathRegistry is NULL", );

    U2DataPath* oldDp = dpr->getDataPathByName(dataPathName);
    U2DataPath* newDp = new U2DataPath(dataPathName, dirPath, "", U2DataPath::CutFileExtension | (folders ? U2DataPath::AddOnlyFolders : U2DataPath::None));
    if (newDp->isValid() &&
            (!oldDp || *oldDp != *newDp)) {
        if (oldDp) {
            dpr->unregisterEntry(dataPathName);
        }
        if (!dpr->registerEntry(newDp)) {
            delete newDp;
            Q_ASSERT(0);
        }
    }
}

}   // namespace LocalWorkflow
}   // namespace U2
