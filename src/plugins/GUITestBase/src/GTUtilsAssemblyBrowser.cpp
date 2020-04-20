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

#include <QSharedPointer>
#include <QApplication>
#include <QLabel>
#include <QScrollBar>
#include <QLineEdit>

#include <U2View/AssemblyBrowser.h>
#include <U2View/AssemblyModel.h>
#include <U2Core/U2SafePoints.h>

#include "GTUtilsAssemblyBrowser.h"
#include "utils/GTUtilsDialog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsOptionsPanel.h"
#include "GTUtilsProjectTreeView.h"
#include "GTGlobals.h"
#include <drivers/GTKeyboardDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTToolbar.h>
#include <primitives/GTWidget.h>
#include "primitives/PopupChooser.h"
#include <utils/GTThread.h>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "GTUtilsAssemblyBrowser"

#define GT_METHOD_NAME "getView"
AssemblyBrowserUi *GTUtilsAssemblyBrowser::getView(HI::GUITestOpStatus& os, const QString &viewTitle) {
    Q_UNUSED(os);
    AssemblyBrowserUi* view;

    if(viewTitle.isEmpty()){
        view = GTUtilsMdi::activeWindow(os)->findChild<AssemblyBrowserUi*>();
        GT_CHECK_RESULT(NULL != view, "Active windows is not assembly browser", NULL);
    }else {
        const QString objectName = "assembly_browser_" + viewTitle;
        view = qobject_cast<AssemblyBrowserUi*>(GTWidget::findWidget(os, objectName));
        GT_CHECK_RESULT(NULL != view, "Assembly browser wasn't found", NULL);
    }

    return view;
}
#undef GT_METHOD_NAME

void GTUtilsAssemblyBrowser::addRefFromProject(HI::GUITestOpStatus &os, QString docName, QModelIndex parent){
    QWidget* renderArea = GTWidget::findWidget(os, "assembly_reads_area");
    QModelIndex ref = GTUtilsProjectTreeView::findIndex(os, docName, parent);
    GTUtilsProjectTreeView::dragAndDrop(os, ref, renderArea);
}

