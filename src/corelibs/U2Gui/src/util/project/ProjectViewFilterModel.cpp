/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "ProjectViewFilterModel.h"

#include <QFontMetrics>

#include <U2Core/AppContext.h>
#include <U2Core/BunchMimeData.h>
#include <U2Core/L10n.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include "FilteredProjectGroup.h"
#include "ProjectFilterNames.h"
#include "ProjectUtils.h"
#include "ProjectViewModel.h"

namespace U2 {

ProjectViewFilterModel::ProjectViewFilterModel(ProjectViewModel* srcModel, const ProjectTreeControllerModeSettings& settings, QObject* p)
    : QAbstractItemModel(p), settings(settings), srcModel(srcModel) {
    SAFE_POINT(srcModel != nullptr, L10N::nullPointerError("Project view model"), );
    connect(&filterController, SIGNAL(si_objectsFiltered(const QString&, const QList<QPointer<GObject>>&)), SLOT(sl_objectsFiltered(const QString&, const QList<QPointer<GObject>>&)));
    connect(&filterController, SIGNAL(si_filteringStarted()), SIGNAL(si_filteringStarted()));
    connect(&filterController, SIGNAL(si_filteringFinished()), SIGNAL(si_filteringFinished()));

    connect(srcModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)), SLOT(sl_rowsAboutToBeRemoved(const QModelIndex&, int, int)));
    connect(srcModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), SLOT(sl_dataChanged(const QModelIndex&, const QModelIndex&)));
}

ProjectViewFilterModel::~ProjectViewFilterModel() {
    clearFilterGroups();
}

namespace {

QList<QPointer<Document>> getAllDocumentsSafely() {
    QList<QPointer<Document>> result;

    Project* proj = AppContext::getProject();
    SAFE_POINT(proj != nullptr, L10N::nullPointerError("project"), result);
    foreach (Document* doc, proj->getDocuments()) {
        result.append(doc);
    }
    return result;
}

}  // namespace

void ProjectViewFilterModel::updateSettings(const ProjectTreeControllerModeSettings& newSettings) {
    settings = newSettings;
    clearFilterGroups();

    if (settings.isObjectFilterActive()) {
        const QList<QPointer<Document>> allDocs = getAllDocumentsSafely();
        CHECK(!allDocs.isEmpty(), );
        filterController.startFiltering(settings, allDocs);
    }
}

void ProjectViewFilterModel::addFilteredObject(const QString& filterGroupName, GObject* obj) {
    SAFE_POINT(!filterGroupName.isEmpty(), "Empty project filter group name", );
    SAFE_POINT(obj != nullptr, L10N::nullPointerError("object"), );

    if (!hasFilterGroup(filterGroupName)) {
        addFilterGroup(filterGroupName);
    }

    FilteredProjectGroup* group = findFilterGroup(filterGroupName);
    SAFE_POINT(group != nullptr, L10N::nullPointerError("project filter group"), );

#ifdef _DEBUG
    SAFE_POINT(!group->contains(obj), "Attempting to add duplicate to a filter group", );
#endif

    const int objectRow = group->getNewObjectNumber(obj);
    beginInsertRows(getIndexForGroup(group), objectRow, objectRow);
    group->addObject(obj, objectRow);
    endInsertRows();
}

FilteredProjectGroup* ProjectViewFilterModel::findFilterGroup(const QString& name) const {
    SAFE_POINT(!name.isEmpty(), "Empty project filter group name", nullptr);

    if (ProjectFilterNames::OBJ_NAME_FILTER_NAME == name) {
        return filterGroups.isEmpty() ? nullptr : *(filterGroups.constBegin());
    } else {
        FilteredProjectGroup testGroup(name);
        const QList<FilteredProjectGroup*>::const_iterator begin = filterGroups.constBegin();
        const QList<FilteredProjectGroup*>::const_iterator end = filterGroups.constEnd();

        QList<FilteredProjectGroup*>::const_iterator posNextToResult = std::upper_bound(begin, end, &testGroup, FilteredProjectGroup::groupLessThan);
        return (posNextToResult != begin && (*(--posNextToResult))->getGroupName() == name) ? *posNextToResult : nullptr;
    }
}

