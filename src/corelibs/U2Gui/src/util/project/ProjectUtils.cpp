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

#include "ProjectUtils.h"

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

const int ProjectUtils::MAX_OBJS_TO_SHOW_LOAD_PROGRESS = 100;

bool ProjectUtils::isSystemFolder(const QString& folderPath) {
    return folderPath == U2ObjectDbi::ROOT_FOLDER;
}

Document* ProjectUtils::findDocument(const QString& url) {
    Project* project = AppContext::getProject();
    CHECK(project != nullptr, nullptr);
    return project->findDocumentByURL(url);
}

bool ProjectUtils::hasLoadedDocument(const QString& url) {
    Document* doc = findDocument(url);
    CHECK(doc != nullptr, false);
    return doc->isLoaded();
}

bool ProjectUtils::hasUnloadedDocument(const QString& url) {
    const Document* doc = findDocument(url);
    CHECK(doc != nullptr, false);
    return !doc->isLoaded();
}

LoadUnloadedDocumentTask* ProjectUtils::findLoadTask(const QString& url) {
    Document* doc = findDocument(url);
    CHECK(doc != nullptr, nullptr);
    return LoadUnloadedDocumentTask::findActiveLoadingTask(doc);
}

}  // namespace U2
