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

#ifndef _U2_PROJECT_FILTER_TASK_REGISTRY_H_
#define _U2_PROJECT_FILTER_TASK_REGISTRY_H_

#include <QMutex>

#include <U2Core/DocumentModel.h>

namespace U2 {

class AbstractProjectFilterTask;
class ProjectFilterTaskFactory;
class ProjectTreeControllerModeSettings;

class U2CORE_EXPORT ProjectFilterTaskRegistry {
public:
    virtual ~ProjectFilterTaskRegistry();

    QList<AbstractProjectFilterTask *> createFilterTasks(const ProjectTreeControllerModeSettings &settings,
        const QList<QPointer<Document> > &docs);
    void registerTaskFactory(ProjectFilterTaskFactory *factory);

private:
    QMutex lock;
    QList<ProjectFilterTaskFactory *> factories;
};

}

#endif // _U2_PROJECT_FILTER_TASK_REGISTRY_H_
