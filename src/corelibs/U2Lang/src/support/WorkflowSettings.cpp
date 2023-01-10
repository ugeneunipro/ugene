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

#include "WorkflowSettings.h"

#include <QApplication>
#include <QColor>
#include <QDir>
#include <QSettings>
#include <QStyle>

#include <U2Core/AppContext.h>
#include <U2Core/CMDLineUtils.h>
#include <U2Core/GUrl.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

#define SETTINGS QString("workflowview/")
#define GRID_STATE SETTINGS + "showGrid"
#define SNAP_STATE SETTINGS + "snap2rid"
#define LOCK_STATE SETTINGS + "monitorRun"
#define DEBUGGER_STATE SETTINGS + "enableDebugger"
#define STYLE SETTINGS + "style"
#define FONT SETTINGS + "font"
#define DIR "workflow_settings/path"
#define BG_COLOR SETTINGS + "bgcolor"
#define SCRIPT_MODE SETTINGS + "scriptMode"
#define EXTERNAL_TOOL_WORKER_PATH SETTINGS + "externalToolWorkerPath"
#define INCLUDED_WORKER_PATH SETTINGS + "includedWorkerPath"
#define WORKFLOW_OUTPUT_PATH SETTINGS + "workflowOutputPath"

Watcher* const WorkflowSettings::watcher = new Watcher;

bool WorkflowSettings::showGrid() {
    return AppContext::getSettings()->getValue(GRID_STATE, true).toBool();
}

void WorkflowSettings::setShowGrid(bool v) {
    if (showGrid() != v) {
        AppContext::getSettings()->setValue(GRID_STATE, v);
        emit watcher->changed();
    }
}

bool WorkflowSettings::snap2Grid() {
    return AppContext::getSettings()->getValue(SNAP_STATE, true).toBool();
}

void WorkflowSettings::setSnap2Grid(bool v) {
    AppContext::getSettings()->setValue(SNAP_STATE, v);
}

bool WorkflowSettings::monitorRun() {
    return AppContext::getSettings()->getValue(LOCK_STATE, true).toBool();
}

void WorkflowSettings::setMonitorRun(bool v) {
    AppContext::getSettings()->setValue(LOCK_STATE, v);
}

bool WorkflowSettings::isDebuggerEnabled() {
    return AppContext::getSettings()->getValue(DEBUGGER_STATE, false).toBool();
}

void WorkflowSettings::setDebuggerEnabled(bool v) {
    AppContext::getSettings()->setValue(DEBUGGER_STATE, v);
}

QString WorkflowSettings::defaultStyle() {
    return AppContext::getSettings()->getValue(STYLE, "ext").toString();
}

void WorkflowSettings::setDefaultStyle(const QString& s) {
    if (s != defaultStyle()) {
        AppContext::getSettings()->setValue(STYLE, s);
        emit watcher->changed();
    }
}

QFont WorkflowSettings::defaultFont() {
    return AppContext::getSettings()->getValue(FONT, true).value<QFont>();
}

void WorkflowSettings::setDefaultFont(const QFont& f) {
    if (defaultFont() != f) {
        AppContext::getSettings()->setValue(FONT, QVariant::fromValue<QFont>(f));
        emit watcher->changed();
    }
}

/** TODO: make this method global for UGENE and use it everywhere. */
static QString getDataDirPath() {
    QString dataDir = qgetenv("UGENE_DATA_PATH");
    if (!dataDir.isEmpty()) {
        return dataDir;
    }
    QStringList paths = QDir::searchPaths(PATH_PREFIX_DATA);
    return paths.first();
}

const QString WorkflowSettings::getUserDirectory() {
    Settings* s = AppContext::getSettings();
    QString defaultPath = getDataDirPath() + "/workflow_samples/users/";
    QString path = s->getValue(DIR, defaultPath, true).toString();
    return path;
}

void WorkflowSettings::setUserDirectory(const QString& newDir) {
    Settings* s = AppContext::getSettings();
    QString defaultPath = getDataDirPath() + "/workflow_samples/users/";
    QString path = s->getValue(DIR, defaultPath, true).toString();
    QString newFixedDir = GUrlUtils::getSlashEndedPath(QDir::fromNativeSeparators(newDir));

    AppContext::getSettings()->setValue(DIR, newFixedDir, true);

    if (path != newFixedDir) {
        QDir dir(path);
        if (!dir.exists()) {
            return;
        }
        dir.setNameFilters(QStringList() << "*.usa");
        QFileInfoList fileList = dir.entryInfoList();
        foreach (const QFileInfo& fileInfo, fileList) {
            QString newFileUrl = newFixedDir + fileInfo.fileName();
            QFile::copy(fileInfo.filePath(), newFileUrl);
        }
    }
}

