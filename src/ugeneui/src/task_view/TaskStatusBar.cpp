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

#include "TaskStatusBar.h"

#include "TaskViewController.h"

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>
#include <U2Gui/MainWindow.h>

#include <QEvent>

#include <QPainter>
#include <QIcon>

#include <math.h>

namespace U2 {

TaskStatusBar::TaskStatusBar() {
    nReports = 0;
    tvConnected = false;
    taskToTrack = NULL;
    taskProgressBar = NULL;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    l = new QHBoxLayout();
    l->setMargin(0);
    setLayout(l);

    l->addSpacing(20);

    taskInfoLabel = new QLabel();
    taskInfoLabel->setTextFormat(Qt::PlainText);
    taskInfoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    taskInfoLabel->setObjectName("taskInfoLabel");
    l->addWidget(taskInfoLabel);

    taskCountLabel = new QLabel();
    taskCountLabel->setTextFormat(Qt::PlainText);
    taskCountLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    taskCountLabel->setMinimumWidth(100);
    taskCountLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    taskCountLabel->setObjectName("taskCountLabel");
    l->addWidget(taskCountLabel);

    notificationEmpty = QPixmap(":ugene/images/empty_notification.png");
    notificationReport = QPixmap(":ugene/images/has_report.png");
    notificationError = QPixmap(":ugene/images/has_error.png");

    lampLabel = new QLabel();
    notificationLabel = new QLabel();
    notificationLabel->setPixmap(notificationEmpty);
    l->addWidget(lampLabel);
    l->addWidget(notificationLabel);


#ifdef Q_OS_MAC
    l->addSpacing(16);
#endif

    iconOn = QIcon(":ugene/images/lightbulb.png").pixmap(16, 16);
    iconOff = QIcon(":ugene/images/lightbulb_off.png").pixmap(16, 16);

    connect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), SLOT(sl_taskStateChanged(Task*)));
    connect(AppContext::getTaskScheduler(), SIGNAL(si_topLevelTaskUnregistered(Task*)), SLOT(sl_newReport(Task*)));

    nStack = AppContext::getMainWindow()->getNotificationStack();
    //nStack = new NotificationStack;
    connect(nStack, SIGNAL(si_changed()), SLOT(sl_notificationChanged()));

    lampLabel->installEventFilter(this);
    taskCountLabel->installEventFilter(this);
    notificationLabel->installEventFilter(this);

    taskProgressBar = new QProgressBar();
    taskProgressBar->setRange(0, 100);
    taskProgressBar->setValue(0);
    taskProgressBar->setFixedWidth(120);
    taskProgressBar->setFixedHeight(16);
    taskProgressBar->setObjectName("taskProgressBar");
    l->insertWidget(2, taskProgressBar);

    setObjectName("taskStatusBar");
    updateState();
}

namespace {
    NotificationType getNotificationType(const U2OpStatus &os) {
        if (os.hasError()) {
            return Error_Not;
        }
        if (os.hasWarnings()) {
            return Warning_Not;
        }
        return Report_Not;
    }
}

void TaskStatusBar::sl_newReport(Task* task) {
    Notification *t = NULL;
    if (task->isReportingEnabled()) {
        NotificationType nType = getNotificationType(task->getStateInfo());
        if (task->isNotificationReport()) {
            t = new Notification(tr("The task '%1' has been finished").arg(task->getTaskName()), nType);
        } else {
            QAction *action = new QAction("action", this);
            action->setData(QVariant(task->getTaskName() + "|" + QString::number(task->getTaskId()) + "|" + TVReportWindow::prepareReportHTML(task)));
            connect(action, SIGNAL(triggered()), SLOT(sl_showReport()));
            t = new Notification(tr("Report for task: '%1'").arg(task->getTaskName()), nType, action);
        }
    } else if (task->hasError() && !task->isErrorNotificationSuppressed()) {
        t = new Notification(tr("'%1' task failed: %2").arg(task->getTaskName()).arg(task->getError()), Error_Not);
    } else if (task->getStateInfo().hasWarnings()) {
        QStringList warnings = task->getWarnings();
        t = new Notification(tr("There %1:\n")
                                           .arg(warnings.size() == 1
                                                ? "was 1 warning"
                                                : QString("were %1 warnings").arg(warnings.size())) + warnings.join("\n"), Warning_Not);
    }
    if (NULL != t) {
        nStack->addNotification(t);
    }
}

