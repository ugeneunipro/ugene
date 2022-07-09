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

#include <functional>
#include <utils/GTThread.h>

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QGraphicsView>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPlainTextEdit>
#include <QPointer>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QTreeView>
#include <QTreeWidget>
#include <QWidget>
#include <QMdiArea>
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QProgressBar>

#include "GTGlobals.h"
#include "GTMainWindow.h"

namespace HI {
/*!
 * \brief The class for working with QWidget primitive
 */
class HI_EXPORT GTWidget {
public:
    // fails if widget is NULL, not visible or not enabled; if p isNull, clicks in the center of widget
    static void click(GUITestOpStatus& os, QWidget* w, Qt::MouseButton mouseButton = Qt::LeftButton, QPoint p = QPoint());

    // fails if widget is NULL, GTWidget::click fails or widget hasn't got focus
    static void setFocus(GUITestOpStatus& os, QWidget* w);

    // finds widget with the given object name using given FindOptions. Parent widget is QMainWindow, if not set
    static QWidget* findWidget(GUITestOpStatus& os, const QString& objectName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& = {});

    static QPoint getWidgetCenter(QWidget* widget);

    static QAbstractButton* findButtonByText(GUITestOpStatus& os, const QString& text, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& = {});

    static QList<QLabel*> findLabelByText(GUITestOpStatus& os, const QString& text, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& = {});

    // returns color of point p in widget w coordinates
    static QColor getColor(GUITestOpStatus& os, QWidget* widget, const QPoint& point);

    static bool hasPixelWithColor(GUITestOpStatus& os, QWidget* widget, const QColor& expectedColor);

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
    static QImage getImage(GUITestOpStatus& os, QWidget* widget, bool useGrabWindow = false);

    /** Creates sub-image from the given image. Fails if 'rect' is not within the image. */
    static QImage createSubImage(GUITestOpStatus& os, const QImage& image, const QRect& rect);

    static void close(GUITestOpStatus& os, QWidget* widget);
    static void showMaximized(GUITestOpStatus& os, QWidget* widget);
    static void showNormal(GUITestOpStatus& os, QWidget* widget);

    static void clickLabelLink(GUITestOpStatus& os, QWidget* label, int step = 10, int indent = 0);
    static void clickWindowTitle(GUITestOpStatus& os, QWidget* window);
    static void moveWidgetTo(GUITestOpStatus& os, QWidget* window, const QPoint& point);
    static void resizeWidget(GUITestOpStatus& os, QWidget* widget, const QSize& size);
    static QPoint getWidgetGlobalTopLeftPoint(GUITestOpStatus& os, QWidget* widget);

    static QWidget* getActiveModalWidget(GUITestOpStatus& os);
    static QWidget* getActivePopupWidget(GUITestOpStatus& os);
    static QMenu* getActivePopupMenu(GUITestOpStatus& os);

    static void checkEnabled(GUITestOpStatus& os, QWidget* widget, bool expectedEnabledState = true);
    static void checkEnabled(GUITestOpStatus& os, const QString& widgetName, bool expectedEnabledState = true, QWidget* parent = nullptr);

    static void scrollToIndex(GUITestOpStatus& os, QAbstractItemView* itemView, const QModelIndex& index);

#define GT_CLASS_NAME "GTWidget"
#define GT_METHOD_NAME "findExactWidget"
    template<class T>
    static T findExactWidget(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {}) {
        QWidget* w = findWidget(os, widgetName, parentWidget, options);
        T result = qobject_cast<T>(w);
        if (options.failIfNotFound) {
            GT_CHECK_RESULT(w != nullptr, "widget " + widgetName + " not found", result);
            GT_CHECK_RESULT(result != nullptr, "widget of specified class not found, but there is another widget with the same name, its class is: " + QString(w->metaObject()->className()), result);
        }
        return result;
    }
#undef GT_METHOD_NAME

