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

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QWidget>

#include "GTGlobals.h"

namespace HI {
/*!
 * \brief The class for working with QWidget primitive
 */
class HI_EXPORT GTWidget {
public:
    // fails if widget is NULL, not visible or not enabled; if p isNull, clicks on the center of widget
    static void click(GUITestOpStatus &os, QWidget *w, Qt::MouseButton mouseButton = Qt::LeftButton, QPoint p = QPoint());

    // fails if widget is NULL, GTWidget::click fails or widget hasn't got focus
    static void setFocus(GUITestOpStatus &os, QWidget *w);

    // finds widget with the given object name using given FindOptions. Parent widget is QMainWindow, if not set
    static QWidget *findWidget(GUITestOpStatus &os, const QString &widgetName, const QWidget *const parentWidget = NULL, const GTGlobals::FindOptions & = GTGlobals::FindOptions());
    static QPoint getWidgetCenter(QWidget *widget);

    static QAbstractButton *findButtonByText(GUITestOpStatus &os, const QString &text, QWidget *parentWidget = NULL, const GTGlobals::FindOptions & = GTGlobals::FindOptions());
    static QList<QLabel *> findLabelByText(GUITestOpStatus &os, const QString &text, QWidget *parentWidget = NULL, const GTGlobals::FindOptions & = GTGlobals::FindOptions());

    //returns color of point p in widget w coordinates
    static QColor getColor(GUITestOpStatus &os, QWidget *widget, const QPoint &point);
    static bool hasPixelWithColor(GUITestOpStatus &os, QWidget *widget, const QColor &expectedColor);
    static bool hasPixelWithColor(GUITestOpStatus &os, const QImage &image, const QColor &expectedColor);

    /** Returns true if the image has only the given color. */
    static bool hasSingleFillColor(const QImage &image, const QColor &color);

    /**
     * Returns image of the widget using widget->grab() method.
     * If useGrabWindow is true calls QPixmap::grabWindow method: it allows to capture non-QT (like OpenGL) images.
     */
    static QImage getImage(GUITestOpStatus &os, QWidget *widget, bool useGrabWindow = false);

    /** Creates sub-image from the given image. Fails if 'rect' is not within the image. */
    static QImage createSubImage(GUITestOpStatus &os, const QImage &image, const QRect &rect);

    static void close(GUITestOpStatus &os, QWidget *widget);
    static void showMaximized(GUITestOpStatus &os, QWidget *widget);
    static void showNormal(GUITestOpStatus &os, QWidget *widget);

    static void clickLabelLink(GUITestOpStatus &os, QWidget *label, int step = 10, int indent = 0);
    static void clickWindowTitle(GUITestOpStatus &os, QWidget *window);
    static void moveWidgetTo(GUITestOpStatus &os, QWidget *window, const QPoint &point);
    static void resizeWidget(GUITestOpStatus &os, QWidget *widget, const QSize &size);
    static QPoint getWidgetGlobalTopLeftPoint(GUITestOpStatus &os, QWidget *widget);

    static QWidget *getActiveModalWidget(GUITestOpStatus &os);
    static QWidget *getActivePopupWidget(GUITestOpStatus &os);
    static QMenu *getActivePopupMenu(GUITestOpStatus &os);

    static void checkEnabled(GUITestOpStatus &os, QWidget *widget, bool expectedEnabledState = true);
    static void checkEnabled(GUITestOpStatus &os, const QString &widgetName, bool expectedEnabledState = true, QWidget const *const parent = NULL);

    static void scrollToIndex(GUITestOpStatus &os, QAbstractItemView *itemView, const QModelIndex &index);

#define GT_CLASS_NAME "GTWidget"
#define GT_METHOD_NAME "findExactWidget"
    template<class T>
    static T findExactWidget(GUITestOpStatus &os, const QString &widgetName, QWidget const *const parentWidget = NULL, const GTGlobals::FindOptions &options = GTGlobals::FindOptions()) {
        T result = NULL;
        QWidget *w = findWidget(os, widgetName, parentWidget, options);
        result = qobject_cast<T>(w);
        if (options.failIfNotFound) {
            GT_CHECK_RESULT(w != NULL, "widget " + widgetName + " not found", result);
            GT_CHECK_RESULT(result != NULL, "widget of specefied class not found, but there is another widget with the same name, its class is: " + QString(w->metaObject()->className()), result);
        }
        return result;
    }
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findWidgetByType"
    /** Finds a child widget with the given type. Fails is widget can't be found. */
    template<class T>
    static T findWidgetByType(GUITestOpStatus &os, QWidget *parentWidget, const QString &errorMessage) {
        T widget = nullptr;
        for (int time = 0; time < GT_OP_WAIT_MILLIS && widget == nullptr; time += GT_OP_CHECK_MILLIS) {
            GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
            widget = parentWidget->findChild<T>();
        }
        GT_CHECK_RESULT(widget != nullptr, errorMessage, nullptr);
        return widget;
    }
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
};

}    // namespace HI

#endif
