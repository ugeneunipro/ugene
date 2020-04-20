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

#include "ProjectViewFilterModel.h"

namespace U2 {

ProjectViewFilterModel::ProjectViewFilterModel(ProjectViewModel *srcModel, const ProjectTreeControllerModeSettings &settings, QObject *p)
    : QAbstractItemModel(p), settings(settings), srcModel(srcModel)
{
    SAFE_POINT(NULL != srcModel, L10N::nullPointerError("Project view model"), );
    connect(&filterController, SIGNAL(si_objectsFiltered(const QString &, const QList<QPointer<GObject> > &)),
        SLOT(sl_objectsFiltered(const QString &, const QList<QPointer<GObject> > &)));
    connect(&filterController, SIGNAL(si_filteringStarted()), SIGNAL(si_filteringStarted()));
    connect(&filterController, SIGNAL(si_filteringFinished()), SIGNAL(si_filteringFinished()));

    connect(srcModel, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)),
        SLOT(sl_rowsAboutToBeRemoved(const QModelIndex &, int, int)));
    connect(srcModel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
        SLOT(sl_dataChanged(const QModelIndex &, const QModelIndex &)));
}

ProjectViewFilterModel::~ProjectViewFilterModel() {
    clearFilterGroups();
}

namespace {

QList<QPointer<Document> > getAllDocumentsSafely() {
    QList<QPointer<Document> > result;

    Project *proj = AppContext::getProject();
    SAFE_POINT(NULL != proj, L10N::nullPointerError("project"), result);
    foreach (Document *doc, proj->getDocuments()) {
        result.append(doc);
    }
    return result;
}

}

void ProjectViewFilterModel::updateSettings(const ProjectTreeControllerModeSettings &newSettings) {
    settings = newSettings;
    clearFilterGroups();

    if (settings.isObjectFilterActive()) {
        const QList<QPointer<Document> > allDocs = getAllDocumentsSafely();
        CHECK(!allDocs.isEmpty(), );
        filterController.startFiltering(settings, allDocs);
    }
}

void ProjectViewFilterModel::addFilteredObject(const QString &filterGroupName, GObject *obj) {
    SAFE_POINT(!filterGroupName.isEmpty(), "Empty project filter group name", );
    SAFE_POINT(NULL != obj, L10N::nullPointerError("object"), );

    if (!hasFilterGroup(filterGroupName)) {
        addFilterGroup(filterGroupName);
    }

    FilteredProjectGroup *group = findFilterGroup(filterGroupName);
    SAFE_POINT(NULL != group, L10N::nullPointerError("project filter group"), );

#ifdef _DEBUG
    SAFE_POINT(!group->contains(obj), "Attempting to add duplicate to a filter group", );
#endif

    const int objectRow = group->getNewObjectNumber(obj);
    beginInsertRows(getIndexForGroup(group), objectRow, objectRow);
    group->addObject(obj, objectRow);
    endInsertRows();
}

FilteredProjectGroup * ProjectViewFilterModel::findFilterGroup(const QString &name) const {
    SAFE_POINT(!name.isEmpty(), "Empty project filter group name", NULL);

    if (ProjectFilterNames::OBJ_NAME_FILTER_NAME == name) {
        return filterGroups.isEmpty() ? NULL : *(filterGroups.constBegin());
    } else {
        FilteredProjectGroup testGroup(name);
        const QList<FilteredProjectGroup *>::const_iterator begin = filterGroups.constBegin();
        const QList<FilteredProjectGroup *>::const_iterator end = filterGroups.constEnd();

        QList<FilteredProjectGroup *>::const_iterator posNextToResult = std::upper_bound(begin, end, &testGroup, FilteredProjectGroup::groupLessThan);
        return (posNextToResult != begin && (*(--posNextToResult))->getGroupName() == name) ? *posNextToResult : NULL;
    }
}

QModelIndex ProjectViewFilterModel::getIndexForGroup(FilteredProjectGroup *group) const {
    const int groupRow = filterGroups.indexOf(group);
    SAFE_POINT(-1 != groupRow, "Unexpected filter project group detected", QModelIndex());
    return createIndex(groupRow, 0, group);
}

QModelIndex ProjectViewFilterModel::getIndexForObject(const QString &groupName, GObject *obj) const {
    FilteredProjectGroup *group = findFilterGroup(groupName);
    SAFE_POINT(NULL != group, L10N::nullPointerError("project filter group"), QModelIndex());

    WrappedObject *wrappedObj = group->getWrappedObject(obj);
    SAFE_POINT(NULL != wrappedObj, L10N::nullPointerError("filtered object"), QModelIndex());
    return createIndex(group->getWrappedObjectNumber(wrappedObj), 0, wrappedObj);
}

