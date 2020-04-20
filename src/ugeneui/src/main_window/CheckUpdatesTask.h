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

#ifndef _U2_CHECK_UPDATES_TASKS_H_
#define _U2_CHECK_UPDATES_TASKS_H_

#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Task.h>
#include <U2Core/Version.h>
#include <QMessageBox>

class QPushButton;

namespace U2 {

class CheckUpdatesTask : public Task {
    Q_OBJECT
public slots:
    void sl_registerInTaskScheduler();
public:
    enum Answer {Update, DoNothing, Skip};

    CheckUpdatesTask(bool startUp = false);
    void run();
    ReportResult report();

private:
    Version siteVersion;
    bool    runOnStartup;
    bool    startError;
};

class UpdateMessage : public QObject {
    Q_OBJECT
public:
    UpdateMessage(const QString &newVersion);
    CheckUpdatesTask::Answer getAnswer() const;

private:
    QObjectScopedPointer<QMessageBox> dialog;
    QPushButton *updateButton;
    QPushButton *postponeButton;
};

class VersionMessage : public QObject {
    Q_OBJECT
public:
    VersionMessage(const Version &newVersion);
    CheckUpdatesTask::Answer getAnswer() const;

private:
    QString getMessageText(const Version &thisVersion, const Version &newVersion) const;

private:
    QObjectScopedPointer<QMessageBox> dialog;
    QPushButton *updateButton;
};

}//namespace

#endif