void TaskStatusBar::sl_reportsCountChanged() {
    TaskViewDockWidget* twd = qobject_cast<TaskViewDockWidget*>(sender());
    nReports = twd->countAvailableReports();
    updateState();
}

void TaskStatusBar::sl_showReport() {
    QAction *a = qobject_cast<QAction*>(sender());
    QString str = a->data().toString();

    if(str.split("|").size() == 3) {
        QString taskName = str.split("|")[0];
        MWMDIManager* mdi = AppContext::getMainWindow()->getMDIManager();

        /*foreach(MWMDIWindow *wnd, mdi->getWindows()) {
            if(wnd->windowTitle() == TVReportWindow::genWindowName(taskName)) {
                return;
            }
        }*/
        MWMDIWindow* w = new TVReportWindow(taskName, str.split("|")[1].toInt(), str.split("|")[2]);
        mdi->addMDIWindow(w);
    }
}

void TaskStatusBar::updateState() {
    QString reportsString = nReports == 0 ? QString("") : tr("Reports: %1").arg(nReports);
    if (taskToTrack ==  NULL) {
        taskInfoLabel->setText("");
        taskProgressBar->setVisible(false);
        if (nReports == 0) {
            taskCountLabel->setText(tr("No active tasks"));
        } else {
            taskCountLabel->setText(reportsString);
        }
        lampLabel->setPixmap(iconOff);
        return;
    }

    QString desc = taskToTrack->getStateInfo().getDescription();
    QString text = tr("Running task: %1").arg(taskToTrack->getTaskName());
    if (taskToTrack->isCanceled() && !taskToTrack->isFinished()) {
        QString cancelStr = tr("canceling...");
        if (!desc.isEmpty()) {
            cancelStr=", " + cancelStr;
        }
        desc+=cancelStr;
    }
    if (!desc.isEmpty()) {
        text+=tr(": %1").arg(desc);
    }
    taskInfoLabel->setText(text);
    int nTasks = AppContext::getTaskScheduler()->getTopLevelTasks().size();
    if (nReports > 0) {
        taskCountLabel->setText(tr("Tasks: %1, Reports %2").arg(nTasks).arg(nReports));
    } else {
        taskCountLabel->setText(tr("Tasks: %1").arg(nTasks));
    }

    taskProgressBar->setVisible(true);
    int progress = taskToTrack->getProgress();
    if (progress != -1 ) {
        taskProgressBar->setValue(progress);
    }

    lampLabel->setPixmap(iconOn);
}

void TaskStatusBar::sl_taskStateChanged(Task* t) {
    assert(taskToTrack == NULL);
    if (t->isFinished()) {
        return;
    }
    setTaskToTrack(t);
    //AppContext::getTaskScheduler()->disconnect(this);
    disconnect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), this, SLOT(sl_taskStateChanged(Task*)));
}

void TaskStatusBar::setTaskToTrack(Task* t) {
    assert(taskToTrack == NULL);
    if (Q_UNLIKELY(NULL != taskToTrack)) {
        disconnect(taskToTrack, NULL, this, NULL);
    }
    taskToTrack = t;
    connect(taskToTrack, SIGNAL(si_stateChanged()), SLOT(sl_taskStateChanged()));
    connect(taskToTrack, SIGNAL(si_progressChanged()), SLOT(sl_taskProgressChanged()));
    connect(taskToTrack, SIGNAL(si_descriptionChanged()), SLOT(sl_taskDescChanged()));
    updateState();
}

