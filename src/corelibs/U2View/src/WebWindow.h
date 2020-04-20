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

#ifndef _U2_WEB_WINDOW_H_
#define _U2_WEB_WINDOW_H_

#include <U2Core/global.h>
#include <U2Gui/MainWindow.h>

class QTextBrowser;

namespace U2 {

/**
 * A window with a simple HTML content.
 * Note: HTML support is limited to the features supported by QTextBrowser component.
 * https://doc.qt.io/qt-5/richtext-html-subset.html
 */
class U2VIEW_EXPORT WebWindow : public MWMDIWindow {
    Q_OBJECT
public:
    WebWindow(const QString& title, const QString& htmlContent);
    QTextBrowser* textBrowser;
};

}//namespace
#endif
