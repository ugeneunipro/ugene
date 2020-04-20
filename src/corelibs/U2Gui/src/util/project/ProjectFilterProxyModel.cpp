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

#include <U2Core/L10n.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include "ProjectUtils.h"

#include "ProjectFilterProxyModel.h"

namespace U2 {

ProjectFilterProxyModel::ProjectFilterProxyModel(const ProjectTreeControllerModeSettings &settings, QObject *p)
    : QSortFilterProxyModel(p), settings(settings)
{
    setDynamicSortFilter(true);
    setFilterKeyColumn(0);
}

void ProjectFilterProxyModel::updateSettings(const ProjectTreeControllerModeSettings &newSettings) {
    settings = newSettings;
    invalidateFilter();
}

ProjectViewModel * ProjectFilterProxyModel::sourceModel() const {
    ProjectViewModel *srcModel = qobject_cast<ProjectViewModel *>(QSortFilterProxyModel::sourceModel());
    SAFE_POINT(NULL != srcModel, L10N::nullPointerError("project view model"), NULL);
    return srcModel;
}

QModelIndex ProjectFilterProxyModel::getIndexForDoc(Document *doc) const {
    ProjectViewModel *srcModel = sourceModel();
    CHECK(NULL != srcModel, QModelIndex());
    return mapFromSource(srcModel->getIndexForDoc(doc));
}

bool ProjectFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    ProjectViewModel *srcModel = sourceModel();
    CHECK(NULL != srcModel, false);

    const QModelIndex index = srcModel->index(sourceRow, 0, sourceParent);
    ProjectViewModel::Type itemType = srcModel->itemType(index);
    switch (itemType) {
    case ProjectViewModel::DOCUMENT :
        return settings.isDocumentShown(srcModel->toDocument(index));
    case ProjectViewModel::FOLDER :
        return filterAcceptsFolder(srcModel->toFolder(index));
    case ProjectViewModel::OBJECT :
        return settings.isObjectShown(srcModel->toObject(index));
    default :
        FAIL("Unexpected project item type", false);
    }
}

bool ProjectFilterProxyModel::filterAcceptsFolder(const Folder *folder) const {
    SAFE_POINT(NULL != folder, "Invalid folder detected", false);

    ProjectViewModel *srcModel = sourceModel();
    CHECK(NULL != srcModel, true);

    const QString path = folder->getFolderPath();

    if (!settings.isObjectFilterActive()) {
        return true;
    } else if (ProjectUtils::isFolderInRecycleBinSubtree(path)) {
        return false;
    }

    Document *doc = folder->getDocument();
    QList<GObject *> objs = srcModel->getFolderObjects(doc, path);
    foreach(GObject *obj, objs) {
        if (settings.isObjectShown(obj)) {
            return true;
        }
    }

    foreach (Folder *folder, srcModel->getSubfolders(doc, path)) {
        if (filterAcceptsFolder(folder)) {
            return true;
        }
    }

    return false;
}

} // namespace U2
