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

#include "primitives/GTMainWindow.h"
#include "utils/GTUtilsApp.h"

#include <QMainWindow>

namespace HI {

#define GT_CLASS_NAME "GTUtilsApp"

#define GT_METHOD_NAME "checkUGENETitle"
void GTUtilsApp::checkUGENETitle(GUITestOpStatus &os, const QString &title) {

    bool isTitleGood = false;
    foreach(QWidget* w, GTMainWindow::getMainWindowsAsWidget(os)){
        QString ugeneTitle = w->windowTitle();
        if(ugeneTitle == title){
            isTitleGood = true;
        }
    }

    GT_CHECK(isTitleGood, "UGENE title not <" + title + ">");
}

#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

#define GT_CLASS_NAME "GTUtilsApp"

#define GT_METHOD_NAME "checkUGENETitleContains"
void GTUtilsApp::checkUGENETitleContains(GUITestOpStatus &os, const QString& string) {

    bool isTitleContains = false;
    foreach(QWidget* w, GTMainWindow::getMainWindowsAsWidget(os)){
        QString ugeneTitle = w->windowTitle();
        if(ugeneTitle.contains(string)){
            isTitleContains = true;
        }
    }

    GT_CHECK(isTitleContains, "UGENE title not contains <" + string + ">");
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}