QModelIndex ProjectViewFilterModel::getIndexForGroup(FilteredProjectGroup* group) const {
    SAFE_POINT(group != nullptr, "group is nullptr!", {});
    int groupRow = filterGroups.indexOf(group);
    SAFE_POINT(groupRow != -1, "Unexpected filter project group detected", QModelIndex());
    return createIndex(groupRow, 0, group);
}

QModelIndex ProjectViewFilterModel::getIndexForObject(const QString& groupName, GObject* obj) const {
    FilteredProjectGroup* group = findFilterGroup(groupName);
    SAFE_POINT(group != nullptr, L10N::nullPointerError("project filter group"), {});

    WrappedObject* wrappedObj = group->getWrappedObject(obj);
    SAFE_POINT(wrappedObj != nullptr, L10N::nullPointerError("filtered object"), {});
    return createIndex(group->getWrappedObjectNumber(wrappedObj), 0, wrappedObj);
}

void ProjectViewFilterModel::addFilterGroup(const QString& name) {
    SAFE_POINT(!name.isEmpty(), "Empty project filter group name", );
#ifdef _DEBUG
    SAFE_POINT(!hasFilterGroup(name), "Attempting to add a duplicate filter group", );
#endif

    auto newGroup = new FilteredProjectGroup(name);
    QList<FilteredProjectGroup*>::iterator insertionPlace = std::upper_bound(filterGroups.begin(), filterGroups.end(), newGroup, FilteredProjectGroup::groupLessThan);

    int groupNumber = insertionPlace - filterGroups.begin();
    beginInsertRows(QModelIndex(), groupNumber, groupNumber);
    filterGroups.insert(insertionPlace, newGroup);
    endInsertRows();

    QModelIndex groupIndex = createIndex(groupNumber, 0, newGroup);
    emit si_filterGroupAdded(groupIndex);
}

bool ProjectViewFilterModel::hasFilterGroup(const QString& name) const {
    return findFilterGroup(name) != nullptr;
}

void ProjectViewFilterModel::clearFilterGroups() {
    filterController.stopFiltering();

    beginResetModel();
    qDeleteAll(filterGroups);
    filterGroups.clear();
    endResetModel();
}

QModelIndex ProjectViewFilterModel::mapToSource(const QModelIndex& proxyIndex) const {
    switch (getType(proxyIndex)) {
        case GROUP:
            return QModelIndex();
        case OBJECT: {
            WrappedObject* obj = toObject(proxyIndex);
            return srcModel->getIndexForObject(obj->getObject());
        }
        default:
            FAIL("Unexpected parent item type", QModelIndex());
    }
}

int ProjectViewFilterModel::rowCount(const QModelIndex& parent) const {
    CHECK(parent.isValid(), filterGroups.size());

    switch (getType(parent)) {
        case GROUP: {
            FilteredProjectGroup* parentFilterGroup = toGroup(parent);
            return parentFilterGroup->getObjectsCount();
        }
        case OBJECT:
            return 0;
        default:
            FAIL("Unexpected parent item type", 0);
    }
}

int ProjectViewFilterModel::columnCount(const QModelIndex& /*parent*/) const {
    return 1;
}

void ProjectViewFilterModel::sl_objectsFiltered(const QString& groupName, const QList<QPointer<GObject>>& objs) {
    for (const QPointer<GObject>& obj : qAsConst(objs)) {
        CHECK_CONTINUE(!obj.isNull());
        QString objPath = srcModel->getObjectFolder(obj->getDocument(), obj);
        addFilteredObject(groupName, obj);
    }
}

QModelIndex ProjectViewFilterModel::index(int row, int column, const QModelIndex& parent) const {
    if (!parent.isValid()) {
        CHECK(row < filterGroups.size(), {});
        return createIndex(row, column, filterGroups[row]);
    }

    ItemType parentType = getType(parent);
    switch (parentType) {
        case GROUP: {
            FilteredProjectGroup* group = toGroup(parent);
            SAFE_POINT(group != nullptr, "toGroup(parent) returned nullptr!", {});
            return createIndex(row, column, group->getWrappedObject(row));
        }
        default:
            FAIL("Unexpected parent item type", {});
    }
}

