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

#ifndef _TASK_STATUS_BAR_H_
#define _TASK_STATUS_BAR_H_

#include <U2Core/Task.h>
#include <U2Gui/Notification.h>
#include <U2Core/Log.h>

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QPixmap>

namespace U2 {

class MainWindowImpl;

class TaskStatusBar : public QWidget {
    Q_OBJECT

public:
    TaskStatusBar();

private slots:
    void sl_taskStateChanged(Task* t);
    void sl_taskStateChanged();
    void sl_taskProgressChanged();
    void sl_taskDescChanged();
    void sl_reportsCountChanged();
    void sl_newReport(Task*);

    void sl_showReport();
    void sl_notificationChanged();

protected:
    bool eventFilter(QObject *o, QEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void drawProgress(QLabel* l);
    
private:
    void updateState();
    void setTaskToTrack(Task* t);

    Task*           taskToTrack;
    QHBoxLayout*    l;
    QLabel*         taskInfoLabel;
    QProgressBar*   taskProgressBar;
    QLabel*         taskCountLabel;
    QLabel*         lampLabel;
    QLabel*         notificationLabel;
    QPixmap         iconOn;
    QPixmap         iconOff;
    int             nReports;
    bool            tvConnected;
    NotificationStack*   nStack;  
    QPixmap         notificationEmpty;
    QPixmap         notificationReport;
    QPixmap         notificationError;
};


}//namespace

#endif
