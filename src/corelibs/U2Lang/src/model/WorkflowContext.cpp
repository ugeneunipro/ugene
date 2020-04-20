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

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMutexLocker>
#include <QTextStream>

#include <U2Core/AppContext.h>
#include <U2Core/AppFileStorage.h>
#include <U2Core/CMDLineRegistry.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Lang/ActorModel.h>
#include <U2Lang/BaseTypes.h>
#include <U2Lang/Datatype.h>
#include <U2Lang/GrouperOutSlot.h>
#include <U2Lang/IntegralBus.h>
#include <U2Lang/WorkflowMonitor.h>
#include <U2Lang/WorkflowSettings.h>

#include "WorkflowContext.h"

namespace U2 {
namespace Workflow {

static QString getWorkflowId(WorkflowContext *ctx) {
    qint64 pid = QApplication::applicationPid();
    QString wId = QByteArray::number(pid) + "_" + QByteArray::number(qint64(ctx));

    return wId;
}

WorkflowContext::WorkflowContext(const QList<Actor*> &procs, WorkflowMonitor *_monitor)
: monitor(_monitor), storage(NULL), process("")
{
    foreach (Actor *p, procs) {
        procMap.insert(p->getId(), p);
    }

    { // register WD process
        AppFileStorage *fileStorage = AppContext::getAppFileStorage();
        CHECK(NULL != fileStorage, );

        U2OpStatusImpl os;
        process = WorkflowProcess(getWorkflowId(this));
        fileStorage->registerWorkflowProcess(process, os);
        CHECK_OP(os, );
    }
}

WorkflowContext::~WorkflowContext() {
    foreach (const QString &url, externalProcessFiles) {
        QFile::remove(url);
    }
    delete storage;

    // unregister WD process
    if (!process.getId().isEmpty()) {
        AppFileStorage *fileStorage = AppContext::getAppFileStorage();
        CHECK(NULL != fileStorage, );

        U2OpStatusImpl os;
        fileStorage->unregisterWorkflowProcess(process, os);
    }
}

bool WorkflowContext::init() {
    storage = new DbiDataStorage();
    CHECK(initWorkingDir(), false);
    return storage->init();
}

DbiDataStorage * WorkflowContext::getDataStorage() {
    return storage;
}

WorkflowMonitor * WorkflowContext::getMonitor() {
    return monitor;
}

void WorkflowContext::addExternalProcessFile(const QString &url) {
    QMutexLocker locker(&addFileMutex);
    externalProcessFiles << url;
}

DataTypePtr WorkflowContext::getOutSlotType(const QString &slotStr) {
    QStringList tokens = slotStr.split(">");
    assert(tokens.size() > 0);
    tokens = tokens[0].split(".");
    assert(2 == tokens.size());

    Actor *proc = procMap.value(tokens[0], NULL);
    if (NULL == proc) {
        return DataTypePtr();
    }

    QString slotId = tokens[1];
    foreach (Port *port, proc->getOutputPorts()) {
        assert(port->getOutputType()->isMap());
        QMap<Descriptor, DataTypePtr> typeMap = port->getOutputType()->getDatatypesMap();

        if (typeMap.keys().contains(slotId)) {
            DataTypePtr type = typeMap.value(slotId);
            assert(DataType::Single == type->kind());
            return type;
        }
    }

    return DataTypePtr();
}

const WorkflowProcess & WorkflowContext::getWorkflowProcess() const {
    return process;
}

WorkflowProcess & WorkflowContext::getWorkflowProcess() {
    return process;
}

QString WorkflowContext::workingDir() const {
    return _workingDir;
}

QString WorkflowContext::absolutePath(const QString &relative) const {
    CHECK(!relative.isEmpty(), "");
    QFileInfo info(relative);
    if (info.isAbsolute()) {
        return info.absoluteFilePath();
    }

    if (relative.startsWith(".") || relative.startsWith("..")) {
        return info.absoluteFilePath();
    }

    return workingDir() + relative;
}

MessageMetadataStorage & WorkflowContext::getMetadataStorage() {
    return metadataStorage;
}

bool WorkflowContext::initWorkingDir() {
    U2OpStatus2Log os;

    QString root = WorkflowContextCMDLine::getOutputDirectory(os);
    CHECK_OP(os, false);

    if (!root.endsWith("/")) {
        root += "/";
    }

    if (WorkflowContextCMDLine::useSubDirs()) {
        QString dirName = WorkflowContextCMDLine::createSubDirectoryForRun(root, os);
        CHECK_OP(os, false);
        _workingDir = root + dirName + "/";
    } else {
        _workingDir = root;
    }
    if (!AppContext::isGUIMode()) {
        WorkflowContextCMDLine::saveRunInfo(workingDir());
    }
    monitor->setOutputDir(workingDir());
    coreLog.details("Workflow output folder is: " + workingDir());
    return true;
}

/************************************************************************/
/* WorkflowContextCMDLine */
/************************************************************************/

const QString WorkflowContextCMDLine::WORKING_DIR = "working-dir";

QString WorkflowContextCMDLine::getOutputDirectory(U2OpStatus &os) {
    // 1. Detect folder
    QString root;

    CMDLineRegistry* cmdlineReg = AppContext::getCMDLineRegistry();
    assert(cmdlineReg != nullptr);

    if (useOutputDir()) {
        root = WorkflowSettings::getWorkflowOutputDirectory();
    } else if (cmdlineReg != nullptr && cmdlineReg->hasParameter(WORKING_DIR)) {
        root = FileAndDirectoryUtils::getAbsolutePath(cmdlineReg->getParameterValue(WORKING_DIR));
    } else{
        root = QProcess().workingDirectory();
    }

    // 2. Create folder if it does not exist
    QDir rootDir(root);
    if (!rootDir.exists()) {
        bool created = rootDir.mkpath(rootDir.absolutePath());
        if (!created) {
            os.setError(QObject::tr("Can not create folder: ") + root);
            return "";
        }
    }
    return rootDir.absolutePath();
}

QString WorkflowContextCMDLine::createSubDirectoryForRun(const QString &root, U2OpStatus &os) {
    QDir rootDir(root);
    // 1. Find free sub-folder name
    QString baseDirName = QDateTime::currentDateTime().toString("yyyy.MM.dd_hh-mm");
    QString dirName = baseDirName;
    {
        int counter = 1;
        while (rootDir.exists(dirName)) {
            dirName = QString("%1_%2").arg(baseDirName).arg(QString::number(counter));
            counter++;
        }
    }

    // 2. Try to create the sub-folder
    bool created = rootDir.mkdir(dirName);
    if (!created) {
        os.setError(QObject::tr("Can not create folder %1 in the folder %2")
            .arg(dirName).arg(rootDir.absolutePath()));
        return "";
    }
    return dirName;
}

bool WorkflowContextCMDLine::useOutputDir() {
    return AppContext::isGUIMode();
}

bool WorkflowContextCMDLine::useSubDirs() {
    return useOutputDir();
}

void WorkflowContextCMDLine::saveRunInfo(const QString &dir) {
    QFile runInfo(dir + "run.info");
    bool opened = runInfo.open(QIODevice::WriteOnly);
    CHECK(opened, );

    QTextStream stream(&runInfo);
    stream.setCodec("UTF-8");
    stream << QCoreApplication::arguments().join(" ") + "\n";
    stream.flush();

    runInfo.close();
}

} // Workflow
} // U2
