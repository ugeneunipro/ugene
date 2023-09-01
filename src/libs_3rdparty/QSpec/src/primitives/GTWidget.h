/**
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

#pragma once

#include <functional>
#include <utils/GTThread.h>

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QDialogButtonBox>
#include <QGraphicsView>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMdiArea>
#include <QMenu>
#include <QPlainTextEdit>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QTreeWidget>
#include <QWidget>

#include "GTGlobals.h"
#include "GTMainWindow.h"

namespace HI {
/*!
 * \brief The class for working with QWidget primitive
 */
class HI_EXPORT GTWidget {
public:
    // fails if widget is NULL, not visible or not enabled; if p isNull, clicks in the center of widget
    static void click(QWidget* w, Qt::MouseButton mouseButton = Qt::LeftButton, QPoint p = QPoint());

    // use this method if you need to click on a point and not on a widget.
    static void moveToAndClick(const QPoint& point);

    // fails if widget is NULL, GTWidget::click fails or widget hasn't got focus
    static void setFocus(QWidget* w);

    // finds widget with the given object name using given FindOptions. Parent widget is QMainWindow, if not set
    static QWidget* findWidget(const QString& objectName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& = {});

    static QPoint getWidgetCenter(QWidget* widget);
    static QPoint getWidgetVisibleCenter(QWidget* widget);
    static QPoint getWidgetVisibleCenterGlobal(QWidget* widget);

    static QAbstractButton* findButtonByText(const QString& text, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& = {});

    static QList<QLabel*> findLabelByText(const QString& text, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& = {});

    // returns color of point p in widget w coordinates
    static QColor getColor(QWidget* widget, const QPoint& point);

    static bool hasPixelWithColor(QWidget* widget, const QColor& expectedColor);

    static bool hasPixelWithColor(const QImage& image, const QColor& expectedColor);

    /** Returns true if the image has only the given color. */
    static bool hasSingleFillColor(const QImage& image, const QColor& color);

    /**
     * Returns set of colors found in the image.
     * Once 'maxColors' limit is reached the algorithm stops and returns the current set.
     * This parameter helps to avoid out of memory errors and optimize performance.
     */
    static QSet<QRgb> countColors(const QImage& image, int maxColors = 100000);

    /**
     * Returns image of the widget using widget->grab() method.
     * If useGrabWindow is true calls QPixmap::grabWindow method: it allows to capture non-QT (like OpenGL) images.
     */
    static QImage getImage(QWidget* widget, bool useGrabWindow = false);

    /** Creates sub-image from the given image. Fails if 'rect' is not within the image. */
    static QImage createSubImage(const QImage& image, const QRect& rect);

    static void close(QWidget* widget);
    static void showMaximized(QWidget* widget);
    static void showNormal(QWidget* widget);
    static void showMinimized(QWidget* widget);

    static void clickLabelLink(QWidget* label, int step = 10, int indent = 0);
    static void clickWindowTitle(QWidget* window);
    static void resizeWidget(QWidget* widget, const QSize& size);
    static QPoint getWidgetGlobalTopLeftPoint(QWidget* widget);

    static QWidget* getActiveModalWidget();
    static QMenu* getActivePopupMenu();

    static void checkEnabled(QWidget* widget, bool expectedEnabledState = true);
    static void checkEnabled(const QString& widgetName, bool expectedEnabledState = true, QWidget* parent = nullptr);

    static void scrollToIndex(QAbstractItemView* itemView, const QModelIndex& index);

#define GT_CLASS_NAME "GTWidget"
    template<class T>
    static T findExactWidget(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {}) {
        QWidget* w = findWidget(widgetName, parentWidget, options);
        T result = qobject_cast<T>(w);
        if (options.failIfNotFound) {
            GT_CHECK_RESULT(w != nullptr, "widget " + widgetName + " not found", result);
            GT_CHECK_RESULT(result != nullptr, "widget of specified class not found, but there is another widget with the same name, its class is: " + QString(w->metaObject()->className()), result);
        }
        return result;
    }