void ProjectViewFilterModel::addFilterGroup(const QString &name) {
    SAFE_POINT(!name.isEmpty(), "Empty project filter group name", );
#ifdef _DEBUG
    SAFE_POINT(!hasFilterGroup(name), "Attempting to add a duplicate filter group", );
#endif

    FilteredProjectGroup *newGroup = new FilteredProjectGroup(name);
    QList<FilteredProjectGroup *>::iterator insertionPlace = std::upper_bound(filterGroups.begin(), filterGroups.end(),
        newGroup, FilteredProjectGroup::groupLessThan);

    const int groupNumber = insertionPlace - filterGroups.begin();
    beginInsertRows(QModelIndex(), groupNumber, groupNumber);
    filterGroups.insert(insertionPlace, newGroup);
    endInsertRows();

    emit si_filterGroupAdded(createIndex(groupNumber, 0, newGroup));
}

bool ProjectViewFilterModel::hasFilterGroup(const QString &name) const {
    return NULL != findFilterGroup(name);
}

void ProjectViewFilterModel::clearFilterGroups() {
    filterController.stopFiltering();

    beginResetModel();
    qDeleteAll(filterGroups);
    filterGroups.clear();
    endResetModel();
}

QModelIndex ProjectViewFilterModel::mapToSource(const QModelIndex &proxyIndex) const {
    switch (getType(proxyIndex)) {
    case GROUP:
        return QModelIndex();
    case OBJECT: {
        WrappedObject *obj = toObject(proxyIndex);
        return srcModel->getIndexForObject(obj->getObject());
    }
    default:
        FAIL("Unexpected parent item type", QModelIndex());
    }
}

int ProjectViewFilterModel::rowCount(const QModelIndex &parent) const {
    CHECK(parent.isValid(), filterGroups.size());

    switch (getType(parent)) {
    case GROUP: {
        FilteredProjectGroup *parentFilterGroup = toGroup(parent);
        return parentFilterGroup->getObjectsCount();
    }
    case OBJECT:
        return 0;
    default:
        FAIL("Unexpected parent item type", 0);
    }
}

int ProjectViewFilterModel::columnCount(const QModelIndex & /*parent*/) const {
    return 1;
}

void ProjectViewFilterModel::sl_objectsFiltered(const QString &groupName, const QList<QPointer<GObject> > &objs) {
    foreach (const QPointer<GObject> &obj, objs) {
        const QString objPath = srcModel->getObjectFolder(obj->getDocument(), obj.data());
        if (!obj.isNull() && !ProjectUtils::isFolderInRecycleBinSubtree(objPath)) {
            addFilteredObject(groupName, obj.data());
        }
    }
}

QModelIndex ProjectViewFilterModel::index(int row, int column, const QModelIndex &parent) const {
    if (!parent.isValid()) {
        CHECK(row < filterGroups.size(), QModelIndex());
        return createIndex(row, column, filterGroups[row]);
    }

    switch (getType(parent)) {
    case GROUP:
        return createIndex(row, column, toGroup(parent)->getWrappedObject(row));
    default:
        FAIL("Unexpected parent item type", QModelIndex());
    }
}