QModelIndex ProjectViewFilterModel::parent(const QModelIndex& index) const {
    CHECK(index.isValid(), QModelIndex());

    switch (getType(index)) {
        case GROUP:
            return QModelIndex();
        case OBJECT:
            return getIndexForGroup(toObject(index)->getParentGroup());
        default:
            FAIL("Unexpected parent item type", QModelIndex());
    }
}

Qt::ItemFlags ProjectViewFilterModel::flags(const QModelIndex& index) const {
    CHECK(index.isValid(), QAbstractItemModel::flags(index));

    switch (getType(index)) {
        case GROUP:
            return QAbstractItemModel::flags(index);
        default: {
            Qt::ItemFlags result = srcModel->flags(mapToSource(index));
            result &= ~Qt::ItemIsEditable;
            result &= ~Qt::ItemIsDropEnabled;
            return result;
        }
    }
}

QVariant ProjectViewFilterModel::getGroupData(const QModelIndex& index, int role) const {
    SAFE_POINT(0 <= index.row() && index.row() < filterGroups.size(), "Project group number out of range", QVariant());

    switch (role) {
        case Qt::DisplayRole:
            return filterGroups[index.row()]->getGroupName();
        default:
            return QVariant();
    }
}

QVariant ProjectViewFilterModel::data(const QModelIndex& index, int role) const {
    const ItemType itemType = getType(index);
    switch (itemType) {
        case GROUP:
            return getGroupData(index, role);
        case OBJECT:
            return getObjectData(index, role);
        default:
            FAIL("Unexpected model item type", QVariant());
    }
}

void ProjectViewFilterModel::sl_dataChanged(const QModelIndex& before, const QModelIndex& after) {
    SAFE_POINT(before == after, "Unexpected project item index change", );

    if (ProjectViewModel::itemType(before) == ProjectViewModel::OBJECT) {
        GObject* object = ProjectViewModel::toObject(before);
        foreach (FilteredProjectGroup* group, filterGroups) {
            if (group->contains(object)) {
                const QModelIndex proxyObjIndex = getIndexForObject(group->getGroupName(), object);
                emit dataChanged(proxyObjIndex, proxyObjIndex);
            }
        }
    }
}

void ProjectViewFilterModel::sl_rowsAboutToBeRemoved(const QModelIndex& parent, int first, int last) {
    SAFE_POINT(first == last, "Unexpected row range", );

    const QModelIndex removedIndex = srcModel->index(first, 0, parent);
    QList<GObject*> objectsBeingRemoved;
    switch (ProjectViewModel::itemType(removedIndex)) {
        case ProjectViewModel::OBJECT:
            objectsBeingRemoved.append(ProjectViewModel::toObject(removedIndex));
            break;
        case ProjectViewModel::FOLDER: {
            Folder* folder = ProjectViewModel::toFolder(removedIndex);
            objectsBeingRemoved.append(srcModel->getFolderObjects(folder->getDocument(), folder->getFolderPath()));
        } break;
        case ProjectViewModel::DOCUMENT:
            objectsBeingRemoved.append(ProjectViewModel::toDocument(removedIndex)->getObjects());
            break;
        default:
            FAIL("Unexpected project item type", );
    }

    for (GObject* obj : qAsConst(objectsBeingRemoved)) {
        foreach (FilteredProjectGroup* group, filterGroups) {
            WrappedObject* wrappedObj = group->getWrappedObject(obj);
            if (wrappedObj != nullptr) {
                QModelIndex parentIndex = getIndexForGroup(group);
                int objNumber = group->getWrappedObjectNumber(wrappedObj);
                SAFE_POINT(objNumber != -1, "Unexpected object number", );
                beginRemoveRows(parentIndex, objNumber, objNumber);
                group->removeAt(objNumber);
                endRemoveRows();
            }
        }
    }
}