    /** Calls findExactWidget with QRadioButton type. Shortcut method. */
    static QRadioButton* findRadioButton(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QGroupBox type. Shortcut method. */
    static QGroupBox* findGroupBox(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QLineEdit type. Shortcut method. */
    static QLineEdit* findLineEdit(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTreeWidget type. Shortcut method. */
    static QTreeView* findTreeView(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTextEdit type. Shortcut method. */
    static QTextEdit* findTextEdit(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTableWidget type. Shortcut method. */
    static QTableWidget* findTableWidget(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTabWidget type. Shortcut method. */
    static QTabWidget* findTabWidget(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QPlainTextEdit type. Shortcut method. */
    static QPlainTextEdit* findPlainTextEdit(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QCheckBox type. Shortcut method. */
    static QCheckBox* findCheckBox(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QComboBox type. Shortcut method. */
    static QComboBox* findComboBox(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QSpinBox type. Shortcut method. */
    static QSpinBox* findSpinBox(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QDoubleSpinBox type. Shortcut method. */
    static QDoubleSpinBox* findDoubleSpinBox(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QToolButton type. Shortcut method. */
    static QToolButton* findToolButton(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QToolBar type. Shortcut method. */
    static QToolBar* findToolBar(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QScrollBar type. Shortcut method. */
    static QScrollBar* findScrollBar(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTreeWidget type. Shortcut method. */
    static QTreeWidget* findTreeWidget(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QListWidget type. Shortcut method. */
    static QListWidget* findListWidget(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QGraphicsView type. Shortcut method. */
    static QGraphicsView* findGraphicsView(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QMenu type. Shortcut method. */
    static QMenu* findMenuWidget(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QPushButton type. Shortcut method. */
    static QPushButton* findPushButton(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QSlider type. Shortcut method. */
    static QSlider* findSlider(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QSplitter type. Shortcut method. */
    static QSplitter* findSplitter(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QLabel type. Shortcut method. */
    static QLabel* findLabel(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QMdiArea type. Shortcut method. */
    static QMdiArea* findMdiArea(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QScrollArea type. Shortcut method. */
    static QScrollArea* findScrollArea(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTextBrowser type. Shortcut method. */
    static QTextBrowser* findTextBrowser(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTableView type. Shortcut method. */
    static QTableView* findTableView(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QDialogButtonBox type. Shortcut method. */
    static QDialogButtonBox* findDialogButtonBox(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QProgressBar type. Shortcut method. */
    static QProgressBar* findProgressBar(const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Finds a child widget with the given type. Fails is widget can't be found. */
    template<class T>
    static T findWidgetByType(QWidget* parentWidget, const QString& errorMessage) {
        T widget = nullptr;
        for (int time = 0; time < GT_OP_WAIT_MILLIS && widget == nullptr; time += GT_OP_CHECK_MILLIS) {
            GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0, "findWidgetByType: " + errorMessage);
            widget = parentWidget->findChild<T>();
        }
        GT_CHECK_RESULT(widget != nullptr, errorMessage, nullptr);
        return widget;
    }

    /**
     * Finds all children of the 'parent' using 'findChildren' method and checkFn to check if the child is matched.
     * If parent is null, find all child in all main window.
     * The function is run in main thread.
     */
    template<class ChildType>
    static QList<ChildType*> findChildren(QObject* parent, std::function<bool(ChildType*)> matchFn) {
        QList<ChildType*> result;

        // object->findChildren for UX objects (widgets, actions) must be run in the main thread only to avoid parallel modification on GUI restructuring.
        class FindChildrenScenario : public CustomScenario {
        public:
            FindChildrenScenario(QObject* parent, std::function<bool(ChildType*)>& _matchFn, QList<ChildType*>& _result)
                : parentPtr(parent), matchFn(_matchFn), result(_result) {
                if (parent != nullptr) {
                    useParent = true;
                    parentObjectName = parent->objectName();
                }
            }

            QPointer<QObject> parentPtr;
            bool useParent = false;
            QString parentObjectName;
            std::function<bool(ChildType*)>& matchFn;
            QList<ChildType*>& result;

            void run() override {
                // If parent is null, start from QMainWindows.
                QList<QObject*> roots;
                QObject* parent = parentPtr;
                if (parent != nullptr) {
                    roots << parent;
                } else {
                    GT_CHECK(!useParent, "Parent object was destroyed before run(): " + parentObjectName);
                    QList<QWidget*> topLevelWidgets = GTMainWindow::getMainWindowsAsWidget();
                    for (const auto& topLevelWidget : qAsConst(topLevelWidgets)) {
                        roots << topLevelWidget;
                    }
                }
                for (auto root : qAsConst(roots)) {
                    QList<ChildType*> children = root->findChildren<ChildType*>();
                    for (ChildType* child : qAsConst(children)) {
                        if (matchFn(child)) {
                            result.append(child);
                        }
                    }
                }
            }
        };
        GTThread::runInMainThread(new FindChildrenScenario(parent, matchFn, result));
        return result;
    }

#undef GT_CLASS_NAME
};

}  // namespace HI
