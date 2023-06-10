/**
 * UGENE - Integrated Bioinformatics Tools.
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "primitives/GTWidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QGuiApplication>
#include <QMdiArea>
#include <QProgressBar>
#include <QScrollBar>
#include <QStyle>
#include <QTextBrowser>
#include <QWindow>

#include "drivers/GTMouseDriver.h"
#include "utils/GTThread.h"
#include "utils/GTUtilsText.h"

namespace HI {
#define GT_CLASS_NAME "GTWidget"

#define GT_METHOD_NAME "click"
void GTWidget::click(QWidget* widget, Qt::MouseButton mouseButton, QPoint p) {
    GT_CHECK(widget != nullptr, "widget is NULL");

    if (p.isNull()) {
        p = getWidgetVisibleCenter(widget);
        // TODO: this is a fast fix
        if (widget->objectName().contains("ADV_single_sequence_widget")) {
            p += QPoint(0, 8);
        }
    }
    QPoint globalPoint = widget->mapToGlobal(p);
    bool clickIsOk = GTMouseDriver::click(globalPoint, mouseButton);
    GT_CHECK(clickIsOk, QString("GTWidget::click Successfully clicked position: %1 %2").arg(globalPoint.x()).arg(globalPoint.y()));
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "moveToAndClick"
void GTWidget::moveToAndClick(const QPoint& point) {
    bool moveIsOk = GTMouseDriver::moveTo(point);
    GT_CHECK(moveIsOk, QString("GTWidget::moveToAndClick: move is OK: %1 %2").arg(point.x()).arg(point.y()));

    GTThread::waitForMainThread();

    bool clickIsOk = GTMouseDriver::click();
    GT_CHECK(clickIsOk, QString("GTWidget::moveToAndClick: click is OK"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setFocus"
void GTWidget::setFocus(QWidget* w) {
    GT_CHECK(w != nullptr, "widget is NULL");
    GTWidget::click(w);
    GTGlobals::sleep(200);
    if (!qobject_cast<QComboBox*>(w)) {
        GT_CHECK(w->hasFocus(), QString("Can't set focus on widget '%1'").arg(w->objectName()));
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findWidget"
QWidget* GTWidget::findWidget(const QString& objectName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    CHECK_NO_OS_ERROR(nullptr);
    QPointer<QWidget> parentWidgetPtr(parentWidget);
    QWidget* widget = nullptr;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && widget == nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        if (parentWidget != nullptr && parentWidgetPtr == nullptr) {
            break;  // Parent widget was removed while waiting.
        }
        QList<QWidget*> matchedWidgets = findChildren<QWidget>(parentWidget, [&objectName](QWidget* w) { return w->objectName() == objectName; });
#ifdef _DEBUG
        if (matchedWidgets.size() >= 2)
#endif
            GT_CHECK_RESULT(matchedWidgets.size() < 2, QString("There are %1 widgets with name '%2'").arg(matchedWidgets.size()).arg(objectName), nullptr);
        widget = matchedWidgets.isEmpty() ? nullptr : matchedWidgets[0];
        if (!options.failIfNotFound) {
            break;
        }
    }
    if (options.failIfNotFound) {
        GT_CHECK_RESULT(widget != nullptr, QString("Widget '%1' not found").arg(objectName), nullptr);
    }
    return widget;
}
#undef GT_METHOD_NAME

QRadioButton* GTWidget::findRadioButton(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QRadioButton*>(widgetName, parentWidget, options);
}

QGroupBox* GTWidget::findGroupBox(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QGroupBox*>(widgetName, parentWidget, options);
}

QLineEdit* GTWidget::findLineEdit(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QLineEdit*>(widgetName, parentWidget, options);
}

QTreeView* GTWidget::findTreeView(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QTreeView*>(widgetName, parentWidget, options);
}

QTextEdit* GTWidget::findTextEdit(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QTextEdit*>(widgetName, parentWidget, options);
}

QTableWidget* GTWidget::findTableWidget(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QTableWidget*>(widgetName, parentWidget, options);
}

QTabWidget* GTWidget::findTabWidget(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QTabWidget*>(widgetName, parentWidget, options);
}

QPlainTextEdit* GTWidget::findPlainTextEdit(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QPlainTextEdit*>(widgetName, parentWidget, options);
}

QCheckBox* GTWidget::findCheckBox(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QCheckBox*>(widgetName, parentWidget, options);
}

QComboBox* GTWidget::findComboBox(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QComboBox*>(widgetName, parentWidget, options);
}

QSpinBox* GTWidget::findSpinBox(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QSpinBox*>(widgetName, parentWidget, options);
}

QDoubleSpinBox* GTWidget::findDoubleSpinBox(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QDoubleSpinBox*>(widgetName, parentWidget, options);
}

QToolButton* GTWidget::findToolButton(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QToolButton*>(widgetName, parentWidget, options);
}

QToolBar* GTWidget::findToolBar(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QToolBar*>(widgetName, parentWidget, options);
}

QScrollBar* GTWidget::findScrollBar(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QScrollBar*>(widgetName, parentWidget, options);
}

QTreeWidget* GTWidget::findTreeWidget(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QTreeWidget*>(widgetName, parentWidget, options);
}

QListWidget* GTWidget::findListWidget(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QListWidget*>(widgetName, parentWidget, options);
}

QGraphicsView* GTWidget::findGraphicsView(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QGraphicsView*>(widgetName, parentWidget, options);
}

QMenu* GTWidget::findMenuWidget(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QMenu*>(widgetName, parentWidget, options);
}

QPushButton* GTWidget::findPushButton(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QPushButton*>(widgetName, parentWidget, options);
}

QSlider* GTWidget::findSlider(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QSlider*>(widgetName, parentWidget, options);
}

QSplitter* GTWidget::findSplitter(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QSplitter*>(widgetName, parentWidget, options);
}

QLabel* GTWidget::findLabel(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QLabel*>(widgetName, parentWidget, options);
}

QMdiArea* GTWidget::findMdiArea(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QMdiArea*>(widgetName, parentWidget, options);
}

QScrollArea* GTWidget::findScrollArea(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QScrollArea*>(widgetName, parentWidget, options);
}

QTextBrowser* GTWidget::findTextBrowser(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QTextBrowser*>(widgetName, parentWidget, options);
}

QTableView* GTWidget::findTableView(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QTableView*>(widgetName, parentWidget, options);
}

QDialogButtonBox* GTWidget::findDialogButtonBox(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QDialogButtonBox*>(widgetName, parentWidget, options);
}

QProgressBar* GTWidget::findProgressBar(const QString& widgetName, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    return findExactWidget<QProgressBar*>(widgetName, parentWidget, options);
}

QPoint GTWidget::getWidgetCenter(QWidget* widget) {
    return widget->mapToGlobal(widget->rect().center());
}

QPoint GTWidget::getWidgetVisibleCenter(QWidget* widget) {
    return widget->mapFromGlobal(getWidgetVisibleCenterGlobal(widget));
}

QPoint GTWidget::getWidgetVisibleCenterGlobal(QWidget* widget) {
    QRect rect = widget->rect();
    QRect gRect(widget->mapToGlobal(rect.topLeft()), rect.size());
    QWidget* parent = widget->parentWidget();
    while (parent) {
        QRect pRect = parent->rect();
        QRect gParentRect(widget->mapToGlobal(pRect.topLeft()), pRect.size());

        gRect = gRect.intersected(gParentRect);
        parent = parent->parentWidget();
    }
    return gRect.center();
}

#define GT_METHOD_NAME "findButtonByText"
QAbstractButton* GTWidget::findButtonByText(const QString& text, QWidget* parentWidget, const GTGlobals::FindOptions& options) {
    QList<QAbstractButton*> resultButtonList;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && resultButtonList.isEmpty(); time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        resultButtonList = findChildren<QAbstractButton>(
            parentWidget,
            [text](auto button) { return button->text().contains(text, Qt::CaseInsensitive); });
        if (!options.failIfNotFound) {
            break;
        }
    }
    GT_CHECK_RESULT(resultButtonList.size() <= 1, QString("There are %1 buttons with text '%2'").arg(resultButtonList.size()).arg(text), nullptr);
    if (options.failIfNotFound) {
        GT_CHECK_RESULT(!resultButtonList.isEmpty(), QString("Button with the text <%1> is not found").arg(text), nullptr);
    }
    return resultButtonList.isEmpty() ? nullptr : resultButtonList.first();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findLabelByText"
QList<QLabel*> GTWidget::findLabelByText(
    const QString& text,
    QWidget* parentWidget,
    const GTGlobals::FindOptions& options) {
    QList<QLabel*> resultLabelList;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && resultLabelList.isEmpty(); time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        resultLabelList = findChildren<QLabel>(
            parentWidget,
            [text](auto label) { return label->text().contains(text, Qt::CaseInsensitive); });
        if (!options.failIfNotFound) {
            break;
        }
    }
    GT_CHECK_RESULT(!options.failIfNotFound || !resultLabelList.isEmpty(), QString("Label with text <%1> not found").arg(text), {});
    return resultLabelList;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "close"
void GTWidget::close(QWidget* widget) {
    GT_CHECK(widget != nullptr, "Widget is NULL");

    class Scenario : public CustomScenario {
    public:
        Scenario(QWidget* _widget)
            : widget(_widget) {
        }

        void run() override {
            widget->close();
            GTGlobals::sleep(100);
        }

    private:
        QWidget* widget = nullptr;
    };
    GTThread::runInMainThread(new Scenario(widget));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "showMaximized"
void GTWidget::showMaximized(QWidget* widget) {
    class ShowMaximizedScenario : public CustomScenario {
    public:
        ShowMaximizedScenario(QWidget* w)
            : widget(w) {
        }
        void run() override {
            widget->setWindowState(Qt::WindowActive);
            widget->showMaximized();
        }
        QWidget* widget;
    };
    GTThread::runInMainThread(new ShowMaximizedScenario(widget));
    GTGlobals::sleep(1000);  // Wait for OS to complete the op.
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "showMinimized"
void GTWidget::showMinimized(QWidget* widget) {
    class ShowMinimizedScenario : public CustomScenario {
    public:
        ShowMinimizedScenario(QWidget* w)
            : widget(w) {
        }
        void run() override {
            widget->showMinimized();
        }
        QWidget* widget;
    };
    GTThread::runInMainThread(new ShowMinimizedScenario(widget));
    GTGlobals::sleep(1000);  // Wait for OS to complete the op.
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "showNormal"
void GTWidget::showNormal(QWidget* widget) {
    GT_CHECK(widget != nullptr, "Widget is NULL");

    class Scenario : public CustomScenario {
    public:
        Scenario(QWidget* widget)
            : widget(widget) {
        }

        void run() {

            CHECK_SET_ERR(widget != nullptr, "Widget is NULL");
            widget->showNormal();
            GTGlobals::sleep(100);
        }

    private:
        QWidget* widget;
    };

    GTThread::runInMainThread(new Scenario(widget));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getColor"
QColor GTWidget::getColor(QWidget* widget, const QPoint& point) {
    GT_CHECK_RESULT(widget != nullptr, "Widget is NULL", QColor());
    return QColor(getImage(widget).pixel(point));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getImage"
QImage GTWidget::getImage(QWidget* widget, bool useGrabWindow) {
    GT_CHECK_RESULT(widget != nullptr, "Widget is NULL", QImage());

    class GrabImageScenario : public CustomScenario {
    public:
        GrabImageScenario(QWidget* _widget, QImage& _image, bool _useGrabWindow)
            : widget(_widget), image(_image), useGrabWindow(_useGrabWindow) {
        }

        void run() override {
            CHECK_SET_ERR(widget != nullptr, "Widget to grab is NULL");
            QPixmap pixmap = useGrabWindow ? QPixmap::grabWindow(widget->winId()) : widget->grab(widget->rect());
            image = pixmap.toImage();
            double ratio = ((QGuiApplication*)QGuiApplication::instance())->devicePixelRatio();
            if (!useGrabWindow && ratio != 1 && ratio > 0) {
                image = image.scaled(qRound(image.width() / ratio), qRound(image.height() / ratio));
            }
        }

        QWidget* widget;
        QImage& image;
        bool useGrabWindow;
    };

    QImage image;
    GTThread::runInMainThread(new GrabImageScenario(widget, image, useGrabWindow));
    return image;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "createSubImage"
QImage GTWidget::createSubImage(const QImage& image, const QRect& rect) {
#ifdef _DEBUG
    if (!image.rect().contains(rect))
#endif
        GT_CHECK_RESULT(image.rect().contains(rect), "Invalid sub-image rect: " + GTUtilsText::rectToString(rect), QImage());
    int offset = rect.x() * image.depth() / 8 + rect.y() * image.bytesPerLine();
    return QImage(image.bits() + offset, rect.width(), rect.height(), image.bytesPerLine(), image.format());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasSingleFillColor"
bool GTWidget::hasSingleFillColor(const QImage& image, const QColor& color) {
    for (int x = 0; x < image.width(); x++) {
        for (int y = 0; y < image.height(); y++) {
            QColor pixelColor = image.pixel(x, y);
            if (pixelColor != color) {
                return false;
            }
        }
    }
    return true;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "countColors"
QSet<QRgb> GTWidget::countColors(const QImage& image, int maxColors) {
    QSet<QRgb> colorSet;
    for (int i = 0; i < image.width() && colorSet.size() < maxColors; i++) {
        for (int j = 0; j < image.height(); j++) {
            colorSet << image.pixel(i, j);
        }
    }
    return colorSet;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasPixelWithColor"
bool GTWidget::hasPixelWithColor(QWidget* widget, const QColor& expectedColor) {
    QImage image = getImage(widget);
    return hasPixelWithColor(image, expectedColor);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasPixelWithColorInImage"
bool GTWidget::hasPixelWithColor(const QImage& image, const QColor& expectedColor) {
    for (int x = 0; x < image.width(); x++) {
        for (int y = 0; y < image.height(); y++) {
            QColor pixelColor = image.pixel(x, y);
            if (pixelColor == expectedColor) {
                return true;
            }
        }
    }
    return false;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickLabelLink"
void GTWidget::clickLabelLink(QWidget* label, int step, int indent) {
    QRect r = label->rect();

    int left = r.left();
    int right = r.right();
    int top = r.top() + indent;
    int bottom = r.bottom();
    for (int i = left; i < right; i += step) {
        for (int j = top; j < bottom; j += step) {
            GTMouseDriver::moveTo(label->mapToGlobal(QPoint(i, j)));
            if (label->cursor().shape() == Qt::PointingHandCursor) {
                GTGlobals::sleep(500);
                GTMouseDriver::click();
                return;
            }
        }
    }
    GT_FAIL("label does not contain link", );
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "clickWindowTitle"
void GTWidget::clickWindowTitle(QWidget* window) {
    GT_CHECK(window != nullptr, "Window is NULL");

    QStyleOptionTitleBar opt;
    opt.initFrom(window);
    const QRect titleLabelRect = window->style()->subControlRect(QStyle::CC_TitleBar, &opt, QStyle::SC_TitleBarLabel);
    GTMouseDriver::moveTo(getWidgetGlobalTopLeftPoint(window) + titleLabelRect.center());
    GTMouseDriver::click();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "resizeWidget"
void GTWidget::resizeWidget(QWidget* widget, const QSize& size) {
    GT_CHECK(widget != nullptr, "Widget is NULL");

    QRect displayRect = QApplication::desktop()->screenGeometry();
    GT_CHECK((displayRect.width() >= size.width()) && (displayRect.height() >= size.height()), "Specified the size larger than the size of the screen");

    bool isRequiredPositionFound = false;
    QSize oldSize = widget->size();

    QPoint topLeftPos = getWidgetGlobalTopLeftPoint(widget) + QPoint(5, 5);
    for (int i = 0; i < 5; i++) {
        GTMouseDriver::moveTo(topLeftPos);
        QPoint newTopLeftPos = topLeftPos + QPoint(widget->frameGeometry().width() - 1, widget->frameGeometry().height() - 1) - QPoint(size.width(), size.height());
        GTMouseDriver::dragAndDrop(topLeftPos, newTopLeftPos);
        if (widget->size() != oldSize) {
            isRequiredPositionFound = true;
            break;
        } else {
            topLeftPos -= QPoint(1, 1);
        }
    }
    GT_CHECK(isRequiredPositionFound, "Required mouse position to start window resize was not found");
    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getWidgetGlobalTopLeftPoint"
QPoint GTWidget::getWidgetGlobalTopLeftPoint(QWidget* widget) {
    GT_CHECK_RESULT(widget != nullptr, "Widget is NULL", QPoint());
    return (widget->isWindow() ? widget->pos() : widget->parentWidget()->mapToGlobal(QPoint(0, 0)));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getActiveModalWidget"
QWidget* GTWidget::getActiveModalWidget() {
    QWidget* modalWidget = nullptr;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && modalWidget == nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        modalWidget = QApplication::activeModalWidget();
    }
    GT_CHECK_RESULT(modalWidget != nullptr, "Active modal widget is NULL", nullptr);
    return modalWidget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getActivePopupMenu"
QMenu* GTWidget::getActivePopupMenu() {
    QMenu* popupWidget = nullptr;
    for (int time = 0; time < GT_OP_WAIT_MILLIS && popupWidget == nullptr; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
        popupWidget = qobject_cast<QMenu*>(QApplication::activePopupWidget());
    }
    GT_CHECK_RESULT(popupWidget != nullptr, "Active popup menu is NULL", nullptr);
    return popupWidget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkEnabled"
void GTWidget::checkEnabled(QWidget* widget, bool expectedEnabledState) {
    GT_CHECK(widget != nullptr, "Widget is NULL");
    GT_CHECK(widget->isVisible(), "Widget is not visible");
    bool actualEnabledState = widget->isEnabled();
    for (int time = 0; time < GT_OP_WAIT_MILLIS && actualEnabledState != expectedEnabledState; time += GT_OP_CHECK_MILLIS) {
        GTGlobals::sleep(GT_OP_CHECK_MILLIS);
        actualEnabledState = widget->isEnabled();
    }
    GT_CHECK(actualEnabledState == expectedEnabledState,
             QString("Widget state is incorrect: expected '%1', got '%2'")
                 .arg(expectedEnabledState ? "enabled" : "disabled")
                 .arg(actualEnabledState ? "enabled" : "disabled"));
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkEnabled"
void GTWidget::checkEnabled(const QString& widgetName, bool expectedEnabledState, QWidget* parent) {
    checkEnabled(GTWidget::findWidget(widgetName, parent), expectedEnabledState);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToIndex"
void GTWidget::scrollToIndex(QAbstractItemView* itemView, const QModelIndex& index) {
    GT_CHECK(itemView != nullptr, "ItemView is nullptr");
    GT_CHECK(index.isValid(), "Model index is invalid");

    // Find cell. TODO: scroll to parameter by mouse/keyboard?
    class MainThreadActionScroll : public CustomScenario {
    public:
        MainThreadActionScroll(QAbstractItemView* _itemView, const QModelIndex& _index)
            : itemView(_itemView), index(_index) {
        }
        void run() override {
            itemView->scrollTo(index);
        }
        QAbstractItemView* itemView = nullptr;
        QModelIndex index;
    };
    GTThread::runInMainThread(new MainThreadActionScroll(itemView, index));
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace HI
