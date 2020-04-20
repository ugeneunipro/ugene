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

#ifndef _HI_MAIN_THREAD_RUNNABLE_H_
#define _HI_MAIN_THREAD_RUNNABLE_H_

#include "GTGlobals.h"

#include <core/CustomScenario.h>

namespace HI {

/**
 * Class takes ownership over the scenario.
 */
class HI_EXPORT MainThreadRunnable : public QObject {
    Q_OBJECT
public:
    MainThreadRunnable(GUITestOpStatus &os, CustomScenario *scenario);
    ~MainThreadRunnable();

    void doRequest();
    void run();

    static void runInMainThread(GUITestOpStatus &os, CustomScenario *scenario);

signals:
    void si_request(MainThreadRunnable *mainThreadRunnable);

private:
    GUITestOpStatus &os;
    CustomScenario *scenario;
};

class MainThreadRunnableObject : public QObject {
    Q_OBJECT
public:
    MainThreadRunnableObject();

public slots:
    void sl_requestAsked(MainThreadRunnable *mainThreadRunnable);

};


}   // namespace

#endif
