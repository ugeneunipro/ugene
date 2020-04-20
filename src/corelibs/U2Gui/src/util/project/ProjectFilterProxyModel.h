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

#ifndef _U2_PROJECT_FILTER_PROXY_MODEL_H_
#define _U2_PROJECT_FILTER_PROXY_MODEL_H_

#include <QSortFilterProxyModel>

#include <U2Gui/ProjectViewModel.h>

namespace U2 {

class ProjectFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    ProjectFilterProxyModel(const ProjectTreeControllerModeSettings &settings, QObject *p = NULL);

    ProjectViewModel * sourceModel() const;

    void updateSettings(const ProjectTreeControllerModeSettings &settings);
    // wraps the ProjectViewModel's method with the same name
    QModelIndex getIndexForDoc(Document *doc) const;

protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    bool filterAcceptsFolder(const Folder *folder) const;

    ProjectTreeControllerModeSettings settings;
};

} // namespace U2

#endif // _U2_PROJECT_FILTER_PROXY_MODEL_H_
