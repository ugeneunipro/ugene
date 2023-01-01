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

#ifndef _U2_STATUS_DASHBOARD_WIDGET_H_
#define _U2_STATUS_DASHBOARD_WIDGET_H_

#include <QDomElement>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QWidget>

#include <U2Core/global.h>

#include <U2Lang/WorkflowMonitor.h>

namespace U2 {

class U2DESIGNER_EXPORT StatusDashboardWidget : public QWidget {
    Q_OBJECT
public:
    StatusDashboardWidget(const QDomElement& dom, const WorkflowMonitor* monitor = nullptr);

    static bool isValidDom(const QDomElement& dom);

    void startTimer();

    void stopTimer();

    QString toHtml() const;

private slots:
    void sl_progressChanged(int progress);
    void sl_taskStateChanged(Monitor::TaskState newState);
    void sl_timerEvent();

private:
    static QString state2StatusMessage(const Monitor::TaskState& state);
    static QString state2LabelStyle(const Monitor::TaskState& state);
    void updateTimeLabel();

    const WorkflowMonitor* monitor;
    QProgressBar* progressBar;
    QLabel* statusMessageLabel;
    QLabel* timeLabel;
    QString timeText;
    QString statusMessage;
    Monitor::TaskState state;
    QTimer timer;
    qint64 timerStartMillis;
};

}  // namespace U2

#endif
