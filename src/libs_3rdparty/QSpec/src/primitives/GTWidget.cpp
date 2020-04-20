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
#include <QComboBox>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QLineEdit>
#include <QMainWindow>
#include <QStyle>

#include "drivers/GTMouseDriver.h"
#include "primitives/GTMainWindow.h"
#include "primitives/GTWidget.h"
#include "utils/GTThread.h"

namespace HI {
#define GT_CLASS_NAME "GTWidget"

#define GT_METHOD_NAME "click"
void GTWidget::click(GUITestOpStatus &os, QWidget *w, Qt::MouseButton mouseButton, QPoint p, bool safe) {
    GTGlobals::sleep(100);
    GT_CHECK(w != NULL, "widget is NULL");
//    GT_CHECK(w->isEnabled() == true, "widget " + w->objectName() + "is not enabled");

    if (p.isNull()) {
        p = w->rect().center();

        // TODO: this is a fast fix
        if (w->objectName().contains("ADV_single_sequence_widget")) {
            p += QPoint(0, 8);
        }
    }
    GTMouseDriver::moveTo(w->mapToGlobal(p));
    if (safe) {
        GTMouseDriver::click(mouseButton);
    } else {
        //sometimes GTGlobals::sleep(os) should not be used after clicking
        GTMouseDriver::press(mouseButton);
        GTMouseDriver::release(mouseButton);
    }
    GTGlobals::sleep(200);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFocus"
void GTWidget::setFocus(GUITestOpStatus &os, QWidget *w) {
    GT_CHECK(w != NULL, "widget is NULL");

    GTWidget::click(os, w);
    GTGlobals::sleep(200);

    if(!qobject_cast<QComboBox*>(w)){
        GT_CHECK(w->hasFocus(), QString("Can't set focus on widget '%1'").arg(w->objectName()));
    }

}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findWidget"
QWidget* GTWidget::findWidget(GUITestOpStatus &os, const QString &widgetName, QWidget const * const parentWidget, const GTGlobals::FindOptions& options) {
    Q_UNUSED(os);

    if (parentWidget == NULL) {
        QList<QWidget*> list;
        foreach(QWidget* parent, GTMainWindow::getMainWindowsAsWidget(os)){
            list << parent->findChildren<QWidget*>(widgetName);
        }
        if (options.failIfNotFound) {
            GT_CHECK_RESULT(list.count() != 0, QString("Widget '%1' not found").arg(widgetName), NULL);
        }
        GT_CHECK_RESULT(list.count()<2, QString("There are %1 widgets with name '%2'").arg(list.count()).arg(widgetName), NULL);
        if (list.count() == 0) {
            return NULL;
        } else {
            return list.takeFirst();
        }
    }
    QWidget* widget = parentWidget->findChild<QWidget*>(widgetName);

    if (options.failIfNotFound) {
        GT_CHECK_RESULT(widget != NULL, "Widget " + widgetName + " not found", NULL);
    }

    return widget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getWidgetCenter"
QPoint GTWidget::getWidgetCenter(GUITestOpStatus &os, QWidget *w){
    Q_UNUSED(os)
    return w->mapToGlobal(w->rect().center());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findButtonByText"
QAbstractButton* GTWidget::findButtonByText(GUITestOpStatus &os, const QString &text, QWidget *parentWidget, const GTGlobals::FindOptions& options) {

    if (parentWidget == NULL) {
        QList<QAbstractButton*> resultList;
        foreach(QWidget* parent, GTMainWindow::getMainWindowsAsWidget(os)){
            QList<QAbstractButton*> list = parent->findChildren<QAbstractButton*>();
            foreach(QAbstractButton* ab, list){
                if(ab->text().contains(text, Qt::CaseInsensitive)){
                    resultList.append(ab);
                }
            }
        }

        if (options.failIfNotFound) {
            GT_CHECK_RESULT(resultList.count()!=0,"button not found", NULL);
        }
        GT_CHECK_RESULT(resultList.count()<2, QString("There are %1 buttons with this text").arg(resultList.count()), NULL);
        if(resultList.count() == 0){
            return NULL;
        }else{
            return resultList.takeFirst();
        }
    }
    QList<QAbstractButton*> buttonList = parentWidget->findChildren<QAbstractButton*>();
    QList<QAbstractButton*> foundButtonList;
    QStringList sL;

    foreach(QAbstractButton* but, buttonList){
        sL.append(but->text());
        if (but->text().contains(text, Qt::CaseInsensitive)){
            foundButtonList << but;
        }
    }

    GT_CHECK_RESULT(foundButtonList.count()<=1, QString("there are %1 buttons with such text").arg(foundButtonList.count()), NULL);

    if (options.failIfNotFound) {
        GT_CHECK_RESULT(foundButtonList.count() != 0, QString("button with this text <%1> not found").arg(text), NULL);
    }

    return foundButtonList.takeFirst();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findWidget"
void GTWidget::getAllWidgetsInfo(GUITestOpStatus &os, QWidget *parent){

    QList<QObject*> list;
    if(parent == NULL){
        foreach(QWidget* parent, GTMainWindow::getMainWindowsAsWidget(os)){
            list.append(parent->findChildren<QObject*>());
        }
    }else{
        list = parent->findChildren<QObject*>();
    }
    QString actStr;
    actStr.append("Getting all info about widget\n");

    foreach(QObject* act, list){
        actStr.append(act->objectName()+ "  " + act->metaObject()->className() + "  " + /*QString("%1").arg(act->isVisible()) + " " + QString("%1").arg(act->geometry().width()) +*/ "\n");
    }
    CHECK_SET_ERR(false, actStr);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "close"
void GTWidget::close(GUITestOpStatus &os, QWidget *widget) {
    GT_CHECK(NULL != widget, "Widget is NULL");

    class Scenario : public CustomScenario {
    public:
        Scenario(QWidget *widget) :
            widget(widget)
        {

        }

        void run(GUITestOpStatus &os) {
            Q_UNUSED(os);
            CHECK_SET_ERR(NULL != widget, "Widget is NULL");
            widget->close();
            GTGlobals::sleep(100);
        }

    private:
        QWidget *widget;
    };

    GTThread::runInMainThread(os, new Scenario(widget));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "showMaximized"
void GTWidget::showMaximized(GUITestOpStatus &os, QWidget *widget) {
    GT_CHECK(NULL != widget, "Widget is NULL");

    class Scenario : public CustomScenario {
    public:
        Scenario(QWidget *widget) :
            widget(widget)
        {

        }

        void run(GUITestOpStatus &os) {
            Q_UNUSED(os);
            CHECK_SET_ERR(NULL != widget, "Widget is NULL");
            widget->showMaximized();
            GTGlobals::sleep(100);
        }

    private:
        QWidget *widget;
    };

    GTThread::runInMainThread(os, new Scenario(widget));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "showNormal"
void GTWidget::showNormal(GUITestOpStatus &os, QWidget *widget) {
    GT_CHECK(NULL != widget, "Widget is NULL");

    class Scenario : public CustomScenario {
    public:
        Scenario(QWidget *widget) :
            widget(widget)
        {

        }

        void run(GUITestOpStatus &os) {
            Q_UNUSED(os);
            CHECK_SET_ERR(NULL != widget, "Widget is NULL");
            widget->showNormal();
            GTGlobals::sleep(100);
        }

    private:
        QWidget *widget;
    };

    GTThread::runInMainThread(os, new Scenario(widget));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColor"
QColor GTWidget::getColor(GUITestOpStatus &os, QWidget *widget, const QPoint &point) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(NULL != widget, "Widget is NULL", QColor());

    return QColor(getImage(os, widget).pixel(point));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getPixmap"
QPixmap GTWidget::getPixmap(GUITestOpStatus &os, QWidget *widget) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(NULL != widget, "Widget is NULL", QPixmap());

    class Scenario : public CustomScenario {
    public:
        Scenario(QWidget *widget, QPixmap &pixmap) :
            widget(widget),
            pixmap(pixmap)
        {

        }

        void run(GUITestOpStatus &os) {
            Q_UNUSED(os);
            CHECK_SET_ERR(NULL != widget, "Widget to grab is NULL");
            pixmap = widget->grab(widget->rect());
        }

    private:
        QWidget *widget;
        QPixmap &pixmap;
    };

    QPixmap pixmap;
    GTThread::runInMainThread(os, new Scenario(widget, pixmap));
    return pixmap;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getImage"
QImage GTWidget::getImage(GUITestOpStatus &os, QWidget *widget) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(NULL != widget, "Widget is NULL", QImage());

    return getPixmap(os, widget).toImage();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickLabelLink"
void GTWidget::clickLabelLink(GUITestOpStatus &os, QWidget *label, int step, int indent){

    QRect r = label->rect();

    int left = r.left();
    int right = r.right();
    int top = r.top() + indent;
    int bottom = r.bottom();
    for(int i = left; i < right; i+=step){
        for(int j = top; j < bottom; j+=step){
            GTMouseDriver::moveTo(label->mapToGlobal(QPoint(i,j)));
            if(label->cursor().shape() == Qt::PointingHandCursor){
                GTGlobals::sleep(500);
                GTMouseDriver::click();
                return;
            }
        }
    }
    GT_CHECK(false, "label does not contain link");
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickWindowTitle"
void GTWidget::clickWindowTitle(GUITestOpStatus &os, QWidget *window) {
    GT_CHECK(NULL != window, "Window is NULL");

    QStyleOptionTitleBar opt;
    opt.initFrom(window);
    const QRect titleLabelRect = window->style()->subControlRect(QStyle::CC_TitleBar, &opt, QStyle::SC_TitleBarLabel);
    GTMouseDriver::moveTo(getWidgetGlobalTopLeftPoint(os, window) + titleLabelRect.center());
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveWidgetTo"
void GTWidget::moveWidgetTo(GUITestOpStatus &os, QWidget *window, const QPoint &point){
    //QPoint(window->width()/2,3) - is hack
    GTMouseDriver::moveTo(getWidgetGlobalTopLeftPoint(os, window) + QPoint(window->width()/2,3));
    const QPoint p0 = getWidgetGlobalTopLeftPoint(os, window) + QPoint(window->width()/2,3);
    const QPoint p1 = point + QPoint(window->width()/2,3);
    GTMouseDriver::dragAndDrop(p0, p1);
    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "resizeWidget"
void GTWidget::resizeWidget(GUITestOpStatus &os, QWidget *widget, const QSize &size) {
    GT_CHECK(NULL != widget, "Widget is NULL");

    QRect displayRect = QApplication::desktop()->screenGeometry();
    GT_CHECK( (displayRect.width() >= size.width()) && (displayRect.height() >= size.height()) , "Specified the size larger than the size of the screen");

    bool neededPositionFound = false;
    QSize oldSize = widget->size();

    QPoint topLeftPos = getWidgetGlobalTopLeftPoint(os, widget) + QPoint(5, 5);
    for (int i=0; i<5; i++){
        GTMouseDriver::moveTo(topLeftPos);
        QPoint newTopLeftPos = topLeftPos + QPoint(widget->frameGeometry().width() - 1, widget->frameGeometry().height() - 1) - QPoint(size.width(), size.height());
        GTMouseDriver::dragAndDrop(topLeftPos, newTopLeftPos);
        if (widget->size() != oldSize){
            neededPositionFound = true;
            break;
        }else{
            topLeftPos -= QPoint(1,1);
        }
    }
    GT_CHECK(neededPositionFound, "Needed mouse position for resizing not found");

    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getWidgetGlobalTopLeftPoint"
QPoint GTWidget::getWidgetGlobalTopLeftPoint(GUITestOpStatus &os, QWidget *widget) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(NULL != widget, "Widget is NULL", QPoint());
    return (widget->isWindow() ? widget->pos() : widget->parentWidget()->mapToGlobal(QPoint(0, 0)));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getActiveModalWidget"
QWidget *GTWidget::getActiveModalWidget(GUITestOpStatus &os) {
    Q_UNUSED(os);
    QWidget *modalWidget = QApplication::activeModalWidget();
    GT_CHECK_RESULT(NULL != modalWidget, "Active modal widget is NULL", NULL);
    return modalWidget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkEnabled"
void GTWidget::checkEnabled(GUITestOpStatus &os, QWidget *widget, bool expectedEnabledState) {
    Q_UNUSED(os);
    GT_CHECK(NULL != widget, "Widget is NULL");
    GT_CHECK(widget->isVisible(), "Widget is not visible");
    const bool actualEnabledState = widget->isEnabled();
    GT_CHECK(expectedEnabledState == actualEnabledState,
             QString("Widget state is incorrect: expected '%1', got '%'2")
             .arg(expectedEnabledState ? "enabled" : "disabled")
             .arg(actualEnabledState ? "enabled" : "disabled"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkEnabled"
void GTWidget::checkEnabled(GUITestOpStatus &os, const QString &widgetName, bool expectedEnabledState, QWidget const * const parent) {
    checkEnabled(os, GTWidget::findWidget(os, widgetName, parent), expectedEnabledState);
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

} //namespace