void TaskStatusBar::sl_taskStateChanged() {
    if (!tvConnected) {
        QWidget* w = AppContext::getMainWindow()->getDockManager()->findWidget(DOCK_TASK_VIEW);
        if (w!=NULL) {
            TaskViewDockWidget* twd = qobject_cast<TaskViewDockWidget*>(w);
            nReports = twd->countAvailableReports();
            connect(twd, SIGNAL(si_reportsCountChanged()), SLOT(sl_reportsCountChanged()));
            tvConnected = true;
        }
    }
    assert(taskToTrack == sender());
    if (!taskToTrack->isFinished()) {
        updateState();
        return;
    }
    taskToTrack->disconnect(this);
    taskToTrack = NULL;
    taskProgressBar->setValue(false);

    foreach(Task* newT, AppContext::getTaskScheduler()->getTopLevelTasks()) {
        if (!newT->isFinished()) {
            setTaskToTrack(newT);
            break;
        }
    }
    if (taskToTrack == NULL) {
        connect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), SLOT(sl_taskStateChanged(Task*)));
    }
    updateState();
}

bool TaskStatusBar::eventFilter(QObject *o, QEvent *e) {
    Q_UNUSED(o);
    QEvent::Type t = e->type();
    if (t == QEvent::MouseButtonDblClick) {
        if(o == notificationLabel) {
            nStack->showStack();
        } else {
            AppContext::getMainWindow()->getDockManager()->toggleDock(DOCK_TASK_VIEW);
        }
    } else if(t == QEvent::ToolTip && o == notificationLabel) {
        QHelpEvent *hEvent = static_cast<QHelpEvent*>(e);
        QToolTip::showText(hEvent->globalPos(), tr("%1 notification(s)").arg(nStack->count()));
    }
    return false;
}

void TaskStatusBar::mouseDoubleClickEvent(QMouseEvent *e) {
    if (taskToTrack!=NULL) {
        QWidget* w = AppContext::getMainWindow()->getDockManager()->activateDock(DOCK_TASK_VIEW);
        if (w!=NULL) {
            TaskViewDockWidget* twd = qobject_cast<TaskViewDockWidget*>(w);
            twd->selectTask(taskToTrack);
        }
    }
    QWidget::mouseDoubleClickEvent(e);
}

void TaskStatusBar::sl_notificationChanged() {
    if(nStack->count() == 0) {
        notificationLabel->setPixmap(notificationEmpty);
    } else  {
        QPixmap iconWithNumber;

        if(nStack->hasError()) {
            iconWithNumber = notificationError;

        } else {
            iconWithNumber = notificationReport;
        }

        QPainter painter(&iconWithNumber);
        painter.setPen(Qt::black);
        QFont font("Arial", 7);
        font.setBold(true);
        painter.setFont(font);
        QRect rect(0,0,16,16);
        painter.drawText(rect, Qt::AlignRight, QString::number(nStack->count()));
        painter.end();
        notificationLabel->setPixmap(iconWithNumber);
    }
}

void TaskStatusBar::sl_taskProgressChanged() {
    CHECK(sender() != NULL, );
    SAFE_POINT(taskToTrack == sender(), tr("Wrong signal sender!"), );
    updateState();
}

void TaskStatusBar::sl_taskDescChanged() {
    assert(taskToTrack == sender());
    updateState();
}

void TaskStatusBar::drawProgress(QLabel* label) {
    static QColor piecolor("#fdc689");

    int percent = taskToTrack->getStateInfo().progress;
    int h = height()-2;
    //float radius = h / 2;
    QPixmap pix(h, h);
    QPainter p(&pix);

    p.fillRect(pix.rect(), palette().window().color());

    p.setPen(piecolor);
    p.setBrush(piecolor);
    p.drawPie(pix.rect(), -90, qRound(- percent * 57.60));

    p.setPen(Qt::black);
    p.drawText(pix.rect(), Qt::AlignCenter | Qt::TextDontClip, QString("%1").arg(percent));

    label->setPixmap(pix);
}

} //namespace