QString ProjectViewFilterModel::getStyledObjectName(GObject* obj, FilteredProjectGroup* group) const {
    SAFE_POINT(obj != nullptr && group != nullptr, "Invalid arguments supplied", QString());

    QString result = obj->getGObjectName();
    if (group->getGroupName() == ProjectFilterNames::OBJ_NAME_FILTER_NAME) {
        const QString stylePattern = "<span style=\"background-color:yellow;color:black\">%1</span>";
        foreach (const QString& token, settings.tokensToShow) {
            int nextTokenPos = -1;
            const int tokenSize = token.length();
            while (-1 != (nextTokenPos = result.indexOf(token, nextTokenPos + 1, Qt::CaseInsensitive))) {
                const QString coloredText = QString(stylePattern).arg(result.mid(nextTokenPos, tokenSize));
                result.replace(nextTokenPos, tokenSize, coloredText);
                nextTokenPos += coloredText.size();
            }
        }
    }

    const QVariant objFontData = srcModel->getIndexForObject(obj).data(Qt::FontRole);
    const QFont objectFont = objFontData.isValid() ? objFontData.value<QFont>() : QFont();
    if (objectFont.bold()) {
        result = QString("<b>%1</b>").arg(result);
    }
    return result;
}

QVariant ProjectViewFilterModel::getObjectData(const QModelIndex& index, int role) const {
    QVariant result = srcModel->data(mapToSource(index), role);

    if (role == Qt::DisplayRole) {
        GObject* object = toObject(index)->getObject();
        Document* parentDoc = object->getDocument();
        if (parentDoc != nullptr) {
            QString objectPath = srcModel->getObjectFolder(parentDoc, object);
            QString itemDocInfo = parentDoc->getName();
            QString objectName = getStyledObjectName(object, toGroup(index.parent()));
            result = QString("%1<p style=\"margin-top:0px;font-size:small;\">%2</p>").arg(objectName).arg(itemDocInfo);
        }
    }
    return result;
}

QMimeData* ProjectViewFilterModel::mimeData(const QModelIndexList& indexes) const {
    QSet<GObject*> uniqueObjs;
    foreach (const QModelIndex& index, indexes) {
        if (isObject(index)) {
            uniqueObjs.insert(toObject(index)->getObject());
        }
    }

    QModelIndexList reducedIndexes;
    foreach (GObject* obj, uniqueObjs) {
        reducedIndexes.append(srcModel->getIndexForObject(obj));
    }

    return srcModel->mimeData(reducedIndexes);
}

QStringList ProjectViewFilterModel::mimeTypes() const {
    QStringList result;
    result << GObjectMimeData::MIME_TYPE;
    result << BunchMimeData::MIME_TYPE;
    return result;
}

ProjectViewFilterModel::ItemType ProjectViewFilterModel::getType(const QModelIndex& index) {
    QObject* data = toQObject(index);
    CHECK(data != nullptr, GROUP);

    if (qobject_cast<WrappedObject*>(data) != nullptr) {
        return OBJECT;
    } else if (qobject_cast<FilteredProjectGroup*>(data) != nullptr) {
        return GROUP;
    } else {
        FAIL("Unexpected data type", GROUP);
    }
}

bool ProjectViewFilterModel::isObject(const QModelIndex& index) {
    return getType(index) == OBJECT;
}

QObject* ProjectViewFilterModel::toQObject(const QModelIndex& index) {
    auto internalObj = static_cast<QObject*>(index.internalPointer());
    SAFE_POINT(internalObj != nullptr, "Invalid index data", nullptr);
    return internalObj;
}

FilteredProjectGroup* ProjectViewFilterModel::toGroup(const QModelIndex& index) {
    return qobject_cast<FilteredProjectGroup*>(toQObject(index));
}

WrappedObject* ProjectViewFilterModel::toObject(const QModelIndex& index) {
    return qobject_cast<WrappedObject*>(toQObject(index));
}

}  // namespace U2
