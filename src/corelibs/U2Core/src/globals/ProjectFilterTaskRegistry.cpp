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

#include <QMutexLocker>

#include <U2Core/AbstractProjectFilterTask.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include "ProjectFilterTaskRegistry.h"

namespace U2 {

ProjectFilterTaskRegistry::~ProjectFilterTaskRegistry() {
    QMutexLocker guard(&lock);
    qDeleteAll(factories);
}

QList<AbstractProjectFilterTask *> ProjectFilterTaskRegistry::createFilterTasks(const ProjectTreeControllerModeSettings &settings,
    const QList<QPointer<Document> > &docs)
{
    QMutexLocker guard(&lock);

    QList<AbstractProjectFilterTask *> result;
    foreach (ProjectFilterTaskFactory *factory, factories) {
        result.append(factory->registerNewTask(settings, docs));
    }
    return result;
}

void ProjectFilterTaskRegistry::registerTaskFactory(ProjectFilterTaskFactory *factory) {
    SAFE_POINT(NULL != factory, L10N::nullPointerError("Project filter task factory"), );
    SAFE_POINT(!factories.contains(factory), "Attempting to register a factory twice", );

    QMutexLocker guard(&lock);
    factories.append(factory);
}

} // namespace U2
