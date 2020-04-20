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

#ifndef GUITESTRUNNER_H
#define GUITESTRUNNER_H

#include <QWidget>
#include "ui_GUITestRunner.h"
#include <U2Test/UGUITestBase.h>

namespace U2 {

class GUITestRunner : public QWidget, public Ui_GUITestRunner
{
    Q_OBJECT

public:
    explicit GUITestRunner(UGUITestBase* guiTestBase, QWidget *parent = 0);
    ~GUITestRunner();

private slots:
    void sl_runSelected();
    void sl_runAllGUITests();
    void sl_testFinished();
    void sl_filterChanged(const QString &nameFilter);
    void sl_filterCleared();

private:
    void revisible(const QString &nameFilter);

    UGUITestBase*    guiTestBase;
    QAction*        delTextAction;
};

}

#endif // GUITESTRUNNER_H
