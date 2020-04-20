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

#ifndef _U2_PROJECT_SERVICE_H_
#define _U2_PROJECT_SERVICE_H_

#include <U2Core/ProjectModel.h>
#include <U2Core/ServiceTypes.h>
#include <U2Core/Task.h>

namespace U2 {

enum SaveProjectTaskKind {
    SaveProjectTaskKind_SaveProjectOnly,
    SaveProjectTaskKind_SaveProjectAndDocuments,
    SaveProjectTaskKind_SaveProjectAndDocumentsAskEach
};



class U2CORE_EXPORT ProjectService : public Service {
public:
    ProjectService(const QString& sname, const QString& sdesc)
        : Service(Service_Project, sname, sdesc, QList<ServiceType>(), ServiceFlag_Singleton) {}

    virtual Project* getProject() const = 0;

    virtual Task* saveProjectTask(SaveProjectTaskKind kind) = 0;

    virtual Task* closeProjectTask() = 0;

    // hack to work around ambigous shortcuts
    virtual void enableSaveAction(bool e) = 0;
};

}//namespace

#endif