#define GT_METHOD_NAME "hasReference"
bool GTUtilsAssemblyBrowser::hasReference(HI::GUITestOpStatus& os, const QString &viewTitle) {
    Q_UNUSED(os);

    AssemblyBrowserUi* view = getView(os, viewTitle);
    GT_CHECK_RESULT(NULL != view, "Assembly browser wasn't found", false);

    return hasReference(os, view);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasReference"
bool GTUtilsAssemblyBrowser::hasReference(HI::GUITestOpStatus& os, QWidget* view) {
    Q_UNUSED(os);

    if (NULL == view) {
        view = GTUtilsMdi::activeWindow(os);
    }
    GT_CHECK_RESULT(NULL != view, "View is NULL", false);

    AssemblyBrowserUi* assemblyBrowser = view->findChild<AssemblyBrowserUi*>("assembly_browser_" + view->objectName());
    GT_CHECK_RESULT(NULL != assemblyBrowser, "Assembly browser wasn't found", false);

    return hasReference(os, assemblyBrowser);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "hasReference"
bool GTUtilsAssemblyBrowser::hasReference(HI::GUITestOpStatus &os, AssemblyBrowserUi *assemblyBrowser) {
    Q_UNUSED(os);
    GT_CHECK_RESULT(NULL != assemblyBrowser, "Assembly browser is NULL", false);

    QSharedPointer<AssemblyModel> model = assemblyBrowser->getModel();
    GT_CHECK_RESULT(!model.isNull(), "Assembly model is NULL", false);

    return model->hasReference();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getLength"
qint64 GTUtilsAssemblyBrowser::getLength(HI::GUITestOpStatus &os) {
    QWidget *mdi = GTUtilsMdi::activeWindow(os);

    QWidget* infoOptionsPanel = GTWidget::findWidget(os, "OP_OPTIONS_WIDGET", mdi);
    if (!infoOptionsPanel->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "OP_ASS_INFO", mdi));
        infoOptionsPanel = GTWidget::findWidget(os, "OP_OPTIONS_WIDGET", mdi);
    }
    GT_CHECK_RESULT(NULL != infoOptionsPanel, "Information options panel wasn't found", 0);

    QWidget* b = GTWidget::findWidget(os, "leLength", infoOptionsPanel);
    QLineEdit* leLength = qobject_cast<QLineEdit*>(b);
    GT_CHECK_RESULT(NULL != leLength, "Length line edit wasn't found", 0);

    bool isConverted = false;
    QString lengthString = leLength->text();
    lengthString.replace(" ", "");
    qint64 value = lengthString.toLongLong(&isConverted);
    GT_CHECK_RESULT(isConverted, QString("Can't convert length to number: '%1'").arg(lengthString), 0);

    return value;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getReadsCount"
qint64 GTUtilsAssemblyBrowser::getReadsCount(HI::GUITestOpStatus &os) {
    QWidget *mdi = GTUtilsMdi::activeWindow(os);

    QWidget* infoOptionsPanel = GTWidget::findWidget(os, "OP_OPTIONS_WIDGET", mdi);
    if (!infoOptionsPanel->isVisible()) {
        GTWidget::click(os, GTWidget::findWidget(os, "OP_ASS_INFO", mdi));
        infoOptionsPanel = GTWidget::findWidget(os, "OP_OPTIONS_WIDGET", mdi);
    }
    GT_CHECK_RESULT(NULL != infoOptionsPanel, "Information options panel wasn't found", 0);

    QLineEdit* leReads = qobject_cast<QLineEdit*>(GTWidget::findWidget(os, "leReads", infoOptionsPanel));
    GT_CHECK_RESULT(NULL != leReads, "Length line edit wasn't found", 0);

    bool isConverted = false;
    QString readsString = leReads->text();
    readsString.replace(" ", "");
    qint64 value = readsString.toLongLong(&isConverted);
    GT_CHECK_RESULT(isConverted, QString("Can't convert reads count to number: '%1'").arg(readsString), 0);

    return value;
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "isWelcomeScreenVisible"
bool GTUtilsAssemblyBrowser::isWelcomeScreenVisible(HI::GUITestOpStatus &os) {
    QWidget *coveredRegionsLabel = GTWidget::findWidget(os, "CoveredRegionsLabel", GTUtilsMdi::activeWindow(os));
    GT_CHECK_RESULT(NULL != coveredRegionsLabel, "coveredRegionsLabel is NULL", false);
    return coveredRegionsLabel->isVisible();
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomIn"
void GTUtilsAssemblyBrowser::zoomIn(HI::GUITestOpStatus &os, Method method) {
    switch (method) {
    case Button:
        GTToolbar::clickButtonByTooltipOnToolbar(os, MWTOOLBAR_ACTIVEMDI, "Zoom in");
        break;
    case Hotkey:
        if(!GTWidget::findWidget(os, "assembly_reads_area")->hasFocus()){
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
void GTUtilsAssemblyBrowser::zoomToMax(HI::GUITestOpStatus &os) {
    Q_UNUSED(os);

    QToolBar* toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    GT_CHECK(NULL != toolbar, "Can't find the toolbar");

    QWidget* zoomInButton = GTToolbar::getWidgetForActionTooltip(os, toolbar, "Zoom in");
    GT_CHECK(NULL != zoomInButton, "Can't find the 'Zoom in' button");

    while (zoomInButton->isEnabled()) {
        GTWidget::click(os, zoomInButton);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomToMin"
void GTUtilsAssemblyBrowser::zoomToMin(HI::GUITestOpStatus &os) {
    Q_UNUSED(os);

    QToolBar* toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    GT_CHECK(NULL != toolbar, "Can't find the toolbar");

    QWidget* zoomOutButton = GTToolbar::getWidgetForActionTooltip(os, toolbar, "Zoom out");
    GT_CHECK(NULL != zoomOutButton, "Can't find the 'Zoom in' button");

    while (zoomOutButton->isEnabled()) {
        GTWidget::click(os, zoomOutButton);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "zoomToReads"
void GTUtilsAssemblyBrowser::zoomToReads(GUITestOpStatus &os){
    QLabel* coveredRegionsLabel = GTWidget::findExactWidget<QLabel*>(os, "CoveredRegionsLabel");
    emit coveredRegionsLabel->linkActivated("zoom");
    GTGlobals::sleep(1000);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "goToPosition"
void GTUtilsAssemblyBrowser::goToPosition(HI::GUITestOpStatus &os, qint64 position, Method method) {
    Q_UNUSED(os);

    QToolBar* toolbar = GTToolbar::getToolbar(os, MWTOOLBAR_ACTIVEMDI);
    GT_CHECK(NULL != toolbar, "Can't find the toolbar");

    QLineEdit* positionLineEdit = GTWidget::findExactWidget<QLineEdit*>(os, "go_to_pos_line_edit", toolbar);
    GTLineEdit::setText(os, positionLineEdit,QString::number(position));

    switch (method) {
    case Button:
        GTWidget::click(os, GTWidget::findWidget(os, "Go!"));
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
void GTUtilsAssemblyBrowser::callContextMenu(HI::GUITestOpStatus &os, GTUtilsAssemblyBrowser::Area area) {
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
void GTUtilsAssemblyBrowser::callExportCoverageDialog(HI::GUITestOpStatus &os, Area area) {
    Q_UNUSED(os);

    switch (area) {
    case Consensus:
        GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Export coverage"));
        break;
    case Overview:
        GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Export coverage"));
        break;
    case Reads:
        GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << "Export" << "Export coverage"));
        break;
    default:
        os.setError("Can't call the dialog on this area");
        FAIL(false, );
    }

    callContextMenu(os, area);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "getScrollBar"
QScrollBar* GTUtilsAssemblyBrowser::getScrollBar(GUITestOpStatus &os, Qt::Orientation orientation){
    AssemblyBrowserUi* ui = GTUtilsMdi::activeWindow(os)->findChild<AssemblyBrowserUi*>();
    GT_CHECK_RESULT(ui != NULL, "Assembly browser not found", NULL);

    QList<QScrollBar*> scrollBars = ui->findChildren<QScrollBar*>();
    foreach (QScrollBar* bar, scrollBars) {
        if(bar->orientation() == orientation){
            return bar;
        }
    }

    GT_CHECK_RESULT(false, QString("Scrollbar with orientation %1 not found").arg(orientation), NULL);
}
#undef GT_METHOD_NAME


#undef GT_CLASS_NAME

}   // namespace U2