QModelIndex ProjectViewFilterModel::parent(const QModelIndex &index) const {
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

Qt::ItemFlags ProjectViewFilterModel::flags(const QModelIndex &index) const {
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

QVariant ProjectViewFilterModel::getGroupData(const QModelIndex &index, int role) const {
    SAFE_POINT(0 <= index.row() && index.row() < filterGroups.size(), "Project group number out of range", QVariant());

    switch (role) {
    case Qt::DisplayRole:
        return filterGroups[index.row()]->getGroupName();
    default:
        return QVariant();
    }
}

QVariant ProjectViewFilterModel::data(const QModelIndex &index, int role) const {
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

void ProjectViewFilterModel::sl_dataChanged(const QModelIndex &before, const QModelIndex &after) {
    SAFE_POINT(before == after, "Unexpected project item index change", );

    if (ProjectViewModel::itemType(before) == ProjectViewModel::OBJECT) {
        GObject *object = ProjectViewModel::toObject(before);
        foreach (FilteredProjectGroup *group, filterGroups) {
            if (group->contains(object)) {
                const QModelIndex proxyObjIndex = getIndexForObject(group->getGroupName(), object);
                emit dataChanged(proxyObjIndex, proxyObjIndex);
            }
        }
    }
}

void ProjectViewFilterModel::sl_rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last) {
    SAFE_POINT(first == last, "Unexpected row range", );

    const QModelIndex removedIndex = srcModel->index(first, 0, parent);
    QList<GObject *> objectsBeingRemoved;
    switch (ProjectViewModel::itemType(removedIndex)) {
    case ProjectViewModel::OBJECT:
        objectsBeingRemoved.append(ProjectViewModel::toObject(removedIndex));
        break;
    case ProjectViewModel::FOLDER: {
        Folder *folder = ProjectViewModel::toFolder(removedIndex);
        objectsBeingRemoved.append(srcModel->getFolderObjects(folder->getDocument(), folder->getFolderPath()));
    }
        break;
    case ProjectViewModel::DOCUMENT:
        objectsBeingRemoved.append(ProjectViewModel::toDocument(removedIndex)->getObjects());
        break;
    default:
        FAIL("Unexpected project item type", );
    }

    foreach (GObject *obj, objectsBeingRemoved) {
        foreach (FilteredProjectGroup *group, filterGroups) {
            WrappedObject *wrappedObj = group->getWrappedObject(obj);
            if (NULL != wrappedObj) {
                const QModelIndex parentIndex = getIndexForGroup(group);
                const int objNumber = group->getWrappedObjectNumber(wrappedObj);
                SAFE_POINT(-1 != objNumber, "Unexpected object number", );
                beginRemoveRows(parentIndex, objNumber, objNumber);
                group->removeAt(objNumber);
                endRemoveRows();
            }
        }
    }
}

QString ProjectViewFilterModel::getStyledObjectName(GObject *obj, FilteredProjectGroup *group) const {
    SAFE_POINT(NULL != obj && NULL != group, "Invalid arguments supplied", QString());

    QString result = obj->getGObjectName();
    if (group->getGroupName() == ProjectFilterNames::OBJ_NAME_FILTER_NAME) {
        const QString stylePattern = "<span style=\"background-color:yellow;color:black\">%1</span>";
        foreach (const QString &token, settings.tokensToShow) {
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

QVariant ProjectViewFilterModel::getObjectData(const QModelIndex &index, int role) const {
    QVariant result = srcModel->data(mapToSource(index), role);

    if (Qt::DisplayRole == role) {
        GObject *object = toObject(index)->getObject();
        Document *parentDoc = object->getDocument();
        if (NULL != parentDoc) {
            const QString objectPath = srcModel->getObjectFolder(parentDoc, object);
            const bool isDatabase = parentDoc->isDatabaseConnection();
            const QString itemDocInfo = parentDoc->getName() + (isDatabase ? ": " + objectPath : QString());
            const QString objectName = getStyledObjectName(object, toGroup(index.parent()));
            result = QString("%1<p style=\"margin-top:0px;font-size:small;\">%2</p>").arg(objectName).arg(itemDocInfo);
        }
    }
    return result;
}

QMimeData * ProjectViewFilterModel::mimeData(const QModelIndexList &indexes) const {
    QSet<GObject *> uniqueObjs;
    foreach (const QModelIndex &index, indexes) {
        if (isObject(index)) {
            uniqueObjs.insert(toObject(index)->getObject());
        }
    }

    QModelIndexList reducedIndexes;
    foreach (GObject *obj, uniqueObjs) {
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

ProjectViewFilterModel::ItemType ProjectViewFilterModel::getType(const QModelIndex &index) {
    QObject *data = toQObject(index);
    CHECK(NULL != data, GROUP);

    if (NULL != qobject_cast<WrappedObject *>(data)) {
        return OBJECT;
    } else if (NULL != qobject_cast<FilteredProjectGroup *>(data)) {
        return GROUP;
    } else {
        FAIL("Unexpected data type", GROUP);
    }
}

bool ProjectViewFilterModel::isObject(const QModelIndex &index) {
    return OBJECT == getType(index);
}

QObject * ProjectViewFilterModel::toQObject(const QModelIndex &index) {
    QObject *internalObj = static_cast<QObject *>(index.internalPointer());
    SAFE_POINT(NULL != internalObj, "Invalid index data", NULL);
    return internalObj;
}

FilteredProjectGroup * ProjectViewFilterModel::toGroup(const QModelIndex &index) {
    return qobject_cast<FilteredProjectGroup *>(toQObject(index));
}

WrappedObject * ProjectViewFilterModel::toObject(const QModelIndex &index) {
    return qobject_cast<WrappedObject *>(toQObject(index));
}

} // namespace U2
