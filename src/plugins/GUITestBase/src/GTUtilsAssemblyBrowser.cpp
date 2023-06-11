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

#include "GTUtilsAssemblyBrowser.h"
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QScrollBar>
#include <QSharedPointer>

#include <U2Core/U2SafePoints.h>

#include <U2View/AssemblyBrowser.h>
#include <U2View/AssemblyBrowserFactory.h>
#include <U2View/AssemblyModel.h>

#include "GTGlobals.h"
#include "GTUtilsMdi.h"
#include "GTUtilsProjectTreeView.h"
#include "primitives/PopupChooser.h"
#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsAssemblyBrowser"

#define GT_METHOD_NAME "getActiveAssemblyBrowserWindow"
QWidget* GTUtilsAssemblyBrowser::getActiveAssemblyBrowserWindow() {
    QWidget* widget = GTUtilsMdi::getActiveObjectViewWindow(AssemblyBrowserFactory::ID);
    GTThread::waitForMainThread();
    return widget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkAssemblyBrowserWindowIsActive"
void GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive() {
    getActiveAssemblyBrowserWindow();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getView"
AssemblyBrowserUi* GTUtilsAssemblyBrowser::getView(const QString& viewTitle) {
    if (viewTitle.isEmpty()) {
        checkAssemblyBrowserWindowIsActive();
        QWidget* assemblyBrowserWindow = getActiveAssemblyBrowserWindow();
        AssemblyBrowserUi* view = assemblyBrowserWindow->findChild<AssemblyBrowserUi*>();
        GT_CHECK_RESULT(view != nullptr, "Active windows is not assembly browser", nullptr);
        return view;
    }
    QString objectName = "assembly_browser_" + viewTitle;
    return GTWidget::findExactWidget<AssemblyBrowserUi*>(objectName);
}
#undef GT_METHOD_NAME

void GTUtilsAssemblyBrowser::addRefFromProject(QString docName, QModelIndex parent) {
    checkAssemblyBrowserWindowIsActive();
    auto renderArea = GTWidget::findWidget("assembly_reads_area");
    QModelIndex ref = GTUtilsProjectTreeView::findIndex(docName, parent);
    GTUtilsProjectTreeView::dragAndDrop(ref, renderArea);
}

#define GT_METHOD_NAME "hasReference"
bool GTUtilsAssemblyBrowser::hasReference(const QString& viewTitle) {
    AssemblyBrowserUi* view = getView(viewTitle);
    GT_CHECK_RESULT(nullptr != view, "Assembly browser wasn't found", false);
    return hasReference(view);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasReference"
bool GTUtilsAssemblyBrowser::hasReference(QWidget* view) {
    if (view == nullptr) {
        view = getActiveAssemblyBrowserWindow();
    }
    QString objectName = "assembly_browser_" + view->objectName();
    auto assemblyBrowser = GTWidget::findExactWidget<AssemblyBrowserUi*>(objectName);
    return hasReference(assemblyBrowser);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasReference"
bool GTUtilsAssemblyBrowser::hasReference(AssemblyBrowserUi* assemblyBrowser) {
    GT_CHECK_RESULT(assemblyBrowser != nullptr, "Assembly browser is NULL", false);

    QSharedPointer<AssemblyModel> model = assemblyBrowser->getModel();
    GT_CHECK_RESULT(!model.isNull(), "Assembly model is NULL", false);

    return model->hasReference();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLength"
qint64 GTUtilsAssemblyBrowser::getLength() {
    QWidget* mdi = getActiveAssemblyBrowserWindow();

    auto infoOptionsPanel = GTWidget::findWidget("OP_OPTIONS_WIDGET", mdi);
    if (!infoOptionsPanel->isVisible()) {
        GTWidget::click(GTWidget::findWidget("OP_ASS_INFO", mdi));
        infoOptionsPanel = GTWidget::findWidget("OP_OPTIONS_WIDGET", mdi);
    }
    auto leLength = GTWidget::findLineEdit("leLength", infoOptionsPanel);

    bool isConverted = false;
    QString lengthString = leLength->text();
    lengthString.replace(" ", "");
    qint64 value = lengthString.toLongLong(&isConverted);
    GT_CHECK_RESULT(isConverted, QString("Can't convert length to number: '%1'").arg(lengthString), 0);

    return value;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReadsCount"
qint64 GTUtilsAssemblyBrowser::getReadsCount() {
    QWidget* mdi = getActiveAssemblyBrowserWindow();

    auto infoOptionsPanel = GTWidget::findWidget("OP_OPTIONS_WIDGET", mdi);
    if (!infoOptionsPanel->isVisible()) {
        GTWidget::click(GTWidget::findWidget("OP_ASS_INFO", mdi));
        infoOptionsPanel = GTWidget::findWidget("OP_OPTIONS_WIDGET", mdi);
    }
    auto leReads = GTWidget::findLineEdit("leReads", infoOptionsPanel);

    bool isConverted = false;
    QString readsString = leReads->text();
    readsString.replace(" ", "");
    qint64 value = readsString.toLongLong(&isConverted);
    GT_CHECK_RESULT(isConverted, QString("Can't convert reads count to number: '%1'").arg(readsString), 0);

    return value;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isWelcomeScreenVisible"
bool GTUtilsAssemblyBrowser::isWelcomeScreenVisible() {
    QWidget* window = getActiveAssemblyBrowserWindow();
    auto coveredRegionsLabel = GTWidget::findWidget("CoveredRegionsLabel", window);
    return coveredRegionsLabel->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomIn"
void GTUtilsAssemblyBrowser::zoomIn(Method method) {
    checkAssemblyBrowserWindowIsActive();
    switch (method) {
        case Button:
            GTToolbar::clickButtonByTooltipOnToolbar(MWTOOLBAR_ACTIVEMDI, "Zoom in");
            break;
        case Hotkey:
            if (!GTWidget::findWidget("assembly_reads_area")->hasFocus()) {
                GTWidget::click(GTWidget::findWidget("assembly_reads_area"));
            }
            GTKeyboardDriver::keyClick('+');
            break;
        default:
            break;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomToMax"
void GTUtilsAssemblyBrowser::zoomToMax() {
    checkAssemblyBrowserWindowIsActive();
    QToolBar* toolbar = GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI);
    GT_CHECK(toolbar != nullptr, "Can't find the toolbar");

    QWidget* zoomInButton = GTToolbar::getWidgetForActionTooltip(toolbar, "Zoom in");
    GT_CHECK(zoomInButton != nullptr, "Can't find the 'Zoom in' button");

    while (zoomInButton->isEnabled()) {
        GTWidget::click(zoomInButton);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomToMin"
void GTUtilsAssemblyBrowser::zoomToMin() {
    checkAssemblyBrowserWindowIsActive();

    QToolBar* toolbar = GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI);
    GT_CHECK(toolbar != nullptr, "Can't find the toolbar");

    QWidget* zoomOutButton = GTToolbar::getWidgetForActionTooltip(toolbar, "Zoom out");
    GT_CHECK(zoomOutButton != nullptr, "Can't find the 'Zoom in' button");

    while (zoomOutButton->isEnabled()) {
        GTWidget::click(zoomOutButton);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomToReads"
void GTUtilsAssemblyBrowser::zoomToReads() {
    checkAssemblyBrowserWindowIsActive();
    auto coveredRegionsLabel = GTWidget::findLabel("CoveredRegionsLabel");
    emit coveredRegionsLabel->linkActivated("zoom");
    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "goToPosition"
void GTUtilsAssemblyBrowser::goToPosition(qint64 position, Method method) {
    checkAssemblyBrowserWindowIsActive();

    QToolBar* toolbar = GTToolbar::getToolbar(MWTOOLBAR_ACTIVEMDI);
    GT_CHECK(toolbar != nullptr, "Can't find the toolbar");

    GTLineEdit::setText("go_to_pos_line_edit", QString::number(position), toolbar);

    switch (method) {
        case Button:
            GTWidget::click(GTWidget::findWidget("goButton"));
            break;
        default:
            GTKeyboardDriver::keyClick(Qt::Key_Enter);
            break;
    }
    GTGlobals::sleep(1000);
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callContextMenu"
void GTUtilsAssemblyBrowser::callContextMenu(GTUtilsAssemblyBrowser::Area area) {
    checkAssemblyBrowserWindowIsActive();
    QString widgetName;
    switch (area) {
        case Consensus:
            widgetName = "Consensus area";
            break;
        case Overview:
            widgetName = "Zoomable assembly overview";
            break;
        case Reads:
            widgetName = "assembly_reads_area";
            break;
        default:
            FAIL("Can't find the area", );
    }

    GTWidget::click(GTWidget::findWidget(widgetName), Qt::RightButton);
    GTGlobals::sleep(300);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callExportCoverageDialog"
void GTUtilsAssemblyBrowser::callExportCoverageDialog(Area area) {
    checkAssemblyBrowserWindowIsActive();

    switch (area) {
        case Consensus:
            GTUtilsDialog::waitForDialog(new PopupChooser({"Export coverage"}));
            break;
        case Overview:
            GTUtilsDialog::waitForDialog(new PopupChooser({"Export coverage"}));
            break;
        case Reads:
            GTUtilsDialog::waitForDialog(new PopupChooser({"Export", "Export coverage"}));
            break;
        default:
            FAIL("Can't call the dialog on this area", );
    }

    callContextMenu(area);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getScrollBar"
QScrollBar* GTUtilsAssemblyBrowser::getScrollBar(Qt::Orientation orientation) {
    AssemblyBrowserUi* ui = getView();
    QList<QScrollBar*> scrollBars = ui->findChildren<QScrollBar*>();
    for (QScrollBar* bar : qAsConst(scrollBars)) {
        if (bar->orientation() == orientation) {
            return bar;
        }
    }
    GT_FAIL(QString("Scrollbar with orientation %1 not found").arg(orientation), nullptr);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "scrollToStart"
void GTUtilsAssemblyBrowser::scrollToStart(Qt::Orientation orientation) {
    QScrollBar* scrollBar = getScrollBar(orientation);
    class MainThreadAction : public CustomScenario {
    public:
        MainThreadAction(QScrollBar* _scrollbar)
            : scrollbar(_scrollbar) {
        }
        void run() override {
            scrollbar->setValue(0);
        }
        QScrollBar* scrollbar = nullptr;
    };
    GTThread::runInMainThread(new MainThreadAction(scrollBar));
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
