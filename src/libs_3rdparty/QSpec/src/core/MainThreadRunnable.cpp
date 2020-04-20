/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
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
#include <QThread>

#include "MainThreadRunnable.h"

namespace HI {

MainThreadRunnable::MainThreadRunnable(GUITestOpStatus &os, CustomScenario *scenario) :
    QObject(NULL),
    os(os),
    scenario(scenario)
{
    if (Q_UNLIKELY(NULL == scenario)) {
        os.setError("Scenario is NULL");
        return;
    }
}

MainThreadRunnable::~MainThreadRunnable() {
    delete scenario;
}

void MainThreadRunnable::doRequest() {
    if (QThread::currentThread() == QApplication::instance()->thread()) {
        run();
        return;
    }

    MainThreadRunnableObject obj;

    obj.moveToThread(QApplication::instance()->thread());

    QMetaObject::Connection connection = connect(this,
                                                 SIGNAL(si_request(MainThreadRunnable *)),
                                                 &obj,
                                                 SLOT(sl_requestAsked(MainThreadRunnable *)),
                                                 Qt::BlockingQueuedConnection);

    emit si_request(this);
    disconnect(connection);

}

void MainThreadRunnable::run() {
    if (Q_UNLIKELY(NULL == scenario)) {
        os.setError("Scenario is NULL");
        return;
    }
    scenario->run(os);
}

void MainThreadRunnable::runInMainThread(GUITestOpStatus &os, CustomScenario *scenario) {
    if (Q_UNLIKELY(NULL == scenario)) {
        os.setError("Custom scenario is NULL");
        return;
    }
    MainThreadRunnable mainThreadRunnable(os, scenario);
    mainThreadRunnable.doRequest();
}

MainThreadRunnableObject::MainThreadRunnableObject() :
    QObject(NULL)
{
}

void MainThreadRunnableObject::sl_requestAsked(MainThreadRunnable *runnable){
    runnable->run();
}

}   // namespace
