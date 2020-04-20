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

#include <U2Core/U2SafePoints.h>

#include "WorkflowTasksRegistry.h"

namespace U2 {
namespace Workflow {

WorkflowTasksRegistry::WorkflowTasksRegistry() {

}

WorkflowTasksRegistry::~WorkflowTasksRegistry() {
    foreach (const QString &id, readTasks.keys()) {
        ReadDocumentTaskFactory *factory = readTasks.value(id);
        delete factory;
    }
    readTasks.clear();
}

bool WorkflowTasksRegistry::registerReadDocumentTaskFactory(ReadDocumentTaskFactory *factory) {
    SAFE_POINT(NULL != factory, "NULL ReadDocumentTaskFactory", false);
    SAFE_POINT(!readTasks.contains(factory->getId()),
        QString("Double ReadDocumentTaskFactory registering: %1").arg(factory->getId()), false);

    readTasks[factory->getId()] = factory;
    return true;
}

ReadDocumentTaskFactory *WorkflowTasksRegistry::getReadDocumentTaskFactory(const QString &id) {
    return readTasks.value(id, NULL);
}

} // Workflow
} // U2

