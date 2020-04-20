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

#ifndef _U2_CONDUCT_GO_TASK_
#define _U2_CONDUCT_GO_TASK_

#include <U2Core/AnnotationData.h>
#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/Task.h>

#include "ConductGOSettings.h"

namespace U2 {

class AnnotationTableObject;
class Document;
class SaveDocumentTask;

class ConductGOTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    ConductGOTask(const ConductGOSettings& settings);
    virtual ~ConductGOTask();

    virtual void prepare();
    virtual void run();
    virtual void cleanup();

    const ConductGOSettings & getSettings() const ;
    QStringList getResultFileNames() const;

private:
    ConductGOSettings settings;
    QString workingDir;

    ExternalToolRunTask *etTask;

    static const QString BASE_DIR_NAME;
    static const QString BASE_SUBDIR_NAME;
    static const QString TREAT_NAME;

private:
    bool copyFile(const QString &src, const QString &dst);
};

} // U2

#endif // _U2_CONDUCT_GO_TASK_