    /** Calls findExactWidget with QRadioButton type. Shortcut method. */
    static QRadioButton* findRadioButton(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QGroupBox type. Shortcut method. */
    static QGroupBox* findGroupBox(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QLineEdit type. Shortcut method. */
    static QLineEdit* findLineEdit(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTreeWidget type. Shortcut method. */
    static QTreeView* findTreeView(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTextEdit type. Shortcut method. */
    static QTextEdit* findTextEdit(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTableWidget type. Shortcut method. */
    static QTableWidget* findTableWidget(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTabWidget type. Shortcut method. */
    static QTabWidget* findTabWidget(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QPlainTextEdit type. Shortcut method. */
    static QPlainTextEdit* findPlainTextEdit(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QCheckBox type. Shortcut method. */
    static QCheckBox* findCheckBox(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QComboBox type. Shortcut method. */
    static QComboBox* findComboBox(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QSpinBox type. Shortcut method. */
    static QSpinBox* findSpinBox(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QDoubleSpinBox type. Shortcut method. */
    static QDoubleSpinBox* findDoubleSpinBox(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QToolButton type. Shortcut method. */
    static QToolButton* findToolButton(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QToolBar type. Shortcut method. */
    static QToolBar* findToolBar(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QScrollBar type. Shortcut method. */
    static QScrollBar* findScrollBar(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTreeWidget type. Shortcut method. */
    static QTreeWidget* findTreeWidget(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QListWidget type. Shortcut method. */
    static QListWidget* findListWidget(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QGraphicsView type. Shortcut method. */
    static QGraphicsView* findGraphicsView(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QMenu type. Shortcut method. */
    static QMenu* findMenuWidget(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QPushButton type. Shortcut method. */
    static QPushButton* findPushButton(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QSlider type. Shortcut method. */
    static QSlider* findSlider(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QSplitter type. Shortcut method. */
    static QSplitter* findSplitter(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QLabel type. Shortcut method. */
    static QLabel* findLabel(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QMdiArea type. Shortcut method. */
    static QMdiArea* findMdiArea(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QScrollArea type. Shortcut method. */
    static QScrollArea* findScrollArea(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTextBrowser type. Shortcut method. */
    static QTextBrowser* findTextBrowser(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QTableView type. Shortcut method. */
    static QTableView* findTableView(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QDialogButtonBox type. Shortcut method. */
    static QDialogButtonBox* findDialogButtonBox(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

    /** Calls findExactWidget with QProgressBar type. Shortcut method. */
    static QProgressBar* findProgressBar(GUITestOpStatus& os, const QString& widgetName, QWidget* parentWidget = nullptr, const GTGlobals::FindOptions& options = {});

#define GT_METHOD_NAME "findWidgetByType"
    /** Finds a child widget with the given type. Fails is widget can't be found. */
    template<class T>
    static T findWidgetByType(GUITestOpStatus& os, QWidget* parentWidget, const QString& errorMessage) {
        T widget = nullptr;
        for (int time = 0; time < GT_OP_WAIT_MILLIS && widget == nullptr; time += GT_OP_CHECK_MILLIS) {
            GTGlobals::sleep(time > 0 ? GT_OP_CHECK_MILLIS : 0);
            widget = parentWidget->findChild<T>();
        }
        GT_CHECK_RESULT(widget != nullptr, errorMessage, nullptr);
        return widget;
    }
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "findChildren"
    /**
     * Finds all children of the 'parent' using 'findChildren' method and checkFn to check if the child is matched.
     * If parent is null, find all child in all main window.
     * The function is run in main thread.
     */
    template<class ChildType>
    static QList<ChildType*> findChildren(GUITestOpStatus& os, QObject* parent, std::function<bool(ChildType*)> matchFn) {
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

            void run(HI::GUITestOpStatus& os) override {
                // If parent is null, start from QMainWindows.
                QList<QObject*> roots;
                QObject* parent = parentPtr;
                if (parent != nullptr) {
                    roots << parent;
                } else {
                    GT_CHECK(!useParent, "Parent object was destroyed before run(): " + parentObjectName);
                    QList<QWidget*> topLevelWidgets = GTMainWindow::getMainWindowsAsWidget(os);
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
        GTThread::runInMainThread(os, new FindChildrenScenario(parent, matchFn, result));
        return result;
    }
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME
};

}  // namespace HI

#endif
