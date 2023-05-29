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
QWidget* GTUtilsAssemblyBrowser::getActiveAssemblyBrowserWindow(GUITestOpStatus& os) {
    QWidget* widget = GTUtilsMdi::getActiveObjectViewWindow(os, AssemblyBrowserFactory::ID);
    GTThread::waitForMainThread();
    return widget;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "checkAssemblyBrowserWindowIsActive"
void GTUtilsAssemblyBrowser::checkAssemblyBrowserWindowIsActive(GUITestOpStatus& os) {
    getActiveAssemblyBrowserWindow(os);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getView"
AssemblyBrowserUi* GTUtilsAssemblyBrowser::getView(HI::GUITestOpStatus& os, const QString& viewTitle) {
    if (viewTitle.isEmpty()) {
        checkAssemblyBrowserWindowIsActive(os);
        QWidget* assemblyBrowserWindow = getActiveAssemblyBrowserWindow(os);
        AssemblyBrowserUi* view = assemblyBrowserWindow->findChild<AssemblyBrowserUi*>();
        GT_CHECK_RESULT(view != nullptr, "Active windows is not assembly browser", nullptr);
        return view;
    }
    QString objectName = "assembly_browser_" + viewTitle;
    return GTWidget::findExactWidget<AssemblyBrowserUi*>(os, objectName);
}
#undef GT_METHOD_NAME

void GTUtilsAssemblyBrowser::addRefFromProject(HI::GUITestOpStatus& os, QString docName, QModelIndex parent) {
    checkAssemblyBrowserWindowIsActive(os);
    auto renderArea = GTWidget::findWidget(os, "assembly_reads_area");
    QModelIndex ref = GTUtilsProjectTreeView::findIndex(os, docName, parent);
    GTUtilsProjectTreeView::dragAndDrop(os, ref, renderArea);
}

#define GT_METHOD_NAME "hasReference"
bool GTUtilsAssemblyBrowser::hasReference(HI::GUITestOpStatus& os, const QString& viewTitle) {
    AssemblyBrowserUi* view = getView(os, viewTitle);
    GT_CHECK_RESULT(nullptr != view, "Assembly browser wasn't found", false);
    return hasReference(os, view);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasReference"
bool GTUtilsAssemblyBrowser::hasReference(HI::GUITestOpStatus& os, QWidget* view) {
    if (view == nullptr) {
        view = getActiveAssemblyBrowserWindow(os);
    }
    QString objectName = "assembly_browser_" + view->objectName();
    auto assemblyBrowser = GTWidget::findExactWidget<AssemblyBrowserUi*>(os, objectName);
    return hasReference(os, assemblyBrowser);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasReference"
bool GTUtilsAssemblyBrowser::hasReference(HI::GUITestOpStatus& os, AssemblyBrowserUi* assemblyBrowser) {
    GT_CHECK_RESULT(assemblyBrowser != nullptr, "Assembly browser is NULL", false);

    QSharedPointer<AssemblyModel> model = assemblyBrowser->getModel();
    GT_CHECK_RESULT(!model.isNull(), "Assembly model is NULL", false);

    return model->hasReference();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLength"
qint64 GTUtilsAssemblyBrowser::getLength(HI::GUITestOpStatus& os) {
    QWidget* mdi = getActiveAssemblyBrowserWindow(os);

    auto infoOptionsPanel = GTWidget::findWidget(os, "OP_OPTIONS_WIDGET", mdi);
    if (!infoOptionsPanel->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "OP_ASS_INFO", mdi));
        infoOptionsPanel = GTWidget::findWidget(os, "OP_OPTIONS_WIDGET", mdi);
    }
    auto leLength = GTWidget::findLineEdit(os, "leLength", infoOptionsPanel);

    bool isConverted = false;
    QString lengthString = leLength->text();
    lengthString.replace(" ", "");
    qint64 value = lengthString.toLongLong(&isConverted);
    GT_CHECK_RESULT(isConverted, QString("Can't convert length to number: '%1'").arg(lengthString), 0);

    return value;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReadsCount"
qint64 GTUtilsAssemblyBrowser::getReadsCount(HI::GUITestOpStatus& os) {
    QWidget* mdi = getActiveAssemblyBrowserWindow(os);

    auto infoOptionsPanel = GTWidget::findWidget(os, "OP_OPTIONS_WIDGET", mdi);
    if (!infoOptionsPanel->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "OP_ASS_INFO", mdi));
        infoOptionsPanel = GTWidget::findWidget(os, "OP_OPTIONS_WIDGET", mdi);
    }
    auto leReads = GTWidget::findLineEdit(os, "leReads", infoOptionsPanel);

    bool isConverted = false;
    QString readsString = leReads->text();
    readsString.replace(" ", "");
    qint64 value = readsString.toLongLong(&isConverted);
    GT_CHECK_RESULT(isConverted, QString("Can't convert reads count to number: '%1'").arg(readsString), 0);

    return value;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isWelcomeScreenVisible"
bool GTUtilsAssemblyBrowser::isWelcomeScreenVisible(HI::GUITestOpStatus& os) {
    QWidget* window = getActiveAssemblyBrowserWindow(os);
    auto coveredRegionsLabel = GTWidget::findWidget(os, "CoveredRegionsLabel", window);
    return coveredRegionsLabel->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomIn"
void GTUtilsAssemblyBrowser::zoomIn(HI::GUITestOpStatus& os, Method method) {
    checkAssemblyBrowserWindowIsActive(os);
    switch (method) {
        case Button:
            GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Zoom in");
            break;
        case Hotkey:
            if (!GTWidget::findWidget(os, "assembly_reads_area")->hasFocus()) {
                GTWidget::click(os, GTWidget::findWidget(os, "assembly_reads_area"));
            }
            GTKeyboardDriver::keyClick('+');
            break;
        default:
            break;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomToMax"
void GTUtilsAssemblyBrowser::zoomToMax(HI::GUITestOpStatus& os) {
    checkAssemblyBrowserWindowIsActive(os);
    QToolBar* toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    GT_CHECK(toolbar != nullptr, "Can't find the toolbar");

    QWidget* zoomInButton = GTToolbar::getWidgetForActionTooltip(os, toolbar, "Zoom in");
    GT_CHECK(zoomInButton != nullptr, "Can't find the 'Zoom in' button");

    while (zoomInButton->isEnabled()) {
        GTWidget::click(os, zoomInButton);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomToMin"
void GTUtilsAssemblyBrowser::zoomToMin(HI::GUITestOpStatus& os) {
    checkAssemblyBrowserWindowIsActive(os);

    QToolBar* toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    GT_CHECK(toolbar != nullptr, "Can't find the toolbar");

    QWidget* zoomOutButton = GTToolbar::getWidgetForActionTooltip(os, toolbar, "Zoom out");
    GT_CHECK(zoomOutButton != nullptr, "Can't find the 'Zoom in' button");

    while (zoomOutButton->isEnabled()) {
        GTWidget::click(os, zoomOutButton);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomToReads"
void GTUtilsAssemblyBrowser::zoomToReads(GUITestOpStatus& os) {
    checkAssemblyBrowserWindowIsActive(os);
    auto coveredRegionsLabel = GTWidget::findLabel(os, "CoveredRegionsLabel");
    emit coveredRegionsLabel->linkActivated("zoom");
    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "goToPosition"
void GTUtilsAssemblyBrowser::goToPosition(HI::GUITestOpStatus& os, qint64 position, Method method) {
    checkAssemblyBrowserWindowIsActive(os);

    QToolBar* toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    GT_CHECK(toolbar != nullptr, "Can't find the toolbar");

    GTLineEdit::setText(os, "go_to_pos_line_edit", QString::number(position), toolbar);

    switch (method) {
        case Button:
            GTWidget::click(os, GTWidget::findWidget(os, "goButton"));
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
void GTUtilsAssemblyBrowser::callContextMenu(HI::GUITestOpStatus& os, GTUtilsAssemblyBrowser::Area area) {
    checkAssemblyBrowserWindowIsActive(os);
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
            os.setError("Can't find the area");
            FAIL(false, );
    }

    GTWidget::click(os, GTWidget::findWidget(os, widgetName), Qt::RightButton);
    GTGlobals::sleep(300);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "callExportCoverageDialog"
void GTUtilsAssemblyBrowser::callExportCoverageDialog(HI::GUITestOpStatus& os, Area area) {
    checkAssemblyBrowserWindowIsActive(os);

    switch (area) {
        case Consensus:
            GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Export coverage"}));
            break;
        case Overview:
            GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Export coverage"}));
            break;
        case Reads:
            GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"Export", "Export coverage"}));
            break;
        default:
            os.setError("Can't call the dialog on this area");
            FAIL(false, );
    }

    callContextMenu(os, area);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getScrollBar"
QScrollBar* GTUtilsAssemblyBrowser::getScrollBar(GUITestOpStatus& os, Qt::Orientation orientation) {
    AssemblyBrowserUi* ui = getView(os);
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
void GTUtilsAssemblyBrowser::scrollToStart(GUITestOpStatus& os, Qt::Orientation orientation) {
    QScrollBar* scrollBar = getScrollBar(os, orientation);
    class MainThreadAction : public CustomScenario {
    public:
        MainThreadAction(QScrollBar* _scrollbar)
            : scrollbar(_scrollbar) {
        }
        void run(HI::GUITestOpStatus&) override {
            scrollbar->setValue(0);
        }
        QScrollBar* scrollbar = nullptr;
    };
    GTThread::runInMainThread(os, new MainThreadAction(scrollBar));
    GTThread::waitForMainThread();
}
#undef GT_METHOD_NAME

#undef GT_CLASS_NAME

}  // namespace U2
