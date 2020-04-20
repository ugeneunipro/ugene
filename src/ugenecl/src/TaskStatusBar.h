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

#ifndef _TASK_STATUS_BAR_CON_H_
#define _TASK_STATUS_BAR_CON_H_

#include <U2Core/Task.h>
#define TSB_SETTINGS_ROOT QString("task_status_bar_settings/")
namespace U2 {


class TaskStatusBarCon : public QObject {
    Q_OBJECT
public:
    static const QString NO_TASK_STATUS_BAR_CMD_OPTION;

private:
    static bool helpRegistered;
    static void setTSBCmdlineHelp();

public:
    TaskStatusBarCon();
    ~TaskStatusBarCon();

private slots:
    void sl_taskStateChanged(Task* t);
    void sl_taskStateChanged();
    void sl_update();

private:
    void setTSBSettings();
    void updateState();
    void setTaskToTrack(Task* t);

    Task*           taskToTrack;
    QString         emptyLine;
};


}//namespace

#endif
