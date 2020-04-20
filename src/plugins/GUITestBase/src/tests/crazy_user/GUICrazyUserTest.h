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

#ifndef _U2_GT_TESTS_CRAZY_USER_H_
#define _U2_GT_TESTS_CRAZY_USER_H_

#include <U2Test/UGUITestBase.h>
//#include <U2Core/U2OpStatus.h>
#include "utils/GTUtilsDialog.h"

namespace U2 {

namespace GUITest_crazy_user {

class GTAbstractGUIAction;

class GTCrazyUserMonitor : QObject {
    Q_OBJECT
public:
    GTCrazyUserMonitor() : timer(NULL) {
        timer = new QTimer();

        timer->connect(timer, SIGNAL(timeout()), this, SLOT(checkActiveWidget()));
        timer->start(10000);
    }
    virtual ~GTCrazyUserMonitor() {
        delete timer;
    }

public slots:
    void checkActiveWidget();

private:
    GTCrazyUserMonitor(const GTCrazyUserMonitor&);
    GTCrazyUserMonitor& operator=(const GTCrazyUserMonitor&);

    QTimer *timer;
    QList<GTAbstractGUIAction*> formGUIActions(QWidget* widget) const;
};


class simple_crazy_user : public HI::GUITest {
public:
    simple_crazy_user() : HI::GUITest("simple_crazy_user"){}
protected:
    virtual void run(HI::GUITestOpStatus &os);
};

}

} //namespace

#endif
