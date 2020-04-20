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

#include <QApplication>
#include <QComboBox>
#include <QGroupBox>
#include <QListWidget>

#include <U2Core/U2IdTypes.h>
#include <U2Core/U2SafePoints.h>
#include "GTDatabaseConfig.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsTaskTreeView.h"
#include "SharedConnectionsDialogFiller.h"
#include <drivers/GTMouseDriver.h>
#include <primitives/GTWidget.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTRadioButton.h>
#include <base_dialogs/MessageBoxFiller.h>

namespace U2 {
using namespace HI;

SharedConnectionsDialogFiller::Action::Action(Type type, QString itemName)
: type(type), itemName(itemName), expectedResult(OK)
{
    dbName = GTDatabaseConfig::database();
}

SharedConnectionsDialogFiller::SharedConnectionsDialogFiller(HI::GUITestOpStatus &os, const QList<Action> &actions) :
    Filler(os, "SharedConnectionsDialog"), actions(actions)
{

}

SharedConnectionsDialogFiller::SharedConnectionsDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario) :
    Filler(os, "SharedConnectionsDialog", scenario)
{

}

namespace {

QListWidgetItem * findConnection(HI::GUITestOpStatus &os, QListWidget *list, const QString &name, GTGlobals::FindOptions options = GTGlobals::FindOptions()) {
    GTGlobals::sleep(1000);
    QList<QListWidgetItem*> items = list->findItems(name, Qt::MatchExactly);
    if (1 != items.size()) {
        if (options.failIfNotFound) {
            CHECK_SET_ERR_RESULT(false, QString("List item %1 not found").arg(name), NULL);
        }
        return NULL;
    }

    return items.first();
}

void clickConnection(HI::GUITestOpStatus &os, QListWidget *list, const QString &name) {
    QListWidgetItem *item = findConnection(os, list, name);
    CHECK_OP(os, );

    QRect rect = list->visualItemRect(item);
    QPoint point = list->mapToGlobal(rect.center());
    GTMouseDriver::moveTo(point);
    GTMouseDriver::click();
}

void checkDocument(HI::GUITestOpStatus &os, const QString &name, bool mustBe) {
    QModelIndex idx = GTUtilsProjectTreeView::findIndex(os, name, GTGlobals::FindOptions(false));
    if (!mustBe) {
        CHECK_SET_ERR(!idx.isValid(), "Document is not deleted");
    }
}

void waitForConnection(HI::GUITestOpStatus &os, const SharedConnectionsDialogFiller::Action &action) {
    switch (action.expectedResult) {
        case SharedConnectionsDialogFiller::Action::OK:
            break;
        case SharedConnectionsDialogFiller::Action::WRONG_DATA:
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "OK", "Unable to connect"));
            break;
        case SharedConnectionsDialogFiller::Action::INITIALIZE:
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
            break;
        case SharedConnectionsDialogFiller::Action::DONT_INITIALIZE:
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No));
            break;
        case SharedConnectionsDialogFiller::Action::VERSION:
            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, "OK", "recent version of UGENE"));
            break;
        case SharedConnectionsDialogFiller::Action::LOGIN:
            break;
        default:
            os.setError("Unknown expected result");
    }
}

void establishConnection(HI::GUITestOpStatus &os, const SharedConnectionsDialogFiller::Action &action)
{
    GTGlobals::sleep(1000);
    QWidget* dialog = QApplication::activeModalWidget();

    waitForConnection(os, action);
    CHECK_OP(os, );

    GTWidget::click(os, GTWidget::findWidget(os,"pbConnect", dialog));

    GTGlobals::sleep(2000);
    GTUtilsTaskTreeView::waitTaskFinished(os);
}

void deleteConnection(HI::GUITestOpStatus &os, const SharedConnectionsDialogFiller::Action &action) {
    QListWidget *list = dynamic_cast<QListWidget*>(GTWidget::findWidget(os, "lwConnections"));

    GTWidget::click(os, GTWidget::findWidget(os,"pbDelete"));
    GTGlobals::sleep(2000);

    // Check connection item
    QListWidgetItem *item = findConnection(os, list, action.itemName, GTGlobals::FindOptions(false));
    CHECK_SET_ERR(NULL == item, "Item is not deleted");

    // Check project view
    checkDocument(os, action.dbName, false);
}

void stopConnection(HI::GUITestOpStatus &os, const SharedConnectionsDialogFiller::Action &action) {
    QWidget *cnctBtn = GTWidget::findWidget(os,"pbConnect");
    QWidget *dcntBtn = GTWidget::findWidget(os,"pbDisconnect");
    QWidget *editBtn = GTWidget::findWidget(os,"pbEdit");

    GTWidget::click(os, GTWidget::findWidget(os,"pbDisconnect"));
    GTGlobals::sleep(2000);

    CHECK_SET_ERR(cnctBtn->isEnabled(), "connect button disabled");
    CHECK_SET_ERR(editBtn->isEnabled(), "edit button disabled");
    CHECK_SET_ERR(!dcntBtn->isEnabled(), "disconnect button enabled");

    // Check project view
    checkDocument(os, action.dbName, false);
}

}

#define GT_CLASS_NAME "GTUtilsDialog::SharedConnectionsDialogFiller"
#define GT_METHOD_NAME "commonScenario"

void SharedConnectionsDialogFiller::commonScenario() {
    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");
    QListWidget *list = dynamic_cast<QListWidget*>(GTWidget::findWidget(os, "lwConnections", dialog));
    CHECK_SET_ERR(NULL != list, "NULL list");
    GTGlobals::sleep(1000);

    bool connected = false;
    foreach (const Action &action, actions) {
        CHECK_SET_ERR(!connected, "The dialog must be closed but not all actions are processed");
        switch (action.type) {
            case Action::ADD:
                GTWidget::click(os, GTWidget::findWidget(os,"pbAdd"));
                break;
            case Action::CLICK:
                clickConnection(os, list, action.itemName);
                break;
            case Action::EDIT:
                GTWidget::click(os, GTWidget::findWidget(os,"pbEdit"));
                break;
            case Action::DELETE:
                deleteConnection(os, action);
                CHECK_OP(os, );
                break;
            case Action::CONNECT:
                establishConnection(os, action);
                CHECK_OP(os, );
                if (Action::OK == action.expectedResult) {
                    connected = true;
                }
                break;
            case Action::DISCONNECT:
                stopConnection(os, action);
                CHECK_OP(os, );
                break;
            case Action::CLOSE:
                GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Close);
                break;
            default:
                os.setError("Unknown action type");
                return;
        }
        CHECK_OP(os, );
    }
    GTGlobals::sleep();
}

#undef GT_METHOD_NAME
#undef GT_CLASS_NAME

} // U2
