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

#ifndef _HI_GUI_GTWIDGET_H_
#define _HI_GUI_GTWIDGET_H_

#include "GTGlobals.h"

#include <QAbstractButton>
#include <QPushButton>
#include <QWidget>
#include <QLabel>

namespace HI {
/*!
 * \brief The class for working with QWidget primitive
 */
class HI_EXPORT GTWidget {
public:
    // fails if widget is NULL, not visible or not enabled; if p isNull, clicks on the center of widget
    static void click(GUITestOpStatus &os, QWidget *w, Qt::MouseButton mouseButton = Qt::LeftButton, QPoint p = QPoint(), bool safe = true);

    // fails if widget is NULL, GTWidget::click fails or widget hasn't got focus
    static void setFocus(GUITestOpStatus &os, QWidget *w);

    // finds widget with the given object name using given FindOptions. Parent widget is QMainWindow, if not set
    static QWidget *findWidget(GUITestOpStatus &os, const QString &widgetName, const QWidget * const parentWidget = NULL, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static QPoint getWidgetCenter(GUITestOpStatus &os, QWidget* w);

    static QAbstractButton *findButtonByText(GUITestOpStatus &os, const QString &text, QWidget *parentWidget = NULL, const GTGlobals::FindOptions& = GTGlobals::FindOptions());
    static QList<QLabel*> findLabelByText(GUITestOpStatus &os, const QString &text, QWidget *parentWidget = NULL, const GTGlobals::FindOptions& = GTGlobals::FindOptions());

    //returns color of point p in widget w coordinates
    static QColor getColor(GUITestOpStatus &os, QWidget *widget, const QPoint &point);
    static QPixmap getPixmap(GUITestOpStatus &os, QWidget *widget);
    static QImage getImage(GUITestOpStatus &os, QWidget *widget);

    //this method writes info about all widgets to opStatus
    static void getAllWidgetsInfo(GUITestOpStatus &os, QWidget* parent=NULL);

    static void close(GUITestOpStatus &os, QWidget *widget);
    static void showMaximized(GUITestOpStatus &os, QWidget *widget);
    static void showNormal(GUITestOpStatus &os, QWidget *widget);

    static void clickLabelLink(GUITestOpStatus &os, QWidget* label, int step = 10, int indent = 0);
    static void clickWindowTitle(GUITestOpStatus &os, QWidget *window);
    static void moveWidgetTo(GUITestOpStatus &os, QWidget *window, const QPoint &point);
    static void resizeWidget(GUITestOpStatus &os, QWidget *widget, const QSize &size);
    static QPoint getWidgetGlobalTopLeftPoint(GUITestOpStatus &os, QWidget *widget);

    static QWidget *getActiveModalWidget(GUITestOpStatus &os);

    static void checkEnabled(GUITestOpStatus &os, QWidget *widget, bool expectedEnabledState = true);
    static void checkEnabled(GUITestOpStatus &os, const QString &widgetName, bool expectedEnabledState = true, QWidget const * const parent = NULL);

    #define GT_CLASS_NAME "GTWidget"
    #define GT_METHOD_NAME "findWidget"
    template<class T>
    static T findExactWidget(GUITestOpStatus &os, const QString &widgetName, QWidget const * const parentWidget = NULL, const GTGlobals::FindOptions& options= GTGlobals::FindOptions()){
        T result = NULL;
        QWidget* w = findWidget(os, widgetName, parentWidget, options);
        result = qobject_cast<T>(w);
        if(options.failIfNotFound == true){
            GT_CHECK_RESULT(w != NULL, "widget " + widgetName + " not found", result);
            GT_CHECK_RESULT(result != NULL, "widget of specefied class not found, but there is another widget with the same name, its class is: " + QString(w->metaObject()->className()), result);
        }
        return result;
    }
    #undef GT_METHOD_NAME
    #undef GT_CLASS_NAME
};

} //namespace

#endif