bool WorkflowSettings::isOutputDirectorySet() {
    Settings* s = AppContext::getSettings();
    return s->contains(WORKFLOW_OUTPUT_PATH);
}

void WorkflowSettings::setWorkflowOutputDirectory(const QString& newDir) {
    QString newFixedDir = GUrlUtils::getSlashEndedPath(QDir::fromNativeSeparators(newDir));
    AppContext::getSettings()->setValue(WORKFLOW_OUTPUT_PATH, newFixedDir);
}

const QString WorkflowSettings::getWorkflowOutputDirectory() {
    QString defaultWorkflowOutputDir = qgetenv("UGENE_WORKFLOW_OUTPUT_DIR");
    if (defaultWorkflowOutputDir.isEmpty()) {
        defaultWorkflowOutputDir = QDir::homePath() + "/workflow_output/";
    }
    return AppContext::getSettings()->getValue(WORKFLOW_OUTPUT_PATH, defaultWorkflowOutputDir).toString();
}

const QString WorkflowSettings::getExternalToolDirectory() {
    Settings* s = AppContext::getSettings();
    GUrl url(s->fileName());
    QString defaultPath = url.dirPath();
    defaultPath += "/ExternalToolConfig/";
    QString path = s->getValue(EXTERNAL_TOOL_WORKER_PATH, defaultPath, true).toString();
    return path;
}

void WorkflowSettings::setExternalToolDirectory(const QString& newDir) {
    Settings* s = AppContext::getSettings();
    GUrl url(s->fileName());
    QString defaultPath = url.dirPath();
    defaultPath += "/ExternalToolConfig/";
    QString path = s->getValue(EXTERNAL_TOOL_WORKER_PATH, defaultPath, true).toString();
    QString newFixedDir = GUrlUtils::getSlashEndedPath(QDir::fromNativeSeparators(newDir));

    s->setValue(EXTERNAL_TOOL_WORKER_PATH, newFixedDir, true);

    if (path != newFixedDir) {
        QDir dir(path);
        if (!dir.exists()) {
            return;
        }
        dir.setNameFilters(QStringList() << "*.etc");
        QFileInfoList fileList = dir.entryInfoList();
        foreach (const QFileInfo& fileInfo, fileList) {
            QString newFileUrl = newFixedDir + fileInfo.fileName();
            QFile::copy(fileInfo.filePath(), newFileUrl);
        }
    }
}

QColor WorkflowSettings::getBGColor() {
    Settings* s = AppContext::getSettings();
    QColor ret(Qt::darkCyan);
    ret.setAlpha(200);
    int r, g, b, a;
    ret.getRgb(&r, &g, &b, &a);
    QString defaultColor = QString::number(r) + "," + QString::number(g) + "," + QString::number(b) + "," + QString::number(a);
    QString color = s->getValue(BG_COLOR, defaultColor).toString();
    QStringList lst = color.split(",");
    if (lst.size() != 4) {
        return ret;
    }

    r = lst[0].toInt();
    g = lst[1].toInt();
    b = lst[2].toInt();
    a = lst[3].toInt();
    QColor res(r, g, b, a);
    return res;
}

void WorkflowSettings::setBGColor(const QColor& color) {
    int r, g, b, a;
    color.getRgb(&r, &g, &b, &a);
    QString newColor = QString::number(r) + "," + QString::number(g) + "," + QString::number(b) + "," + QString::number(a);
    Settings* s = AppContext::getSettings();
    s->setValue(BG_COLOR, newColor);
}

bool WorkflowSettings::getScriptingMode() {
    return AppContext::getSettings()->getValue(SCRIPT_MODE, QVariant(false)).value<bool>();
}

void WorkflowSettings::setScriptingMode(bool md) {
    AppContext::getSettings()->setValue(SCRIPT_MODE, md);
}

void WorkflowSettings::setIncludedElementsDirectory(const QString& newDir) {
    QString newFixedDir = GUrlUtils::getSlashEndedPath(QDir::fromNativeSeparators(newDir));
    AppContext::getSettings()->setValue(INCLUDED_WORKER_PATH, newFixedDir, true);
}

const QString WorkflowSettings::getIncludedElementsDirectory() {
    Settings* s = AppContext::getSettings();
    GUrl url(s->fileName());
    QString defaultPath = url.dirPath();
    defaultPath += "/IncludedWorkers/";
    QString path = s->getValue(INCLUDED_WORKER_PATH, defaultPath, true).toString();
    return path;
}

}  // namespace U2
