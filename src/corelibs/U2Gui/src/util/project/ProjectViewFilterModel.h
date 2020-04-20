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

#ifndef _U2_PROJECT_VIEW_FILTER_MODEL_H_
#define _U2_PROJECT_VIEW_FILTER_MODEL_H_

#include <QAbstractProxyModel>

#include <U2Core/ProjectTreeControllerModeSettings.h>

#include "ProjectFilteringController.h"

namespace U2 {

class DocFolders;
class FilteredProjectGroup;
class Folder;
class FolderObjects;
class ProjectViewModel;
class WrappedObject;

class ProjectViewFilterModel : public QAbstractItemModel {
    Q_OBJECT
    Q_DISABLE_COPY(ProjectViewFilterModel)
public:
    ProjectViewFilterModel(ProjectViewModel *srcModel, const ProjectTreeControllerModeSettings &settings, QObject *p = NULL);
    ~ProjectViewFilterModel();

    void updateSettings(const ProjectTreeControllerModeSettings &settings);

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QMimeData * mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

    static bool isObject(const QModelIndex &index);

signals:
    void si_filterGroupAdded(const QModelIndex &groupIndex);
    void si_filteringStarted();
    void si_filteringFinished();

private slots:
    void sl_objectsFiltered(const QString &groupName, const QList<QPointer<GObject> > &objs);
    void sl_rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void sl_dataChanged(const QModelIndex &before, const QModelIndex &after);

private:
    enum ItemType {
        GROUP,
        OBJECT
    };

    static ItemType getType(const QModelIndex &index);
    static QObject * toQObject(const QModelIndex &index);
    static FilteredProjectGroup * toGroup(const QModelIndex &index);
    static WrappedObject * toObject(const QModelIndex &index);

    void addFilteredObject(const QString &filterGroupName, GObject *obj);

    QModelIndex getIndexForGroup(FilteredProjectGroup *group) const;
    QModelIndex getIndexForObject(const QString &groupName, GObject *obj) const;

    QVariant getGroupData(const QModelIndex &index, int role) const;
    QVariant getObjectData(const QModelIndex &index, int role) const;
    QString getStyledObjectName(GObject *obj, FilteredProjectGroup *group) const;

    bool hasFilterGroup(const QString &name) const;
    void addFilterGroup(const QString &name);
    FilteredProjectGroup * findFilterGroup(const QString &groupName) const;
    void clearFilterGroups();

    ProjectTreeControllerModeSettings settings;
    ProjectFilteringController filterController;
    ProjectViewModel *srcModel;
    QList<FilteredProjectGroup *> filterGroups;
};

} // namespace U2

#endif // _U2_PROJECT_VIEW_FILTER_MODEL_H_
