/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include <base_dialogs/GTFileDialog.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTMenu.h>
#include <primitives/GTTreeView.h>
#include <primitives/GTWidget.h>
#include <primitives/PopupChooser.h>
#include <system/GTFile.h>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QListWidget>
#include <QTreeWidgetItem>

#include <U2Core/ImportToDatabaseOptions.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2SafePoints.h>

#include <U2View/AnnotationsTreeView.h>

#include "GTDatabaseConfig.h"
#include "GTTestsSharedDatabase.h"
#include "GTUtilsAnnotationsTreeView.h"
#include "GTUtilsAssemblyBrowser.h"
#include "GTUtilsLog.h"
#include "GTUtilsMdi.h"
#include "GTUtilsMsaEditor.h"
#include "GTUtilsMsaEditorSequenceArea.h"
#include "GTUtilsProjectTreeView.h"
#include "GTUtilsSequenceView.h"
#include "GTUtilsSharedDatabaseDocument.h"
#include "GTUtilsTaskTreeView.h"
#include "runnables/ugene/corelibs/U2Gui/AddFolderDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/CreateObjectRelationDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/EditConnectionDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportChromatogramFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ExportDocumentDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportBAMFileDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/ImportToDatabaseDialogFiller.h"
#include "runnables/ugene/corelibs/U2Gui/SharedConnectionsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportAnnotationsDialogFiller.h"
#include "runnables/ugene/plugins/dna_export/ExportSequencesDialogFiller.h"
#include "runnables/ugene/ugeneui/SelectDocumentFormatDialogFiller.h"

namespace U2 {
namespace GUITest_common_scenarios_shared_database {
using namespace HI;
namespace {

QListWidgetItem* getConnectionItem(HI::GUITestOpStatus& os, const QString& connectionName) {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    auto list = GTWidget::findListWidget(os, "lwConnections", dialog);
    const QList<QListWidgetItem*> items = list->findItems(connectionName, Qt::MatchExactly);
    CHECK_SET_ERR_RESULT(1 == items.size(), QString("List item '%1'' not found").arg(connectionName), nullptr);

    return items.first();
}

void checkButtonStateForConnectionItem(HI::GUITestOpStatus& os, const QString& connectionName, const QString& buttonText, bool isEnabled) {
    QWidget* dialog = GTWidget::getActiveModalWidget(os);
    GTListWidget::click(os, GTWidget::findListWidget(os, "lwConnections", dialog), connectionName);

    QWidget* button = GTWidget::findButtonByText(os, buttonText, dialog);
    CHECK_SET_ERR(nullptr != button, "Button is NULL");
    CHECK_SET_ERR(isEnabled == button->isEnabled(), QString("Button '%1' has an incorrect state").arg(buttonText));
}

void checkConnectionItemIcon(HI::GUITestOpStatus& os, const QString& connectionName, const QString& expectedIconPath) {
    QListWidgetItem* connectionItem = getConnectionItem(os, connectionName);
    CHECK_OP(os, );
    const QIcon icon = connectionItem->icon();
    const QIcon expectedIcon(expectedIconPath);
    if (!expectedIconPath.isEmpty()) {
        CHECK_SET_ERR(expectedIcon.pixmap(expectedIcon.availableSizes().first()).toImage() == icon.pixmap(icon.availableSizes().first()).toImage(), "Icon is incorrect");
    } else {
        QPixmap px(":/core/images/db/database_lightning.png");
        px.fill(Qt::transparent);
        CHECK_SET_ERR(px.toImage() == icon.pixmap(icon.availableSizes().first()).toImage(), "Icon is incorrect");
    }
}

}  // namespace

GUI_TEST_CLASS_DEFINITION(cm_test_0001) {
    // 1. File -> Shared databases.
    // Expected: The dialog appears.
    // 2. Click "Add"
    // 3. Fill the parameters:
    //     Connection name: cm_test_0001: new shared database
    // 4. Click "OK".
    // Expected: the new connection item appears in the list.
    // 5. Choose the new item and click "Connect".
    // Expected: the connection is established:
    //           1) The connection item has the "Connected" icon.
    //           2) The database document appears in the project.
    //           3) There are no errors in the log.
    //           4) The "Edit" button is disabled.
    GTLogTracer lt;
    QString conName = "cm_test_0001: new shared database";
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::ADD);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    {
        EditConnectionDialogFiller::Parameters params;
        params.connectionName = conName;
        GTUtilsDialog::waitForDialog(os, new EditConnectionDialogFiller(os, params, EditConnectionDialogFiller::FROM_SETTINGS));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());

    GTUtilsProjectTreeView::findIndex(os, "Recycle bin");

    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) {
            checkConnectionItemIcon(os, "cm_test_0001: new shared database", ":/core/images/db/database_lightning.png");
            checkButtonStateForConnectionItem(os, "cm_test_0001: new shared database", "Edit", false);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});
}

GUI_TEST_CLASS_DEFINITION(cm_test_0002) {
    // 1. File -> Shared databases.
    // Expected: The dialog appears.
    // 2. Choose the "cm_test_0002: new shared database" item and click "Connect" (or create it).
    // Expected: the connection is established:
    //           1) The connection item has the "Connected" icon.
    //           2) The database document appears in the project.
    //           3) There are no errors in the log.
    //           4) The "Edit" button is disabled.
    // 3. Click "Delete".
    // Expected: the connection is removed:
    //           1) The connection item disappears.
    //           2) The database document is not removed.
    //           3) There are no errors in the log.
    GTLogTracer lt;
    QString conName = "cm_test_0002: new shared database";
    GTDatabaseConfig::initTestConnectionInfo(conName);

    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) {
            checkConnectionItemIcon(os, "cm_test_0002: new shared database", ":/core/images/db/database_lightning.png");
            checkButtonStateForConnectionItem(os, "cm_test_0002: new shared database", "Edit", false);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::DELETE, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLOSE, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    bool exists = GTUtilsProjectTreeView::checkItem(os, conName, QModelIndex());
    CHECK_SET_ERR(exists, "A database connection is not found in the project view");

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(cm_test_0003) {
    // 1. File -> Shared databases.
    // Expected: The dialog appears.
    // 2. Choose the "cm_test_0003: new shared database" item (or create it with the "wrong_login" login).
    // 3. Click "Edit".
    // 4. Fill the parameters:
    //     Connection name: cm_test_0003: new shared database 1
    //     Login: ugene
    // 5. Click "OK".
    // Expected: the new connection name is changed in the list.
    // 6. Click "Connect".
    // Expected: 1) The "Edit" button is disabled.
    //           2) The connection is established:
    //             a) The connection item has the "Connected" icon.
    //             b) The database document appears in the project.
    //             c) There are no errors in the log.
    //             d) The "Edit" button is disabled.
    // 7. Click "Disconnect".
    // Expected: the "Edit" button is enabled.
    GTLogTracer lt;
    QString conName = "cm_test_0003: new shared database";
    QString newConName = "cm_test_0003: new shared database 1";
    GTDatabaseConfig::initTestConnectionInfo(conName, GTDatabaseConfig::database(), false);

    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::EDIT, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, newConName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, newConName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    {
        EditConnectionDialogFiller::Parameters params;
        params.connectionName = newConName;
        GTUtilsDialog::waitForDialog(os, new EditConnectionDialogFiller(os, params, EditConnectionDialogFiller::FROM_SETTINGS));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            checkConnectionItemIcon(os, "cm_test_0003: new shared database 1", ":/core/images/db/database_lightning.png");
            checkButtonStateForConnectionItem(os, "cm_test_0003: new shared database 1", "Edit", false);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, newConName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::DISCONNECT, newConName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLOSE);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    class Scenario2 : public CustomScenario {
        void run(HI::GUITestOpStatus& os) override {
            checkConnectionItemIcon(os, "cm_test_0003: new shared database 1", "");
            checkButtonStateForConnectionItem(os, "cm_test_0003: new shared database 1", "Edit", true);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, new Scenario2));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    bool exists = GTUtilsProjectTreeView::checkItem(os, conName, QModelIndex(), {false});
    CHECK_SET_ERR(!exists, "A database connection unexpectedly is presented in the project view");

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(cm_test_0004) {
    // 1. File -> Shared databases.
    // Expected: The dialog appears.
    // 2. Choose the "ugene_gui_test" item and click "Connect" (or create it).
    // Expected: the connection is established:
    //           1) The connection item has the "Connected" icon.
    //           2) The database document appears in the project.
    //           3) There are no errors in the log.
    //           4) The "Edit" button is disabled.
    GTLogTracer lt;
    QString conName = "ugene_gui_test";
    GTDatabaseConfig::initTestConnectionInfo(conName);
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) {
            checkConnectionItemIcon(os, "ugene_gui_test", ":/core/images/db/database_lightning.png");
            checkButtonStateForConnectionItem(os, "ugene_gui_test", "Edit", false);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(cm_test_0005) {
    // 1. File -> Shared databases.
    // Expected: The dialog appears.
    // 2. Choose the "ugene_gui_test" item and click "Connect".
    // Expected: the connection is established:
    //           1) The connection item has the "Connected" icon.
    //           2) The database document appears in the project.
    //           3) There are no errors in the log.
    //           4) The "Edit" button is disabled.
    // 3. Click "Disconnect".
    // Expected: the connection is removed:
    //           1) The "Connected" icon disappears from the item.
    //           2) The database document disappears from the project.
    //           3) There are no errors in the log.
    GTLogTracer lt;
    QString conName = "ugene_gui_test";
    GTDatabaseConfig::initTestConnectionInfo(conName);
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) {
            checkConnectionItemIcon(os, "ugene_gui_test", ":/core/images/db/database_lightning.png");
            checkButtonStateForConnectionItem(os, "ugene_gui_test", "Edit", false);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::DISCONNECT, conName);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLOSE, conName);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    class Scenario2 : public CustomScenario {
        void run(HI::GUITestOpStatus& os) {
            checkConnectionItemIcon(os, "ugene_gui_test", "");
            checkButtonStateForConnectionItem(os, "ugene_gui_test", "Edit", true);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, new Scenario2));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    const bool exists = GTUtilsProjectTreeView::checkItem(os, conName, QModelIndex(), {false});
    CHECK_SET_ERR(!exists, "A database connection unexpectedly is presented in the project view");

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(cm_test_0006) {
    // 1. File -> Shared databases.
    // Expected: The dialog appears.
    // 2. Choose the "cm_test_0006: uninitialized database" item and click "Connect" (or create it).
    // Expected: the question about database initializations appears.
    // 3. Click "No".
    // Expected: the connection is not established.
    // 4. Click "Connect" again.
    // Expected: the question appears again.
    // 5. Click "Yes".
    // Expected: 1) The initialization task is started and finished.
    //           2) The connection is established:
    //             a) The connection item has the "Connected" icon.
    //             b) The database document appears in the project.
    //             c) There are no errors in the log.
    //             d) The "Edit" button is disabled.
    GTLogTracer lt;

    const QString conName = "cm_test_0006: uninitialized database";
    GTDatabaseConfig::initTestConnectionInfo(conName, GTDatabaseConfig::uninitializedDatabase());

    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) {
            QWidget* dialog = GTWidget::getActiveModalWidget(os);

            GTListWidget::click(os, GTWidget::findListWidget(os, "lwConnections", dialog), "cm_test_0006: uninitialized database");

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::No, "is not initialized"));
            GTWidget::click(os, GTWidget::findWidget(os, "pbConnect", dialog));

            GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes, "is not initialized"));
            GTWidget::click(os, GTWidget::findWidget(os, "pbConnect", dialog));
        }
    };

    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    const bool exists = GTUtilsProjectTreeView::checkItem(os, conName, QModelIndex());
    CHECK_SET_ERR(exists, "A database connection not found in the project view");

    class Scenario2 : public CustomScenario {
        void run(HI::GUITestOpStatus& os) {
            QString conName = "cm_test_0006: uninitialized database";
            checkConnectionItemIcon(os, conName, ":/core/images/db/database_lightning.png");
            checkButtonStateForConnectionItem(os, conName, "Edit", false);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, new Scenario2));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});
    GTUtilsTaskTreeView::waitTaskFinished(os);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(cm_test_0007) {
    // 1. File -> Shared databases.
    // Expected: The dialog appears.
    // 2. Choose the "cm_test_0007: incompatible version database" item and click "Connect" (or create it).
    // Expected: the connection is not established; the error message about version appears.
    QString conName = "cm_test_0007: uninitialized database";
    GTDatabaseConfig::initTestConnectionInfo(conName, GTDatabaseConfig::versionDatabase());
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);

        SharedConnectionsDialogFiller::Action cAction(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        cAction.dbName = GTDatabaseConfig::versionDatabase();
        cAction.expectedResult = SharedConnectionsDialogFiller::Action::VERSION;
        actions << cAction;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLOSE);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});
}

GUI_TEST_CLASS_DEFINITION(cm_test_0008) {
    // Establish several connections
    // 1. File -> Shared databases.
    // Expected: The dialog appears.
    // 2. Choose the "ugene_gui_test" item and click "Connect" (or create it).
    // Expected: the connection is established:
    //           1) The connection item has the "Connected" icon.
    //           2) The database document appears in the project.
    //           3) There are no errors in the log.
    //           4) The "Edit" button is disabled.
    // 2. Choose the "ugene_gui_test_2" item and click "Connect" (or create it).
    // Expected: the connection is established:
    //           1) The connection item has the "Connected" icon.
    //           2) The database document appears in the project.
    //           3) There are no errors in the log.
    //           4) The "Edit" button is disabled.
    GTLogTracer lt;
    QString conName1 = "ugene_gui_test";
    QString conName2 = "ugene_gui_test_2";
    GTDatabaseConfig::initTestConnectionInfo(conName1);
    GTDatabaseConfig::initTestConnectionInfo(conName2, GTDatabaseConfig::secondDatabase());
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName1);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CONNECT, conName1);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) {
            checkConnectionItemIcon(os, "ugene_gui_test", ":/core/images/db/database_lightning.png");
            checkButtonStateForConnectionItem(os, "ugene_gui_test", "Edit", false);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    bool exists = GTUtilsProjectTreeView::checkItem(os, conName1, QModelIndex());
    CHECK_SET_ERR(exists, "A database connection not found in the project view");

    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName2);
        SharedConnectionsDialogFiller::Action cAction(SharedConnectionsDialogFiller::Action::CONNECT, conName2);
        cAction.dbName = GTDatabaseConfig::secondDatabase();
        actions << cAction;
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    class Scenario2 : public CustomScenario {
        void run(HI::GUITestOpStatus& os) {
            checkConnectionItemIcon(os, "ugene_gui_test_2", ":/core/images/db/database_lightning.png");
            checkButtonStateForConnectionItem(os, "ugene_gui_test_2", "Edit", false);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, new Scenario2));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    exists = GTUtilsProjectTreeView::checkItem(os, conName2, QModelIndex());
    CHECK_SET_ERR(exists, "A database connection not found in the project view");

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(cm_test_0009) {
    // 1. File -> Shared databases.
    // Expected: The dialog appears.
    // 2. Click "Add"
    // 3. Fill the parameters:
    //     Connection name: cm_test_0009: new shared database
    //     Host: ugene-quad-ubuntu
    //     Port: 3306
    //     Database: ugene_gui_test
    //     Login: wronglogin
    // 4. Click "OK".
    // Expected: the message about wrong connection data is shown.
    QString conName = "cm_test_0009: new shared database";
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::ADD);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        SharedConnectionsDialogFiller::Action cAction(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        cAction.expectedResult = SharedConnectionsDialogFiller::Action::WRONG_DATA;
        actions << cAction;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLOSE);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    {
        EditConnectionDialogFiller::Parameters params;
        params.connectionName = conName;
        params.host = GTDatabaseConfig::host();
        params.port = QString::number(GTDatabaseConfig::port());
        params.database = GTDatabaseConfig::database();
        params.login = "wrongLogin";
        params.password = "wrongPassword";
        GTUtilsDialog::waitForDialog(os, new EditConnectionDialogFiller(os, params, EditConnectionDialogFiller::MANUAL));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});
}

GUI_TEST_CLASS_DEFINITION(cm_test_0013) {
    // 1. File -> Shared databases.
    // Expected: The dialog appears.
    // 2. Click "Add"
    // 3. Fill the parameters:
    //     Connection name: cm_test_0013: new shared database
    //     Host: ugene-quad-ubuntu
    //     Port: 3306
    //     Database: ugene_gui_test
    // 4. Click "OK".
    // 5. Choose the new item and click "Connect".
    // Expected: the dialog about login and password appears.
    // 6. Fill the parameters:
    //     Login: ugene
    // 7. Click "OK".
    // Expected: the connection is established:
    //           1) The connection item has no icon.
    //           2) The database document appears in the project.
    //           3) There are no errors in the log.
    //           4) The "Edit" button is enabled.
    GTLogTracer lt;
    if (GTUtilsMdi::activeWindow(os, {false}) != nullptr) {
        GTUtilsMdi::click(os, GTGlobals::Close);
    }
    QString conName = "cm_test_0013: new shared database";
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::ADD);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, conName);
        SharedConnectionsDialogFiller::Action cAction(SharedConnectionsDialogFiller::Action::CONNECT, conName);
        cAction.expectedResult = SharedConnectionsDialogFiller::Action::LOGIN;
        actions << cAction;
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    {
        EditConnectionDialogFiller::Parameters params;
        params.connectionName = conName;
        params.host = GTDatabaseConfig::host();
        params.port = QString::number(GTDatabaseConfig::port());
        params.database = GTDatabaseConfig::database();
        GTUtilsDialog::waitForDialog(os, new EditConnectionDialogFiller(os, params, EditConnectionDialogFiller::MANUAL));
    }

    GTUtilsDialog::waitForDialog(os, new AuthenticationDialogFiller(os, GTDatabaseConfig::login(), GTDatabaseConfig::password()));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    class Scenario : public CustomScenario {
        void run(HI::GUITestOpStatus& os) {
            checkConnectionItemIcon(os, "cm_test_0013: new shared database", "");
            checkButtonStateForConnectionItem(os, "cm_test_0013: new shared database", "Edit", true);
            GTUtilsDialog::clickButtonBox(os, QDialogButtonBox::Close);
        }
    };

    GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, new Scenario));
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});

    const bool exists = GTUtilsProjectTreeView::checkItem(os, conName, QModelIndex());
    CHECK_SET_ERR(exists, "A database connection not found in the project view");

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(cm_test_0014) {
    // 1. File -> Shared databases.
    // Expected: The dialog appears, it contains a predefined connection to the public database
    // 2. Select the connection to the public database, click the "edit" button.
    // Expecteds state: all fields are disabled.
    {
        QList<SharedConnectionsDialogFiller::Action> actions;
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLICK, "UGENE public database");
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::EDIT);
        actions << SharedConnectionsDialogFiller::Action(SharedConnectionsDialogFiller::Action::CLOSE);
        GTUtilsDialog::waitForDialog(os, new SharedConnectionsDialogFiller(os, actions));
    }
    {
        class ReadOnlyCheckScenario : public CustomScenario {
            void run(HI::GUITestOpStatus& os) {
                QWidget* dialog = GTWidget::getActiveModalWidget(os);
                QWidget* leName = GTWidget::findWidget(os, "leName", dialog);
                QWidget* leHost = GTWidget::findWidget(os, "leHost", dialog);
                QWidget* lePort = GTWidget::findWidget(os, "lePort", dialog);
                QWidget* leDatabase = GTWidget::findWidget(os, "leDatabase", dialog);
                QWidget* leLogin = GTWidget::findWidget(os, "leLogin", dialog);
                QWidget* lePassword = GTWidget::findWidget(os, "lePassword", dialog);
                QWidget* cbRememberMe = GTWidget::findWidget(os, "cbRemember", dialog);

                CHECK_SET_ERR(nullptr != leName, "Connection name field is NULL");
                CHECK_SET_ERR(nullptr != leHost, "Host field is NULL");
                CHECK_SET_ERR(nullptr != lePort, "Port field is NULL");
                CHECK_SET_ERR(nullptr != leDatabase, "Database name field is NULL");
                CHECK_SET_ERR(nullptr != leLogin, "Login field is NULL");
                CHECK_SET_ERR(nullptr != lePassword, "Password field is NULL");
                CHECK_SET_ERR(nullptr != cbRememberMe, "Remember me checkbox is NULL");

                CHECK_SET_ERR(!leName->isEnabled(), "Connection name field is unexpectedly enabled");
                CHECK_SET_ERR(!leHost->isEnabled(), "Host field is unexpectedly enabled");
                CHECK_SET_ERR(!lePort->isEnabled(), "Port field is unexpectedly enabled");
                CHECK_SET_ERR(!leDatabase->isEnabled(), "Database name field is unexpectedly enabled");
                CHECK_SET_ERR(!leLogin->isEnabled(), "Login field is unexpectedly enabled");
                CHECK_SET_ERR(!lePassword->isEnabled(), "Password field is unexpectedly enabled");
                CHECK_SET_ERR(!cbRememberMe->isEnabled(), "Remember me checkbox is unexpectedly enabled");

                GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Cancel);
            }
        };

        GTUtilsDialog::waitForDialog(os, new EditConnectionDialogFiller(os, new ReadOnlyCheckScenario));
    }
    GTMenu::clickMainMenuItem(os, {"File", "Connect to UGENE shared database..."});
}

GUI_TEST_CLASS_DEFINITION(proj_test_0001) {
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Double click the folder "/proj_test_0001".
    // Expected: the folder is expanded; there are three subfolders: dir1, dir2, dir3.
    // 3. Double click the folder "/proj_test_0001/dir2".
    // Expected: the folder is expanded; there is one object: pt0001_human_T1.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    QModelIndex dirItem = GTUtilsProjectTreeView::findIndex(os, "proj_test_0001");
    GTUtilsProjectTreeView::checkItem(os, "pt0001_dir1", dirItem);
    GTUtilsProjectTreeView::checkItem(os, "pt0001_dir3", dirItem);

    QModelIndex dir2Item = GTUtilsProjectTreeView::findIndex(os, "pt0001_dir2", dirItem);
    GTUtilsProjectTreeView::checkItem(os, "pt0001_human_T1", dir2Item);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(proj_test_0002) {
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Right click the folder "/proj_test_0002" -> Create folder.
    // 3. Enter the name "pt0002_dir" and press Enter.
    // Expected: the subfolder folder is created.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(treeView != nullptr, "Invalid project tree view");

    QModelIndex dirItem = GTUtilsProjectTreeView::findIndex(os, "proj_test_0002");
    int childCount1 = treeView->model()->rowCount(dirItem);
    CHECK_SET_ERR(childCount1 == 0, "Invalid child item count in proj_test_0002. Expected 0, got " + QString::number(childCount1));

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__ADD_MENU << ACTION_PROJECT__CREATE_FOLDER));
    GTUtilsDialog::waitForDialog(os, new AddFolderDialogFiller(os, "pt0002_dir", GTGlobals::UseMouse));
    GTUtilsProjectTreeView::callContextMenu(os, dirItem);

    GTUtilsProjectTreeView::checkItem(os, "pt0002_dir", dirItem);
    int childCount2 = treeView->model()->rowCount(dirItem);
    CHECK_SET_ERR(childCount2 == 1, "Invalid child item count, expected 1, got " + QString::number(childCount2));
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(proj_test_0003) {
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Double click the folder "/proj_test_0003".
    // Expected: the folder is expanded; there is one subfolder.
    // 3. Right on the subfolder -> Rename.
    // 4. Enter the new name "new name" and press Enter.
    // Expected: the subfolder folder is renamed.
    // 5. Press F2.
    // Expected: it is proposed to rename the subfolder.
    // 6. Press Esc.
    // Expected: the subfolder folder is not renamed.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    QModelIndex dirItem = GTUtilsProjectTreeView::findIndex(os, "proj_test_0003");
    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    GTTreeView::checkItemIsExpanded(os, treeView, dirItem);

    QAbstractItemModel* model = treeView->model();
    CHECK_SET_ERR(model->rowCount(dirItem) == 1, "Invalid child item count");

    QModelIndex subfolderItem = model->index(0, 0, dirItem);

    GTUtilsProjectTreeView::rename(os, subfolderItem.data().toString(), "pt0003_new_name");
    dirItem = GTUtilsProjectTreeView::findIndex(os, "proj_test_0003");
    subfolderItem = model->index(0, 0, dirItem);
    QString subfolderName = subfolderItem.data().toString();
    CHECK_SET_ERR(subfolderName == "pt0003_new_name", "Renaming failed");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "pt0003_new_name"));
    GTMouseDriver::click();
    GTKeyboardDriver::keyClick(Qt::Key_F2);
    GTGlobals::sleep(2000);  // wait for some time.
    GTKeyboardDriver::keyClick(Qt::Key_Escape);

    subfolderItem = model->index(0, 0, dirItem);
    subfolderName = subfolderItem.data().toString();
    CHECK_SET_ERR(subfolderName == "pt0003_new_name", "The folder was renamed");

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(proj_test_0004) {
    // Object drag'n'drop
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Expand the folder "/proj_test_0004/pt0004_dir1/pt0004_dir2".
    // Expected: the folder is expanded; there is one object: pt0004_human_T1.
    // 3. Drag'n'drop the object to the folder "/proj_test_0004/not_existing_dir".
    // Expected: the object is moved; there are no errors in the log.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    QAbstractItemModel* model = treeView->model();

    QModelIndex dirItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0004_dir1");
    QModelIndex dirItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0004_dir2", dirItem1);
    QModelIndex objItem = GTUtilsProjectTreeView::findIndex(os, "pt0004_human_T1");
    GTTreeView::checkItemIsExpanded(os, treeView, dirItem2);
    GTUtilsProjectTreeView::dragAndDrop(os, objItem, dirItem1);

    dirItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0004_dir1");
    int dir1RowCount = model->rowCount(dirItem1);
    CHECK_SET_ERR(dir1RowCount == 2, QString("Invalid child item count for pt0004_dir1 Expected: 2; actual: %1").arg(dir1RowCount));

    dirItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0004_dir2");
    int dir2RowCount = model->rowCount(dirItem2);
    CHECK_SET_ERR(dir2RowCount == 0, QString("Invalid child item count for pt0004_dir2 Expected: 0; actual: %1").arg(dir2RowCount));

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(proj_test_0005) {
    // Objects drag'n'drop
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Expand the folder "/proj_test_0005/pt0005_dir1/pt0005_dir2".
    // Expected: the folder is expanded; there is one object: human_T1.
    // 3. Expand the folder "/proj_test_0005/pt0005_dir1/pt0005_dir3".
    // Expected: the folder is expanded; there is one object: COI.
    // 4. Select both objects using CTRL.
    // 5. Drag'n'drop the objects to the folder "/proj_test_0005/pt0005_dir1".
    // Expected: the objects are moved; there are no errors in the log.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");
    QAbstractItemModel* model = treeView->model();

    {  // drag'n'drop
        const QModelIndex dirItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0005_dir1");
        const QModelIndex dirItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0005_dir2");
        const QModelIndex dirItem3 = GTUtilsProjectTreeView::findIndex(os, "pt0005_dir3");
        const QModelIndex objItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0005_human_T1");
        const QModelIndex objItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0005_COI");

        GTTreeView::checkItemIsExpanded(os, treeView, dirItem2);
        GTTreeView::checkItemIsExpanded(os, treeView, dirItem3);

        GTUtilsProjectTreeView::dragAndDropSeveralElements(os, QModelIndexList() << objItem1 << objItem2, dirItem1);
    }

    const QModelIndex dirItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0005_dir1");
    const QModelIndex dirItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0005_dir2");
    const QModelIndex dirItem3 = GTUtilsProjectTreeView::findIndex(os, "pt0005_dir3");
    const QModelIndex objItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0005_human_T1");
    const QModelIndex objItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0005_COI");
    CHECK_SET_ERR(4 == model->rowCount(dirItem1), QString("Invalid child item count for pt0005_dir1 Expected: 4; actual: %1").arg(model->rowCount(dirItem1)));
    CHECK_SET_ERR(0 == model->rowCount(dirItem2), QString("Invalid child item count for pt0005_dir2 Expected: 0; actual: %1").arg(model->rowCount(dirItem2)));
    CHECK_SET_ERR(0 == model->rowCount(dirItem3), QString("Invalid child item count for pt0005_dir3 Expected: 0; actual: %1").arg(model->rowCount(dirItem3)));
    CHECK_SET_ERR(dirItem1 == dirItem2.parent(), "Invalid parent item 1");
    CHECK_SET_ERR(dirItem1 == dirItem3.parent(), "Invalid parent item 2");
    CHECK_SET_ERR(dirItem1 == objItem1.parent(), "Invalid parent item 3");
    CHECK_SET_ERR(dirItem1 == objItem2.parent(), "Invalid parent item 4");
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(proj_test_0006) {
    // Folder drag'n'drop
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Expand the folder "/proj_test_0006/pt0006_dir1/pt0006_dir2".
    // Expected: the folder is expanded; there is one object: pt0006_human_T1.
    // 3. Drag'n'drop the folder "pt0006_dir2" to the folder "/proj_test_0006/".
    // Expected: the folder is moved; there are no errors in the log; the structure is the following:
    //   proj_test_0006
    //     pt0006_dir1
    //     pt0006_dir2
    //       pt0006_human_T1
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );
    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");
    QAbstractItemModel* model = treeView->model();

    {  // drag'n'drop
        const QModelIndex dirItem = GTUtilsProjectTreeView::findIndex(os, "proj_test_0006");
        const QModelIndex dirItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0006_dir2");
        GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, dirItem2));
        GTMouseDriver::click();

        GTUtilsProjectTreeView::dragAndDrop(os, dirItem2, dirItem);
    }

    const QModelIndex dirItem = GTUtilsProjectTreeView::findIndex(os, "proj_test_0006");
    const QModelIndex dirItem1 = GTUtilsProjectTreeView::findIndex(os, "pt0006_dir1");
    const QModelIndex dirItem2 = GTUtilsProjectTreeView::findIndex(os, "pt0006_dir2");
    const QModelIndex objItem = GTUtilsProjectTreeView::findIndex(os, "pt0006_human_T1");
    CHECK_SET_ERR(2 == model->rowCount(dirItem), "Invalid child item count 1");
    CHECK_SET_ERR(0 == model->rowCount(dirItem1), "Invalid child item count 2");
    CHECK_SET_ERR(1 == model->rowCount(dirItem2), "Invalid child item count 3");
    CHECK_SET_ERR(dirItem == dirItem1.parent(), "Invalid parent item 1");
    CHECK_SET_ERR(dirItem == dirItem2.parent(), "Invalid parent item 2");
    CHECK_SET_ERR(dirItem2 == objItem.parent(), "Invalid parent item 3");

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(proj_test_0007) {
    // Filter
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Fill the object name filter: "pt0007".
    // Expected: there are only two objects in the project (pt0007_human_T1 and pt0007_COI).
    // 3. Clear the filter.
    // Expected: the project has usual structure with folders.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );

    auto filterEdit = GTWidget::findLineEdit(os, "nameFilterEdit");
    GTLineEdit::setText(os, filterEdit, "pt0007");

    GTUtilsTaskTreeView::waitTaskFinished(os);

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");
    QAbstractItemModel* model = treeView->model();

    GTGlobals::sleep();

    const QModelIndex invisibleIndex = QModelIndex();
    int objCount1 = model->rowCount(invisibleIndex);
    CHECK_SET_ERR(1 == objCount1, QString("Invalid filtered objects count. Expected 1. Actual: %1").arg(objCount1));

    const QModelIndex index1st = model->index(0, 0, invisibleIndex);
    const QString filterGroupName = index1st.data().toString();
    CHECK_SET_ERR(filterGroupName == "Object name", QString("Wrong filter group name. Expected: %1; actual: %2").arg("Object name", filterGroupName));
    int objCount2 = model->rowCount(index1st);
    CHECK_SET_ERR(2 == objCount2, QString("Invalid filtered objects count. Expected 2. Actual: %1").arg(objCount2));

    QString objName1 = model->index(0, 0, index1st).data().toString();
    QString objName2 = model->index(1, 0, index1st).data().toString();
    CHECK_SET_ERR(objName1.contains("pt0007") && objName1.contains("_COI"), "Wrong object name 1");
    CHECK_SET_ERR(objName2.contains("pt0007") && objName2.contains("_human_T1"), "Wrong object name 2");

    GTLineEdit::setText(os, filterEdit, "");

    model = treeView->model();
    int docCount = model->rowCount(QModelIndex());
    CHECK_SET_ERR(1 == docCount, "Invalid filtered docs count");
    QString docName = model->index(0, 0, QModelIndex()).data().toString();
    CHECK_SET_ERR("ugene_gui_test" == docName, "Wrong doc name");
}

GUI_TEST_CLASS_DEFINITION(proj_test_0008) {
    //    Folders creation with names in different register.

    //    1. Connect to the "ugene_gui_test" database.

    //    2. Create folder "proj_test_0008" in the root folder;

    //    3. Create folder "abcdefgh" in the "/proj_test_0004/" folder;

    //    4. Create folder "ABCDEFGH" in the "/proj_test_0004/" folder;

    //    5. Create folder "AbCdEfGh" in the "/proj_test_0004/" folder;

    //    6. Wait until project updater refreshes the document (something about 10 second).
    //    Expected state: there tree folders in the "/proj_test_0004/" folder.

    GTLogTracer lt;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    QString testFolder = GTUtilsSharedDatabaseDocument::genTestFolderName("proj_test_0008");
    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, "/", testFolder);
    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, "/" + testFolder, "abcdefgh");
    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, "/" + testFolder, "ABCDEFGH");
    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, "/" + testFolder, "AbCdEfGh");

    GTGlobals::sleep(10000);

    const QStringList expectedItems = QStringList() << "/" + testFolder + "/abcdefgh"
                                                    << "/" + testFolder + "/ABCDEFGH"
                                                    << "/" + testFolder + "/AbCdEfGh";
    GTUtilsSharedDatabaseDocument::checkThereAreNoItemsExceptListed(os, databaseDoc, "/" + testFolder, expectedItems);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(proj_test_0009) {
    // check objects order
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    const QModelIndex et0002_features = GTUtilsProjectTreeView::findIndex(os, "et0002_features");
    const QModelIndex et0004_assembly = GTUtilsProjectTreeView::findIndex(os, "et0004_assembly");
    const QModelIndex et0003_alignment = GTUtilsProjectTreeView::findIndex(os, "et0003_alignment");
    const QModelIndex et0001_sequence = GTUtilsProjectTreeView::findIndex(os, "et0001_sequence");
    const QModelIndex et0005_variations = GTUtilsProjectTreeView::findIndex(os, "et0005_variations");

    CHECK_SET_ERR(et0001_sequence.row() < et0002_features.row(), "Unexpected objects order in project");
    CHECK_SET_ERR(et0002_features.row() < et0003_alignment.row(), "Unexpected objects order in project");
    CHECK_SET_ERR(et0003_alignment.row() < et0004_assembly.row(), "Unexpected objects order in project");
    CHECK_SET_ERR(et0004_assembly.row() < et0005_variations.row(), "Unexpected objects order in project");
}

GUI_TEST_CLASS_DEFINITION(import_test_0001) {
    //    Object drag'n'drop
    //    1. Connect to the "ugene_gui_test" database.
    //    2. Create the folder "/import_test_0001".
    //    3. Open "samples/FASTA/human_T1.fa".
    //    4. Drag'n'drop the sequence object to the folder "/import_test_0001".
    //    Expected: the object is imported.
    //    5. Double click the new object.
    //    Expected: the sequence view is opened.

    GTLogTracer lt;

    QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    QString newFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0001");
    QString newFolderPath = parentFolderPath + U2ObjectDbi::PATH_SEP + newFolderName;
    QString fileDocName = "human_T1.fa";
    QString fileObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    QString importedObjectPath = newFolderPath + U2ObjectDbi::PATH_SEP + fileObjectName;

    GTFileDialog::openFile(os, dataDir + "/samples/FASTA/", "human_T1.fa");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, parentFolderPath, newFolderName);
    QModelIndex newFolderItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, newFolderPath);

    QModelIndex fileDocIndex = GTUtilsProjectTreeView::findIndex(os, QStringList() << fileDocName);
    QModelIndex objectDocIndex = GTUtilsProjectTreeView::findIndex(os, fileObjectName, fileDocIndex);

    GTUtilsProjectTreeView::dragAndDrop(os, objectDocIndex, newFolderItemIndex);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, importedObjectPath);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0002) {
    //    Document drag'n'drop
    //    1. Open "samples/FASTA/human_T1.fa".
    //    2. Connect to the "ugene_gui_test" database.
    //    3. Create the folder "/import_test_0002".
    //    4. Drag'n'drop the document to the folder "/import_test_0002".
    //    Expected: the document is imported.
    //    5. Double click the new object.
    //    Expected: the sequence view is opened.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString newFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0002");
    const QString newFolderPath = parentFolderPath + newFolderName;
    const QString fileDocName = "human_T1.fa";
    const QString fileObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString fileObjectNameWidget = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString importedDocFolderPath = newFolderPath + U2ObjectDbi::PATH_SEP + fileDocName;
    const QString importedObjectPath = importedDocFolderPath + U2ObjectDbi::PATH_SEP + fileObjectName;

    GTFileDialog::openFile(os, dataDir + "/samples/FASTA/", "human_T1.fa");
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, parentFolderPath, newFolderName);
    QModelIndex newFolderItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, newFolderPath);
    QModelIndex fileDocIndex = GTUtilsProjectTreeView::findIndex(os, QStringList() << fileDocName);

    GTUtilsProjectTreeView::dragAndDrop(os, fileDocIndex, newFolderItemIndex);

    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, importedDocFolderPath);
    QModelIndex importedObjectItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, importedObjectPath);

    GTUtilsMdi::closeActiveWindow(os);
    GTUtilsSequenceView::checkNoSequenceViewWindowIsOpened(os);

    GTUtilsProjectTreeView::doubleClickItem(os, importedObjectItemIndex);
    GTUtilsMdi::checkWindowIsActive(os, fileObjectNameWidget);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0003) {
    //    Annotations drag'n'drop
    //    1. Connect to the "ugene_gui_test" database.
    //    2. Open "samples/Genbank/murine.gb".
    //    3. Import sequence and annotations objects into a new folder in the database.
    //    4. Open a sequence for the imported sequence.
    //    5. Drag'n'drop the imported annotation object to the sequence view and connect it with the sequence.
    //    Expected: the annotations are added to the sequence view.
    //    6. Reconnect to the database.
    //    7. Double click the sequence object "/imp_test_0003/NC_001363" in the database.
    //    Expected: the sequence view is opened; annotations are shown too.

    GTLogTracer lt;

    GTFileDialog::openFile(os, dataDir + "/samples/Genbank/", "murine.gb");
    GTUtilsTaskTreeView::waitTaskFinished(os);
    GTUtilsMdi::closeActiveWindow(os);

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    // Create a new folder
    QString folderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0003");
    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, U2ObjectDbi::ROOT_FOLDER, folderName);

    QString folderPath = U2ObjectDbi::ROOT_FOLDER + U2ObjectDbi::PATH_SEP + folderName;
    QModelIndex folderItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, folderPath);

    // Import sequence and annotation objects into the DB.
    QModelIndex fileDocIndex = GTUtilsProjectTreeView::findIndex(os, QStringList() << "murine.gb");

    QModelIndex fileSequenceObjectIndex = GTUtilsProjectTreeView::findIndex(os, "NC_001363", fileDocIndex);
    GTUtilsProjectTreeView::dragAndDrop(os, fileSequenceObjectIndex, folderItemIndex);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QModelIndex fileAnnotationObjectIndex = GTUtilsProjectTreeView::findIndex(os, "NC_001363 features", fileDocIndex);
    GTUtilsProjectTreeView::dragAndDrop(os, fileAnnotationObjectIndex, folderItemIndex);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    // Open a sequence view for the imported sequence.
    GTUtilsSequenceView::checkNoSequenceViewWindowIsOpened(os);
    QModelIndex databaseSequenceObjectItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, folderPath + U2ObjectDbi::PATH_SEP + "NC_001363");
    GTUtilsProjectTreeView::doubleClickItem(os, databaseSequenceObjectItemIndex);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Link the annotations with the sequence object.
    QModelIndex databaseAnnotationObjectItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, folderPath + U2ObjectDbi::PATH_SEP + "NC_001363 features");
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsDialog::waitForDialog(os, new CreateObjectRelationDialogFiller(os));
    QTreeWidget* annotationTableWidget = GTUtilsAnnotationsTreeView::getTreeWidget(os);
    GTUtilsProjectTreeView::dragAndDrop(os, databaseAnnotationObjectItemIndex, annotationTableWidget);

    // Disconnect and connect to the database again.
    GTUtilsSharedDatabaseDocument::disconnectDatabase(os, databaseDoc);
    GTUtilsSequenceView::checkNoSequenceViewWindowIsOpened(os);

    databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os, false);  // false -> Do not remove TMP folders during connect.
    databaseSequenceObjectItemIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, folderPath + U2ObjectDbi::PATH_SEP + "NC_001363");

    // Open sequence object from the DB.
    GTUtilsProjectTreeView::doubleClickItem(os, databaseSequenceObjectItemIndex);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    // Check that it has annotations in the view.
    annotationTableWidget = GTUtilsAnnotationsTreeView::getTreeWidget(os);
    GTUtilsAnnotationsTreeView::findItem(os, QString("NC_001363 features [%1]").arg(databaseDoc->getName()));

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0004) {
    //    Import a file via the import dialog, destination path is changed in the dialog

    //    1. Connect to the "ugene_gui_test" database.

    //    2. Create a folder in the database: {import_test_0004}.

    //    3. Call context menu on the database connection document, select {Add -> Import to the database...} item.
    //    Expected state: an import dialog appears.

    //    4. Click the "Add files" button, select {data/samples/FASTA/human_T1.fa}.
    //    Expected state: the file is added to the orders list, it will be imported into the {/} folder.

    //    5. Click twice to the item on the destination folder column and change the folder, set {/import_test_0004}.
    //    Expected state: the file is present in the orders list, it will be imported into the {/import_test_0004} folder.

    //    6. Click the "Import" button.
    //    Expected state: an import task is started, after it finishes a sequence object appears in the {/import_test_0004} folder.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0004");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString resultFolderName = "human_T1";
    const QString resultFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + resultFolderName;
    const QString filePath = QFileInfo(dataDir + "samples/FASTA/human_T1.fa").absoluteFilePath();
    const QString sequenceObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString databaseSequenceObjectPath = resultFolderPath + U2ObjectDbi::PATH_SEP + sequenceObjectName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, parentFolderPath, dstFolderName);

    QList<ImportToDatabaseDialogFiller::Action> actions;

    QVariantMap addFilesAction;
    addFilesAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, QStringList() << filePath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_FILES, addFilesAction);

    QVariantMap changeDestinationAction;
    changeDestinationAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__ITEM, filePath);
    changeDestinationAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__DESTINATION_FOLDER, dstFolderPath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::EDIT_DESTINATION_FOLDER, changeDestinationAction);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());
    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));
    GTUtilsSharedDatabaseDocument::callImportDialog(os, databaseDoc, parentFolderPath);
    GTGlobals::sleep(30000);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, databaseSequenceObjectPath);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0005) {
    //    Import a file via the import dialog, file is imported to the inner folder

    //    1. Connect to the "ugene_gui_test" database.

    //    2. Call context menu on the {import_test_0005} folder in the database connection document, select {Add -> Import to the folder...} item.
    //    Expected state: an import dialog appears.

    //    3. Click the "Add files" button, select {data/samples/FASTA/human_T1.fa}.
    //    Expected state: the file is added to the orders list, it will be imported into the {/import_test_0005} folder.

    //    4. Click the "Import" button.
    //    Expected state: an import task is started, after it finishes a sequence object appears in the {/import_test_0005} folder.

    GTLogTracer lt;

    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0005");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString filePath = QFileInfo(dataDir + "samples/FASTA/human_T1.fa").absoluteFilePath();
    const QString sequenceObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString databaseSequenceObjectPath = dstFolderPath + U2ObjectDbi::PATH_SEP + "human_T1" + U2ObjectDbi::PATH_SEP + sequenceObjectName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, U2ObjectDbi::ROOT_FOLDER, dstFolderName);

    QList<ImportToDatabaseDialogFiller::Action> actions;
    QVariantMap addFilesAction;
    addFilesAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, QStringList() << filePath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_FILES, addFilesAction);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());
    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));
    GTUtilsSharedDatabaseDocument::callImportDialog(os, databaseDoc, dstFolderPath);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, databaseSequenceObjectPath);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0006) {
    // Import a folder via import dialog, the folder is imported non-recursively, a subfolder for the imported folder is not created.

    // 1. Connect to the "ugene_gui_test" database.

    // 2. Call context menu on the {import_test_0006} folder in the database connection document, select {Add -> Import to the folder...} item.
    // Expected state: an import dialog appears.

    // 3. Click the "Add folder" button, select {_common_data/scenarios/shared_database/import/first}.
    // Expected state: the folder is added to the orders list, it will be imported into the {/import_test_0006} folder.

    // 4. Click the "General options" button.
    // Expected state: the options dialog appears.

    // 5. Fill the dialog:
    //{Process folders recursuvely} : false;
    //{Create a subfolder for the top level folder} : false;
    // click the "Ok" button.

    // 6. Click the "Import" button.
    // Expected state: an import task is started, after it finishes a sequence object "SEQUENCE_WITH_A_ENTRY" appears in the {/import_test_0006/seq1} folder.

    GTLogTracer lt;

    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0006");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString resultFolderName = "seq1";
    const QString resultFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + resultFolderName;
    const QString folderPath = testDir + "_common_data/scenarios/shared_database/import/first/";
    const QString notImportedFolderName = "second";
    const QString notImportedObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString sequenceObjectName = "SEQUENCE_WITH_A_ENTRY";
    const QString databaseSequenceObjectPath = resultFolderPath + U2ObjectDbi::PATH_SEP + sequenceObjectName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, U2ObjectDbi::ROOT_FOLDER, dstFolderName);

    QList<ImportToDatabaseDialogFiller::Action> actions;
    QVariantMap addFolderAction;
    addFolderAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, QStringList() << folderPath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_DIRS, addFolderAction);
    QVariantMap editOptionsAction;
    editOptionsAction.insert(ImportToDatabaseOptions::PROCESS_FOLDERS_RECUSIVELY, false);
    editOptionsAction.insert(ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_TOP_LEVEL_FOLDER, false);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::EDIT_GENERAL_OPTIONS, editOptionsAction);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());
    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));
    GTUtilsSharedDatabaseDocument::callImportDialog(os, databaseDoc, dstFolderPath);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, databaseSequenceObjectPath);

    GTGlobals::FindOptions options(false);
    QModelIndex dstFolderIndex = GTUtilsSharedDatabaseDocument::getItemIndex(os, databaseDoc, dstFolderPath);
    QModelIndex notImportedFolderIndexIndex = GTUtilsProjectTreeView::findIndex(os, notImportedFolderName, dstFolderIndex, options);
    CHECK_SET_ERR(!notImportedFolderIndexIndex.isValid(), "The inner folder is unexpectedly imported");

    QModelIndex notImportedObjectIndexIndex = GTUtilsProjectTreeView::findIndex(os, notImportedObjectName, dstFolderIndex, options);
    CHECK_SET_ERR(!notImportedObjectIndexIndex.isValid(), "The object from the inner folder is unexpectedly imported");

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0007) {
    // Import a folder via import dialog, the folder is imported non-recursively, a subfolder for the imported folder is created.

    // 1. Connect to the "ugene_gui_test" database.

    // 2. Call context menu on the {import_test_0007} folder in the database connection document, select {Add -> Import to the folder...} item.
    // Expected state: an import dialog appears.

    // 3. Click the "Add folder" button, select {_common_data/scenarios/shared_database/import/first/second}.
    // Expected state: the folder is added to the orders list, it will be imported into the {/import_test_0007} folder.

    // 4. Click the "General options" button.
    // Expected state: the options dialog appears.

    // 5. Fill the dialog:
    //{Process folders recursuvely} : false;
    //{Create a subfolder for the top level folder} : true
    // click the "Ok" button.

    // 6. Click the "Import" button.
    // Expected state: an import task is started, after it finishes a sequence object "human_T1 (UCSC April 2002 chr7:115977709-117855134)" appears in the {/import_test_0007/second/human_T1_cutted} folder.

    GTLogTracer lt;

    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0007");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString importedTopLevelFolderName = "second";
    const QString importedTopLevelFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + importedTopLevelFolderName;
    const QString resultFolderName = "human_T1_cutted";
    const QString resultFolderPath = importedTopLevelFolderPath + U2ObjectDbi::ROOT_FOLDER + resultFolderName;
    const QString folderPath = testDir + "_common_data/scenarios/shared_database/import/first/second/";
    const QString sequenceObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString databaseSequenceObjectPath = resultFolderPath + U2ObjectDbi::PATH_SEP + sequenceObjectName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, U2ObjectDbi::ROOT_FOLDER, dstFolderName);

    QList<ImportToDatabaseDialogFiller::Action> actions;

    QVariantMap addFolderAction;
    addFolderAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, QStringList() << folderPath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_DIRS, addFolderAction);
    QVariantMap editOptionsAction;
    editOptionsAction.insert(ImportToDatabaseOptions::PROCESS_FOLDERS_RECUSIVELY, false);
    editOptionsAction.insert(ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_TOP_LEVEL_FOLDER, true);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::EDIT_GENERAL_OPTIONS, editOptionsAction);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());
    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));
    GTUtilsSharedDatabaseDocument::callImportDialog(os, databaseDoc, dstFolderPath);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, databaseSequenceObjectPath);

    const QStringList expectedItems = QStringList() << importedTopLevelFolderPath
                                                    << resultFolderPath
                                                    << databaseSequenceObjectPath;
    GTUtilsSharedDatabaseDocument::checkThereAreNoItemsExceptListed(os, databaseDoc, dstFolderPath, expectedItems);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0008) {
    // Import a folder via import dialog, the folder is imported recursively, a subfolder for the imported folder is not created, folders structure is kept.

    // 1. Connect to the "ugene_gui_test" database.

    // 2. Call context menu on the {import_test_0008} folder in the database connection document, select {Add -> Import to the folder...} item.
    // Expected state: an import dialog appears.

    // 3. Click the "Add folder" button, select {_common_data/scenarios/shared_database/import/first}.
    // Expected state: the folder is added to the orders list, it will be imported into the {/import_test_0008} folder.

    // 4. Click the "General options" button.
    // Expected state: the options dialog appears.

    // 5. Fill the dialog:
    //{Process folders recursuvely} : true;
    //{Create a subfolder for the top level folder} : false
    //{Keep folders structure} : true;
    // click the "Ok" button.

    // 6. Click the "Import" button.
    // Expected state: an import task is started, there are two sequence objects: "human_T1 (UCSC April 2002 chr7:115977709-117855134)" in the {/import_test_0008/second/human_T1_cutted} folder and "SEQUENCE_WITH_A_ENTRY" in the {/import_test_0008/seq1} folder after the task finishes.

    GTLogTracer lt;

    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0008");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString innerFolderName = "second";
    const QString resultFirstFolderName = "seq1";
    const QString resultSecondFolderName = "human_T1_cutted";
    const QString resultFirstFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + resultFirstFolderName;
    const QString resultSecondFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + innerFolderName + U2ObjectDbi::ROOT_FOLDER + resultSecondFolderName;
    const QString folderPath = testDir + "_common_data/scenarios/shared_database/import/first/";
    const QString sequenceFirstObjectName = "SEQUENCE_WITH_A_ENTRY";
    const QString sequenceSecondObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString databaseSequenceFirstObjectPath = resultFirstFolderPath + U2ObjectDbi::PATH_SEP + sequenceFirstObjectName;
    const QString databaseSequenceSecondObjectPath = resultSecondFolderPath + U2ObjectDbi::PATH_SEP + sequenceSecondObjectName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, U2ObjectDbi::ROOT_FOLDER, dstFolderName);

    QList<ImportToDatabaseDialogFiller::Action> actions;

    QVariantMap addFolderAction;
    addFolderAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, QStringList() << folderPath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_DIRS, addFolderAction);

    QVariantMap editOptionsAction;
    editOptionsAction.insert(ImportToDatabaseOptions::PROCESS_FOLDERS_RECUSIVELY, true);
    editOptionsAction.insert(ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_TOP_LEVEL_FOLDER, false);
    editOptionsAction.insert(ImportToDatabaseOptions::KEEP_FOLDERS_STRUCTURE, true);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::EDIT_GENERAL_OPTIONS, editOptionsAction);

    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());

    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));

    GTUtilsSharedDatabaseDocument::callImportDialog(os, databaseDoc, dstFolderPath);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, databaseSequenceFirstObjectPath);
    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, databaseSequenceSecondObjectPath);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0009) {
    // Import a folder via import dialog, the folder is imported recursively, a subfolder for the imported folder is not created, folders structure is not kept.

    // 1. Connect to the "ugene_gui_test" database.

    // 2. Call context menu on the {import_test_0009} folder in the database connection document, select {Add -> Import to the folder...} item.
    // Expected state: an import dialog appears.

    // 3. Click the "Add folder" button, select {_common_data/scenarios/shared_database/import/first}.
    // Expected state: the folder is added to the orders list, it will be imported into the {/import_test_0009} folder.

    // 4. Click the "General options" button.
    // Expected state: the options dialog appears.

    // 5. Fill the dialog:
    //{Process folders recursuvely} : true;
    //{Create a subfolder for the top level folder} : false
    //{Keep folders structure} : false;
    // click the "Ok" button.

    // 6. Click the "Import" button.
    // Expected state: an import task is started, there are two sequence objects: "human_T1 (UCSC April 2002 chr7:115977709-117855134)" in the {/import_test_0009/human_T1_cutted} folder and "SEQUENCE_WITH_A_ENTRY" in the {/import_test_0009/seq1} folder after the task finishes.

    GTLogTracer lt;

    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0009");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString resultFirstFolderName = "seq1";
    const QString resultSecondFolderName = "human_T1_cutted";
    const QString resultFirstFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + resultFirstFolderName;
    const QString resultSecondFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + resultSecondFolderName;
    const QString folderPath = testDir + "_common_data/scenarios/shared_database/import/first/";
    const QString sequenceFirstObjectName = "SEQUENCE_WITH_A_ENTRY";
    const QString sequenceSecondObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString databaseSequenceFirstObjectPath = resultFirstFolderPath + U2ObjectDbi::PATH_SEP + sequenceFirstObjectName;
    const QString databaseSequenceSecondObjectPath = resultSecondFolderPath + U2ObjectDbi::PATH_SEP + sequenceSecondObjectName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, U2ObjectDbi::ROOT_FOLDER, dstFolderName);

    QList<ImportToDatabaseDialogFiller::Action> actions;

    QVariantMap addFolderAction;
    addFolderAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, QStringList() << folderPath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_DIRS, addFolderAction);

    QVariantMap editOptionsAction;
    editOptionsAction.insert(ImportToDatabaseOptions::PROCESS_FOLDERS_RECUSIVELY, true);
    editOptionsAction.insert(ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_TOP_LEVEL_FOLDER, false);
    editOptionsAction.insert(ImportToDatabaseOptions::KEEP_FOLDERS_STRUCTURE, false);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::EDIT_GENERAL_OPTIONS, editOptionsAction);

    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());

    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));

    GTUtilsSharedDatabaseDocument::callImportDialog(os, databaseDoc, dstFolderPath);

    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, databaseSequenceFirstObjectPath);
    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, databaseSequenceSecondObjectPath);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0010) {
    // Import an object via import dialog.

    // 1. Open file {data/samples/FASTA/human_T1.fa}.

    // 2. Connect to the "ugene_gui_test" database.

    // 3. Call context menu on the {import_test_0010} folder in the database connection document, select {Add -> Import to the folder...} item.
    // Expected state: an import dialog appears.

    // 4. Click the "Add objects" button, select the "human_T1 (UCSC April 2002 chr7:115977709-117855134)" object in the "human_T1.fa" document.
    // Expected state: the object is added to the orders list, it will be imported into the {/import_test_0010} folder.

    // 5. Click the "Import" button.
    // Expected state: an import task is started, there is a sequence object "human_T1 (UCSC April 2002 chr7:115977709-117855134)" in the {/import_test_0010} folder after the task has finished.

    GTLogTracer lt;

    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0010");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString documentName = "human_T1.fa";
    const QString sequenceObjectName = "human_T1 (UCSC April 2002 chr7:115977709-117855134)";
    const QString databaseSequenceObjectPath = dstFolderPath + U2ObjectDbi::PATH_SEP + sequenceObjectName;

    GTFileDialog::openFile(os, dataDir + "samples/FASTA", documentName);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, U2ObjectDbi::ROOT_FOLDER, dstFolderName);

    QList<ImportToDatabaseDialogFiller::Action> actions;
    QVariantMap addObjectAction;
    QMap<QString, QVariant> projectItemsToSelect;
    projectItemsToSelect.insert(documentName, QStringList() << sequenceObjectName);
    addObjectAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PROJECT_ITEMS_LIST, projectItemsToSelect);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_PROJECT_ITEMS, addObjectAction);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());
    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));
    GTUtilsSharedDatabaseDocument::callImportDialog(os, databaseDoc, dstFolderPath);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, databaseSequenceObjectPath);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0011) {
    //    Import a document via import dialog.

    //    1. Open file {data/samples/Genbank/murine.gb}.

    //    2. Connect to the "ugene_gui_test" database.

    //    3. Call context menu on the {import_test_0011} folder in the database connection document, select {Add -> Import to the folder...} item.
    //    Expected state: an import dialog appears.

    //    4. Click the "Add objects" button, select the "murine.gb" document.
    //    Expected state: the document is added to the orders list, it will be imported into the {/import_test_0011} folder.

    //    5. Click the "Import" button.
    //    Expected state: an import task is started, there are two objects in the {/import_test_0010/murine} folder after the task has finished: a sequence "NC_001363" and an annotation table "NC_001363 features".

    //    6. Doubleclick the imported sequence object.
    //    Expected state: a sequence view is opened, the annotation table object is attached.

    GTLogTracer lt;

    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0011");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString documentName = "murine.gb";
    const QString sequenceObjectName = "NC_001363";
    const QString AnnotationTableObjectName = "NC_001363 features";
    const QString sequenceVisibleWidgetName = "" + sequenceObjectName;
    const QString someFeatureName = "CDS";
    const QString databaseSequenceObjectPath = dstFolderPath + U2ObjectDbi::PATH_SEP + documentName + U2ObjectDbi::PATH_SEP + sequenceObjectName;
    const QString databaseAnnotationTableObjectPath = dstFolderPath + U2ObjectDbi::PATH_SEP + documentName + U2ObjectDbi::PATH_SEP + AnnotationTableObjectName;

    GTFileDialog::openFile(os, dataDir + "samples/Genbank", documentName);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);
    GTUtilsMdi::closeActiveWindow(os);

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, U2ObjectDbi::ROOT_FOLDER, dstFolderName);

    QList<ImportToDatabaseDialogFiller::Action> actions;
    QVariantMap addDocumentAction;
    QMap<QString, QVariant> projectItemsToSelect;
    projectItemsToSelect.insert(documentName, QStringList());
    addDocumentAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PROJECT_ITEMS_LIST, projectItemsToSelect);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_PROJECT_ITEMS, addDocumentAction);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());
    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));
    GTUtilsSharedDatabaseDocument::callImportDialog(os, databaseDoc, dstFolderPath);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, databaseSequenceObjectPath);
    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, databaseAnnotationTableObjectPath);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseSequenceObjectPath);
    GTWidget::findWidget(os, sequenceVisibleWidgetName);

    GTUtilsAnnotationsTreeView::findItem(os, someFeatureName);
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0012) {
    //    Import a multi-sequence file via import dialog as msa.

    //    1. Connect to the "ugene_gui_test" database.

    //    2. Call context menu on the {import_test_0012} folder in the database connection document, select {Add -> Import to the folder...} item.
    //    Expected state: an import dialog appears.

    //    3. Click the "Add files" button, select the {_common_data/fasta/multy_fa.fa} file.
    //    Expected state: the document is added to the orders list, it will be imported into the {/import_test_0012} folder.

    //    4. Click the "General options" button.
    //    Expected state: an options dialog appears.

    //    5. Set options:
    //    {Join into alignment} : checked
    //    and click the "Ok" button.

    //    6. Click the "Import" button.
    //    Expected state: an import task is started, there is a msa object in the {/import_test_0012/multy_fa.fa} folder after the task has finished.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0012");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString objectFolderName = "multy_fa";
    const QString objectFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + objectFolderName;
    const QString malignmentObjectName = "Multiple alignment";
    const QString databaseMalignmentObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + malignmentObjectName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    QVariantMap options;
    options.insert(ImportToDatabaseOptions::MULTI_SEQUENCE_POLICY, ImportToDatabaseOptions::MALIGNMENT);
    GTUtilsSharedDatabaseDocument::importFiles(os, databaseDoc, dstFolderPath, QStringList() << testDir + "_common_data/fasta/multy_fa.fa", options);

    const QStringList expectedItems = QStringList() << objectFolderPath
                                                    << databaseMalignmentObjectPath;
    GTUtilsSharedDatabaseDocument::checkThereAreNoItemsExceptListed(os, databaseDoc, dstFolderPath, expectedItems);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0013) {
    //    Import a multi-sequence file via import dialog and merge them into a single sequence.

    //    1. Connect to the "ugene_gui_test" database.

    //    2. Call context menu on the {import_test_0013} folder in the database connection document, select {Add -> Import to the folder...} item.
    //    Expected state: an import dialog appears.

    //    3. Click the "Add files" button, select the {_common_data/fasta/multy_fa.fa} file.
    //    Expected state: the document is added to the orders list, it will be imported into the {/import_test_0013} folder.

    //    4. Click the "General options" button.
    //    Expected state: an options dialog appears.

    //    5. Set options:
    //    {Merge into a single sequence} : checked
    //    {Number of 'unknown' symbols} : 5
    //    and click the "Ok" button.

    //    6. Click the "Import" button.
    //    Expected state: an import task is started, there are a sequence object and an annotation table object in the {/import_test_0013/multy_fa.fa} folder after the task has finished.

    //    7. Double click the sequence object.
    //    Expected state: a sequence view opens, there is the annotation table object in the annotation tree widget.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0013");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString objectFolderName = "multy_fa";
    const QString objectFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + objectFolderName;
    const QString sequenceObjectName = "SEQUENCE_1";
    const QString annotationTableObjectName = "Contigs";
    const QString sequenceVisibleWidgetName = "[s] " + sequenceObjectName;
    const QString databaseAnnotationTableObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + annotationTableObjectName;
    const QString databaseSequenceObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + sequenceObjectName;
    const QString contigFeatureName = "contig  (0, 2)";
    const QString expectedSecondContigRegion = "243..362";

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    QVariantMap options;
    options.insert(ImportToDatabaseOptions::MULTI_SEQUENCE_POLICY, ImportToDatabaseOptions::MERGE);
    options.insert(ImportToDatabaseOptions::MERGE_MULTI_SEQUENCE_POLICY_SEPARATOR_SIZE, 5);
    GTUtilsSharedDatabaseDocument::importFiles(os, databaseDoc, dstFolderPath, QStringList() << testDir + "_common_data/fasta/multy_fa.fa", options);

    const QStringList expectedItems = QStringList() << objectFolderPath
                                                    << databaseSequenceObjectPath
                                                    << databaseAnnotationTableObjectPath;
    GTUtilsSharedDatabaseDocument::checkThereAreNoItemsExceptListed(os, databaseDoc, dstFolderPath, expectedItems);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseSequenceObjectPath);
    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsAnnotationsTreeView::getTreeWidget(os);

    QTreeWidgetItem* contigGroup = GTUtilsAnnotationsTreeView::findItem(os, contigFeatureName);

    QTreeWidgetItem* secondContig = contigGroup->child(1);
    CHECK_SET_ERR(contigGroup != nullptr, "Second contig annotation is NULL");

    QString secondContigRegion = secondContig->text(AnnotationsTreeView::COLUMN_VALUE);
    CHECK_SET_ERR(expectedSecondContigRegion == secondContigRegion, QString("Invalid contig region: expected %1, got %2").arg(expectedSecondContigRegion).arg(secondContigRegion));

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0014) {
    //    Import a multi-sequence file via import dialog as separate sequences.

    //    1. Connect to the "ugene_gui_test" database.

    //    2. Call context menu on the {import_test_0014} folder in the database connection document, select {Add -> Import to the folder...} item.
    //    Expected state: an import dialog appears.

    //    3. Click the "Add files" button, select the {_common_data/fasta/multy_fa.fa} file.
    //    Expected state: the document is added to the orders list, it will be imported into the {/import_test_0014} folder.

    //    4. Click the "Import" button.
    //    Expected state: an import task is started, there are two sequence objects in the {/import_test_0014/multy_fa.fa} folder after the task has finished.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0014");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString objectFolderName = "multy_fa";
    const QString objectFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + objectFolderName;
    const QString sequenceFirstObjectName = "SEQUENCE_1";
    const QString sequenceSecondObjectName = "SEQUENCE_2";
    const QString databaseSequenceFirstObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + sequenceFirstObjectName;
    const QString databaseSequenceSecondObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + sequenceSecondObjectName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::importFiles(os, databaseDoc, dstFolderPath, QStringList() << testDir + "_common_data/fasta/multy_fa.fa");

    const QStringList expectedItems = QStringList() << objectFolderPath
                                                    << databaseSequenceFirstObjectPath
                                                    << databaseSequenceSecondObjectPath;
    GTUtilsSharedDatabaseDocument::checkThereAreNoItemsExceptListed(os, databaseDoc, dstFolderPath, expectedItems);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0015) {
    //    Import a BAM file via import dialog.

    //    1. Connect to the "ugene_gui_test" database.

    //    2. Call context menu on the {/import_test_0015} folder in the database connection document, select {Add -> Import to the folder...} item.
    //    Expected state: an import dialog appears.

    //    3. Click the "Add files" button, select the {_common_data/bam/chrM.sorted.bam} file.
    //    Expected state: the document is added to the orders list, it will be imported into the {/import_test_0015} folder.

    //    4. Click the "Import" button.
    //    Expected state: an import task is started, there is an assembly object in the {/import_test_0015/chrM.sorted/} folder after the task has finished.

    //    5. Check the assembly object length and count of reads.
    //    Expected state: the length is 16571, there are 38461 reads.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0015");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString objectFolderName = "scerevisiae";
    const QString objectFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + objectFolderName;
    const QString assemblyObjectName = "Scmito";
    const QString databaseAssemblyObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + assemblyObjectName;
    const qint64 expectedLength = 85779;
    const qint64 expectedReadsCount = 2;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    QDir(sandBoxDir).mkdir(name);
    GTFile::copy(os, testDir + "_common_data/bam/scerevisiae.bam", sandBoxDir + name + "/scerevisiae.bam");
    GTUtilsSharedDatabaseDocument::importFiles(os, databaseDoc, dstFolderPath, QStringList() << sandBoxDir + name + "/scerevisiae.bam");

    const QStringList expectedItems = QStringList() << objectFolderPath
                                                    << databaseAssemblyObjectPath;
    GTUtilsSharedDatabaseDocument::checkThereAreNoItemsExceptListed(os, databaseDoc, dstFolderPath, expectedItems);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseAssemblyObjectPath);

    qint64 length = GTUtilsAssemblyBrowser::getLength(os);
    qint64 readsCount = GTUtilsAssemblyBrowser::getReadsCount(os);
    CHECK_SET_ERR(expectedLength == length, QString("The assembly length is incorrect: expect %1, got %2").arg(expectedLength).arg(length));
    CHECK_SET_ERR(expectedReadsCount == readsCount, QString("The assembly reads count is incorrect: expect %1, got %2").arg(expectedReadsCount).arg(readsCount));

    GTUtilsLog::check(os, lt);
}

GUI_TEST_CLASS_DEFINITION(import_test_0016) {
    //    Import a SAM file via import dialog.

    //    1. Connect to the "ugene_gui_test" database.

    //    2. Call context menu on the {/import_test_0016} folder in the database connection document, select {Add -> Import to the folder...} item.
    //    Expected state: an import dialog appears.

    //    3. Click the "Add files" button, select the {_common_data/sam/scerevisiae.sam} file.
    //    Expected state: the document is added to the orders list, it will be imported into the {/import_test_0016} folder.

    //    4. Click the "Import" button.
    //    Expected state: an import task is started, there is an assembly object in the {/import_test_0016/scerevisiae/} folder after the task has finished.

    //    5. Check the assembly object length and count of reads.
    //    Expected state: the length is 85779, there are 2 reads.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0016");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString objectFolderName = "scerevisiae";
    const QString objectFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + objectFolderName;
    const QString assemblyObjectName = "Scmito";
    const QString databaseAssemblyObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + assemblyObjectName;
    const qint64 expectedLength = 85779;
    const qint64 expectedReadsCount = 2;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    QDir(sandBoxDir).mkdir(name);
    GTFile::copy(os, testDir + "_common_data/sam/scerevisiae.sam", sandBoxDir + name + "/scerevisiae.sam");
    GTUtilsSharedDatabaseDocument::importFiles(os, databaseDoc, dstFolderPath, QStringList() << sandBoxDir + name + "/scerevisiae.sam");

    const QStringList expectedItems = QStringList() << objectFolderPath
                                                    << databaseAssemblyObjectPath;
    GTUtilsSharedDatabaseDocument::checkThereAreNoItemsExceptListed(os, databaseDoc, dstFolderPath, expectedItems);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseAssemblyObjectPath);

    qint64 length = GTUtilsAssemblyBrowser::getLength(os);
    qint64 readsCount = GTUtilsAssemblyBrowser::getReadsCount(os);
    CHECK_SET_ERR(expectedLength == length, QString("The assembly length is incorrect: expect %1, got %2").arg(expectedLength).arg(length));
    CHECK_SET_ERR(expectedReadsCount == readsCount, QString("The assembly reads count is incorrect: expect %1, got %2").arg(expectedReadsCount).arg(readsCount));

    GTUtilsLog::check(os, lt);
}

GUI_TEST_CLASS_DEFINITION(import_test_0017) {
    //    Import an ACE file via import dialog as assembly.

    //    1. Connect to the "ugene_gui_test" database.

    //    2. Call context menu on the {/import_test_0017} folder in the database connection document, select {Add -> Import to the folder...} item.
    //    Expected state: an import dialog appears.

    //    3. Click a "General options" button and set a specific option:
    //        Import ACE files as: Short reads assembly;
    //    and accept the dialog.

    //    4. Click the "Add files" button, select the {_common_data/ace/ace_test_2.ace} file.
    //    Expected state: the document is added to the orders list, it will be imported into the {/import_test_0017} folder.

    //    5. Click the "Import" button.
    //    Expected state: an import task is started, there are two assembly objects and two sequence objects in the {/import_test_0017/ace_test_2/} folder after the task has finished.

    //    6. Check the assembly object length, count of reads and check the reference is set.
    //    Expected state: the first assembly: the length is 871, there are 2 reads, reference is shown;
    //                    the second assembly: the length is 3296, there are 14 reads, reference is shown.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0017");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString objectFolderName = "ace_test_2";
    const QString objectFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + objectFolderName;
    const QString assemblyFirstObjectName = "Contig1";
    const QString assemblySecondObjectName = "Contig2";
    const QString sequenceFirstObjectName = "Contig1_ref";
    const QString sequenceSecondObjectName = "Contig2_ref";
    const QString databaseAssemblyFirstObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + assemblyFirstObjectName;
    const QString databaseAssemblySecondObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + assemblySecondObjectName;
    const QString databaseSequenceFirstObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + sequenceFirstObjectName;
    const QString databaseSequenceSecondObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + sequenceSecondObjectName;
    const qint64 expectedLengthFirst = 871;
    const qint64 expectedReadsCountFirst = 2;
    const qint64 expectedLengthSecond = 3296;
    const qint64 expectedReadsCountSecond = 14;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    QVariantMap options;
    options.insert(ImportToDatabaseOptions::PREFERRED_FORMATS, {"ace-importer"});
    GTUtilsSharedDatabaseDocument::importFiles(os, databaseDoc, dstFolderPath, QStringList() << testDir + "_common_data/ace/ace_test_2.ace", options);

    QStringList expectedItems = QStringList() << objectFolderPath
                                              << databaseAssemblyFirstObjectPath
                                              << databaseAssemblySecondObjectPath
                                              << databaseSequenceFirstObjectPath
                                              << databaseSequenceSecondObjectPath;
    GTUtilsSharedDatabaseDocument::checkThereAreNoItemsExceptListed(os, databaseDoc, dstFolderPath, expectedItems);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseAssemblyFirstObjectPath);

    qint64 lengthFirst = GTUtilsAssemblyBrowser::getLength(os);
    qint64 readsCountFirst = GTUtilsAssemblyBrowser::getReadsCount(os);
    bool hasReferenceFirst = GTUtilsAssemblyBrowser::hasReference(os, assemblyFirstObjectName);
    CHECK_SET_ERR(lengthFirst == expectedLengthFirst, QString("The assembly length is incorrect: expect %1, got %2").arg(expectedLengthFirst).arg(lengthFirst));
    CHECK_SET_ERR(readsCountFirst == expectedReadsCountFirst, QString("The assembly reads count is incorrect: expect %1, got %2").arg(expectedReadsCountFirst).arg(readsCountFirst));
    CHECK_SET_ERR(hasReferenceFirst, "The assembly reference is not set");

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseAssemblySecondObjectPath);

    qint64 lengthSecond = GTUtilsAssemblyBrowser::getLength(os);
    qint64 readsCountSecond = GTUtilsAssemblyBrowser::getReadsCount(os);
    bool hasReferenceSecond = GTUtilsAssemblyBrowser::hasReference(os, assemblySecondObjectName);
    CHECK_SET_ERR(lengthSecond == expectedLengthSecond, QString("The assembly length is incorrect: expect %1, got %2").arg(expectedLengthSecond).arg(lengthSecond));
    CHECK_SET_ERR(readsCountSecond == expectedReadsCountSecond, QString("The assembly reads count is incorrect: expect %1, got %2").arg(expectedReadsCountSecond).arg(readsCountSecond));
    CHECK_SET_ERR(hasReferenceSecond, "The assembly reference is not set");

    GTUtilsLog::check(os, lt);
}

GUI_TEST_CLASS_DEFINITION(import_test_0018) {
    //    Import a file via the import dialog, destination path is changed in the dialog

    //    1. Connect to the "ugene_gui_test" database.

    //    2. Create a folder in the database: {import_test_0018}.

    //    3. Call context menu on the database connection document, select {Add -> Import to the database...} item.
    //    Expected state: an import dialog appears.

    //    4. Click the "Add files" button, select {_common_data/ugenedb/chrM.sorted.bam.ugenedb}.
    //    Expected state: the file is added to the orders list, it will be imported into the {/} folder.

    //    5. Click twice to the item on the destination folder column and change the folder, set {/import_test_0018}.
    //    Expected state: the file is present in the orders list, it will be imported into the {/import_test_0018} folder.

    //    6. Click the "Import" button.
    //    Expected state: an import task is started, after it finishes an assembly object appears in the {/import_test_0018} folder.

    GTLogTracer lt;

    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0018");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString resultFolderName = "scerevisiae.bam";
    const QString resultFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + resultFolderName;
    const QString filePath = QFileInfo(testDir + "_common_data/ugenedb/scerevisiae.bam.ugenedb").absoluteFilePath();
    const QString assemblyObjectName = "Scmito";
    const QString databaseAssemblyObjectPath = resultFolderPath + U2ObjectDbi::PATH_SEP + assemblyObjectName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, U2ObjectDbi::ROOT_FOLDER, dstFolderName);

    QList<ImportToDatabaseDialogFiller::Action> actions;

    QVariantMap addFilesAction;
    addFilesAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, QStringList() << filePath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_FILES, addFilesAction);

    QVariantMap changeDestinationAction;
    changeDestinationAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__ITEM, filePath);
    changeDestinationAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__DESTINATION_FOLDER, dstFolderPath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::EDIT_DESTINATION_FOLDER, changeDestinationAction);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());

    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));
    GTUtilsSharedDatabaseDocument::callImportDialog(os, databaseDoc, U2ObjectDbi::ROOT_FOLDER);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, databaseAssemblyObjectPath);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0019) {
    //    Import a file via the import dialog, destination path is changed in the dialog

    //    1. Connect to the "ugene_gui_test" database.

    //    2. Create a folder in the database: {import_test_0019}.

    //    3. Call context menu on the database connection document, select {Add -> Import to the database...} item.
    //    Expected state: an import dialog appears.

    //    4. Click the "Add files" button, select {_common_data/ugenedb/1.bam.ugenedb}.
    //    Expected state: the file is added to the orders list, it will be imported into the {/} folder.

    //    5. Click twice to the item on the destination folder column and change the folder, set {/import_test_0019}.
    //    Expected state: the file is present in the orders list, it will be imported into the {/import_test_0019} folder.

    //    6. Click the "Import" button.
    //    Expected state: an import task is started, after it finishes an assembly object appears in the {/import_test_0019} folder.

    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0019");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString resultFolderName = "1.bam";
    const QString resultFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + resultFolderName;
    const QString filePath = QFileInfo(testDir + "_common_data/ugenedb/1.bam.ugenedb").absoluteFilePath();
    const QString assemblyObjectName = ">chrM";
    const QString databaseAssemblyObjectPath = resultFolderPath + U2ObjectDbi::PATH_SEP + assemblyObjectName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::createFolder(os, databaseDoc, parentFolderPath, dstFolderName);

    QList<ImportToDatabaseDialogFiller::Action> actions;

    QVariantMap addFilesAction;
    addFilesAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__PATHS_LIST, QStringList() << filePath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::ADD_FILES, addFilesAction);

    QVariantMap changeDestinationAction;
    changeDestinationAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__ITEM, filePath);
    changeDestinationAction.insert(ImportToDatabaseDialogFiller::Action::ACTION_DATA__DESTINATION_FOLDER, dstFolderPath);
    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::EDIT_DESTINATION_FOLDER, changeDestinationAction);

    actions << ImportToDatabaseDialogFiller::Action(ImportToDatabaseDialogFiller::Action::IMPORT, QVariantMap());
    GTUtilsDialog::waitForDialog(os, new ImportToDatabaseDialogFiller(os, actions));
    GTUtilsSharedDatabaseDocument::callImportDialog(os, databaseDoc, parentFolderPath);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    GTUtilsSharedDatabaseDocument::checkItemExists(os, databaseDoc, databaseAssemblyObjectPath);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(import_test_0020) {
    //    Import an ACE file via import dialog as malignment.

    //    1. Connect to the "ugene_gui_test" database.

    //    2. Call context menu on the {/import_test_0020} folder in the database connection document, select {Add -> Import to the folder...} item.
    //    Expected state: an import dialog appears.

    //    3. Click the "Add files" button, select the {_common_data/ace/ace_test_2.ace} file.
    //    Expected state: the document is added to the orders list, it will be imported into the {/import_test_0020} folder.

    //    4. Click the "Import" button.
    //    Expected state: an import task is started, there are two malignment objects in the {/import_test_0020/ace_test_2.ace/} folder after the task has finished.

    //    5. Check the malignment object length, count of sequences.
    //    Expected state: the first msa: the length is 871, there are 3 sequences;
    //                    the second msa: the length is 3296, there are 15 sequences.
    GTLogTracer lt;

    const QString parentFolderPath = U2ObjectDbi::ROOT_FOLDER;
    const QString dstFolderName = GTUtilsSharedDatabaseDocument::genTestFolderName("import_test_0020");
    const QString dstFolderPath = U2ObjectDbi::ROOT_FOLDER + dstFolderName;
    const QString objectFolderName = "ace_test_2";
    const QString objectFolderPath = dstFolderPath + U2ObjectDbi::ROOT_FOLDER + objectFolderName;
    const QString malignmentFirstObjectName = "Contig1";
    const QString malignmentSecondObjectName = "Contig2";
    const QString databaseMalignmentFirstObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + malignmentFirstObjectName;
    const QString databaseMalignmentSecondObjectPath = objectFolderPath + U2ObjectDbi::PATH_SEP + malignmentSecondObjectName;
    const qint64 expectedLengthFirst = 871;
    const qint64 expectedSequencesCountFirst = 3;
    const qint64 expectedLengthSecond = 3296;
    const qint64 expectedSequencesCountSecond = 15;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::importFiles(os, databaseDoc, dstFolderPath, QStringList() << testDir + "_common_data/ace/ace_test_2.ace");

    const QStringList expectedItems = QStringList() << objectFolderPath
                                                    << databaseMalignmentFirstObjectPath
                                                    << databaseMalignmentSecondObjectPath;
    GTUtilsSharedDatabaseDocument::checkThereAreNoItemsExceptListed(os, databaseDoc, dstFolderPath, expectedItems);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseMalignmentFirstObjectPath);

    qint64 lengthFirst = GTUtilsMSAEditorSequenceArea::getLength(os);
    qint64 sequenceCountFirst = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(expectedLengthFirst == lengthFirst, QString("The malignment length is incorrect: expect %1, got %2").arg(expectedLengthFirst).arg(lengthFirst));
    CHECK_SET_ERR(expectedSequencesCountFirst == sequenceCountFirst, QString("The malignment sequences count is incorrect: expect %1, got %2").arg(expectedSequencesCountFirst).arg(sequenceCountFirst));

    GTUtilsMdi::click(os, GTGlobals::Close);
    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseMalignmentSecondObjectPath);

    qint64 lengthSecond = GTUtilsMSAEditorSequenceArea::getLength(os);
    qint64 sequenceCountSecond = GTUtilsMsaEditor::getSequencesCount(os);
    CHECK_SET_ERR(expectedLengthSecond == lengthSecond, QString("The malignment length is incorrect: expect %1, got %2").arg(expectedLengthSecond).arg(lengthSecond));
    CHECK_SET_ERR(expectedSequencesCountSecond == sequenceCountSecond, QString("The malignment sequences count is incorrect: expect %1, got %2").arg(expectedSequencesCountSecond).arg(sequenceCountSecond));

    GTUtilsLog::check(os, lt);
}

GUI_TEST_CLASS_DEFINITION(view_test_0001) {
    //    View annotated sequence
    //    1. Connect to the "ugene_gui_test" database.
    //    2. Double click the object "/view_test_0001/NC_001363".
    //    Expected: the sequence and its annotations are opened in the sequence view.
    //    3. Scroll the sequence view.
    //    Expected: the sequence and annotations are correctly viewed.

    GTLogTracer lt;

    const QString folderName = "view_test_0001";
    const QString folderPath = U2ObjectDbi::PATH_SEP + folderName;
    const QString sequenceVisibleName = "NC_001363";
    const QString sequenceVisibleWidgetName = "NC_001363";
    const QString annotationVisibleName = "NC_001363 features";
    const QString someFeatureName = "CDS";
    const QString databaseSequenceObjectPath = folderPath + U2ObjectDbi::PATH_SEP + sequenceVisibleName;
    const int position = 2970;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseSequenceObjectPath);
    GTWidget::findWidget(os, sequenceVisibleWidgetName);

    GTUtilsAnnotationsTreeView::findItem(os, someFeatureName);

    GTUtilsSequenceView::goToPosition(os, position);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(view_test_0002) {
    //    View MSA
    //    1. Connect to the "ugene_gui_test" database.
    //    2. Double click the object "/view_test_0002/[m] COI".
    //    Expected: the alignment is opened in the MSA Editor.
    //    3. Scroll the alignment.
    //    Expected: the alignment is correctly viewed.

    GTLogTracer lt;

    const QString folderName = "view_test_0002";
    const QString folderPath = U2ObjectDbi::PATH_SEP + folderName;
    const QString malignmentVisibleName = "COI";
    const QString malignmentVisibleNameWidget = "COI";
    const QString databaseMalignmentObjectPath = folderPath + U2ObjectDbi::PATH_SEP + malignmentVisibleName;
    const QPoint position(300, 6);

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseMalignmentObjectPath);
    QWidget* msaView = GTWidget::findWidget(os, malignmentVisibleNameWidget);
    CHECK_SET_ERR(nullptr != msaView, "View wasn't opened");

    GTUtilsMSAEditorSequenceArea::clickToPosition(os, position);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(view_test_0003) {
    //    View assembly
    //    1. Connect to the "ugene_gui_test" database.
    //    2. Double click the object "/view_test_0003/[as] chrM".
    //    Expected: the assembly and its reference is opened in the Assembly browser.
    //    3. Scroll the assembly.
    //    Expected: the assembly is correctly viewed.

    GTLogTracer lt;

    const QString folderName = "view_test_0003";
    const QString folderPath = U2ObjectDbi::PATH_SEP + folderName;
    const QString assemblyVisibleName = "chrM";
    const QString assemblyVisibleNameWidget = "chrM";
    const QString databaseAssemblyObjectPath = folderPath + U2ObjectDbi::PATH_SEP + assemblyVisibleName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    QModelIndexList list = GTUtilsProjectTreeView::findIndeciesInProjectViewNoWait(os, assemblyVisibleName, GTUtilsProjectTreeView::findIndex(os, folderName));
    foreach (QModelIndex index, list) {
        if (index.data() == "[as] chrM") {
            GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, index);
        }
    }
    QWidget* assemblyView = GTWidget::findWidget(os, assemblyVisibleNameWidget);
    CHECK_SET_ERR(nullptr != assemblyView, "View wasn't opened");

    bool hasReference = GTUtilsAssemblyBrowser::hasReference(os, assemblyView);
    CHECK_SET_ERR(hasReference, "Reference is not found");

    GTUtilsAssemblyBrowser::zoomToMax(os);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(view_test_0004) {
    //    View text
    //    1. Connect to the "ugene_gui_test" database.
    //    2. Double click the object "/view_test_0004/text".
    //    Expected: the text is opened in the text editor.

    GTLogTracer lt;

    const QString folderName = "view_test_0004";
    const QString folderPath = U2ObjectDbi::PATH_SEP + folderName;
    const QString textVisibleName = "Text";
    const QString textVisibleNameWidget = "Text";
    const QString databaseTextObjectPath = folderPath + U2ObjectDbi::PATH_SEP + textVisibleName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseTextObjectPath);
    QWidget* textView = GTWidget::findWidget(os, textVisibleNameWidget);
    CHECK_SET_ERR(nullptr != textView, "View wasn't opened");

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(view_test_0005) {
    //    View chromatogram
    //    1. Connect to the "ugene_gui_test" database.
    //    2. Double click the object "/view_test_0005/[c] Chromatogram".
    //    Expected: the sequence and its chromatogram are opened in the sequence view.

    GTLogTracer lt;

    const QString folderName = "view_test_0005";
    const QString folderPath = U2ObjectDbi::PATH_SEP + folderName;
    const QString sequenceObjectName = "A1#berezikov";
    const QString chromatogramVisibleName = "Chromatogram";
    const QString databaseChromatogramObjectPath = folderPath + U2ObjectDbi::PATH_SEP + chromatogramVisibleName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseChromatogramObjectPath);

    QWidget* seqView = GTWidget::findWidget(os, sequenceObjectName);
    CHECK_SET_ERR(nullptr != seqView, "Sequence view wasn't opened");

    QWidget* chromaView = seqView->findChild<QWidget*>("chromatogram_view_" + sequenceObjectName);
    CHECK_SET_ERR(nullptr != chromaView, "Chromatogram view wasn't opened");

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(view_test_0006) {
    //    View tree
    //    1. Connect to the "ugene_gui_test" database.
    //    2. Double click the object "/view_test_0006/tree".
    //    Expected: the tree is opened in the Tree Viewer.

    GTLogTracer lt;

    const QString folderName = "view_test_0006";
    const QString folderPath = U2ObjectDbi::PATH_SEP + folderName;
    const QString treeVisibleName = "COI";
    const QString treeVisibleNameWidget = "COI";
    const QString databaseTreeObjectPath = folderPath + U2ObjectDbi::PATH_SEP + treeVisibleName;

    Document* databaseDoc = GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsSharedDatabaseDocument::openView(os, databaseDoc, databaseTreeObjectPath);
    QWidget* treeView = GTWidget::findWidget(os, treeVisibleNameWidget);
    CHECK_SET_ERR(nullptr != treeView, "View wasn't opened");

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(del_test_0001) {
    // Delete object
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Double click the object "/del_tests/dt0001_human_T1".
    // Expected: the sequence is opened.
    // 3. Right click the object "/del_tests/dt0001_human_T1" -> Remove object from document.
    // Expected: the sequence view is closed; the object is moved into the folder "/Recycle bin".
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);

    QString fileName = "dt0001_file";
    QModelIndex folderItem = GTUtilsProjectTreeView::findIndex(os, "del_tests");
    QModelIndex originalItem = GTUtilsProjectTreeView::findIndex(os, fileName, folderItem);
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, originalItem));
    GTMouseDriver::doubleClick();

    GTUtilsSequenceView::checkSequenceViewWindowIsActive(os);

    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__REMOVE_SELECTED));
    GTMouseDriver::click(Qt::RightButton);

    GTUtilsSequenceView::checkNoSequenceViewWindowIsOpened(os);

    // Check that the item is in the recycle bin.
    QModelIndex rbItem = GTUtilsProjectTreeView::findIndex(os, "Recycle bin");
    GTUtilsProjectTreeView::checkItem(os, fileName, rbItem);

    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(del_test_0002) {
    // Delete folder
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Double click the object "/del_tests/dt0002_dir/dt0002_COI".
    // Expected: the alignment is opened.
    // 3. Double click the object "/del_tests/dt0002_dir/dt0002_human_T1".
    // Expected: the sequence is opened.
    // 4. Right click the folder "/del_tests/dt0002_dir" -> Remove folder.
    // Expected: the views are closed; the folder and its objects are moved into the folder "/Recycle bin".
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");
    QAbstractItemModel* model = treeView->model();

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "dt0002_human_T1"));
    GTMouseDriver::doubleClick();
    GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    CHECK_OP(os, );

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "dt0002_COI"));
    GTMouseDriver::doubleClick();
    GTWidget::findWidget(os, "msa_editor_dt0002_COI_0");
    CHECK_OP(os, );

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "dt0002_dir"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__REMOVE_SELECTED));
    GTMouseDriver::click(Qt::RightButton);

    QWidget* seqView = GTWidget::findWidget(os, "ADV_single_sequence_widget_0", nullptr, {false});
    CHECK_SET_ERR(nullptr == seqView, "Sequence view is not closed");
    QWidget* msaView = GTWidget::findWidget(os, "msa_editor_dt0002_COI_0", nullptr, {false});
    CHECK_SET_ERR(nullptr == msaView, "MSA Editor is not closed");

    const QModelIndex rbItem = GTUtilsProjectTreeView::findIndex(os, "Recycle bin");
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, rbItem));
    const QModelIndex dirItem = GTUtilsProjectTreeView::findIndex(os, "dt0002_dir");
    CHECK_SET_ERR(rbItem == dirItem.parent(), "Folder is not in Recycle bin");
    CHECK_SET_ERR(0 == model->rowCount(dirItem), "Objects of a folder in recycle bin are shown");
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(del_test_0003) {
    // Clean up recycle bin [user]
    // 1. Connect to the "ugene_gui_test" database as a simple user.
    // 2. Right click the folder "/Recycle bin" -> Clean up recycle bin.
    // Expected: the folder "/Recycle bin" becomes empty.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);

    GTUtilsProjectTreeView::checkProjectViewIsOpened(os);
    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(treeView != nullptr, "Invalid project tree view");
    QAbstractItemModel* model = treeView->model();

    QModelIndex rbItem = GTUtilsProjectTreeView::findIndex(os, "Recycle bin");
    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, rbItem));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, {"empty_rb"}));
    GTUtilsDialog::waitForDialog(os, new MessageBoxDialogFiller(os, QMessageBox::Yes));
    GTMouseDriver::click(Qt::RightButton);
    GTUtilsDialog::checkNoActiveWaiters(os);
    GTUtilsTaskTreeView::waitTaskFinished(os);

    QModelIndex rbItemAfter = GTUtilsProjectTreeView::findIndex(os, "Recycle bin");
    CHECK_SET_ERR(model->rowCount(rbItemAfter) == 0, "Recycle bin is not empty");
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(export_test_0001) {
    // Export annotated sequence
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Right click the object "/export_tests/et0001_human_T1" -> Export/Import -> Export sequences
    // 3. Set the correct path and the Genbank format and click "Export".
    // Expected: the sequence and its annotations are exported into the file that is opened in the project.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "et0001_sequence"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_SEQUENCE));
    GTUtilsDialog::waitForDialog(os, new ExportSelectedRegionFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0001_export.fasta"));
    GTMouseDriver::click(Qt::RightButton);

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "et0001_export.fasta"));
    GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(export_test_0002) {
    // Export annotations
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Right click the object "/export_tests/et0002_features" -> Export/Import -> Export object
    // 3. Set the correct path and format and click "OK".
    // Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "et0002_features"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << "ep_exportAnnotations2CSV"));
    GTUtilsDialog::waitForDialog(os, new ExportAnnotationsFiller(testDir + "_common_data/scenarios/sandbox/et0002_features.gb", ExportAnnotationsFiller::genbank, os));
    GTMouseDriver::click(Qt::RightButton);

    bool exported = QFileInfo(testDir + "_common_data/scenarios/sandbox/et0002_features.gb").exists();
    CHECK_SET_ERR(exported, "Object is not exported");
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(export_test_0003) {
    // Export MSA
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Right click the object "/export_tests/et0003_alignment" -> Export/Import -> Export object
    // 3. Set the correct path and format and click "OK".
    // Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "et0003_alignment"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_OBJECT));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0003_alignment.aln", ExportDocumentDialogFiller::CLUSTALW, false, true));
    GTMouseDriver::click(Qt::RightButton);

    GTWidget::findWidget(os, "msa_editor_et0003_alignment_0");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(export_test_0004) {
    // Export assembly
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Right click the object "/export_tests/et0004_assemply" -> Export/Import -> Export object
    // 3. Set the correct path and format and click "OK".
    // Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "et0004_assembly"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_OBJECT));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0004_assembly.bam", ExportDocumentDialogFiller::BAM, false, true));
    GTUtilsDialog::waitForDialog(os, new ImportBAMFileFiller(os, testDir + "_common_data/scenarios/sandbox/et0004_assembly.bam.ugenedb"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep(40000);

    GTWidget::findWidget(os, "et0004_assembly [et0004_assembly.bam.ugenedb]");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(export_test_0005) {
    // Export variations
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Right click the object "/export_tests/variations" -> Export/Import -> Export object
    // 3. Set the correct path and format and click "OK".
    // Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "et0005_variations"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_OBJECT));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0005_variations.vcf", ExportDocumentDialogFiller::VCF, false, true));
    GTMouseDriver::click(Qt::RightButton);

    bool exported = QFileInfo(testDir + "_common_data/scenarios/sandbox/et0005_variations.vcf").exists();
    CHECK_SET_ERR(exported, "Object is not exported");
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(export_test_0006) {
    // Export text
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Right click the object "/export_tests/text" -> Export/Import -> Export object
    // 3. Set the correct path and format and click "OK".
    // Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "et0006_text"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_OBJECT));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0006_text.txt", ExportDocumentDialogFiller::TEXT, false, true));
    GTUtilsDialog::waitForDialog(os, new SelectDocumentFormatDialogFiller(os));
    GTMouseDriver::click(Qt::RightButton);

    GTWidget::findWidget(os, "et0006_text [et0006_text.txt]");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(export_test_0007) {
    // Export chromatogram
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Right click the object "/export_tests/et0007_chroma" -> Export/Import -> Export sequences
    // 3. Set the correct path and format and click "OK".
    // Expected: the sequence and its chromatogram are exported into the file that is opened in the project.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "et0007_chroma"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_CHROMATOGRAM));
    GTUtilsDialog::waitForDialog(os, new ExportChromatogramFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0007_chroma.scf", ExportChromatogramFiller::SCF, false, false, true));
    GTMouseDriver::click(Qt::RightButton);

    GTWidget::findWidget(os, "ADV_single_sequence_widget_0");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(export_test_0008) {
    // Export tree
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Right click the object "/export_tests/et0008_tree" -> Export/Import -> Export object
    // 3. Set the correct path and format and click "OK".
    // Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "et0008_tree"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_OBJECT));
    GTUtilsDialog::waitForDialog(os, new ExportDocumentDialogFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0008_tree.nwk", ExportDocumentDialogFiller::NWK, false, true));
    GTMouseDriver::click(Qt::RightButton);

    GTWidget::findWidget(os, "Tree [et0008_tree.nwk]");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

GUI_TEST_CLASS_DEFINITION(export_test_0009) {
    // Export tree
    // 1. Connect to the "ugene_gui_test" database.
    // 2. Right click the object "/export_tests/export_test_0009/long name with bad symbols : /?/=+\|*" -> Export/Import -> Export object
    // 3. Compare modified name with expected
    // Expected: the object is exported into the file that is opened in the project.
    GTLogTracer lt;
    GTUtilsSharedDatabaseDocument::connectToTestDatabase(os);
    CHECK_OP(os, );

    QTreeView* treeView = GTUtilsProjectTreeView::getTreeView(os);
    CHECK_SET_ERR(nullptr != treeView, "Invalid project tree view");

    GTMouseDriver::moveTo(GTUtilsProjectTreeView::getItemCenter(os, "long name with bad symbols : /?/=+\\|*"));
    GTUtilsDialog::waitForDialog(os, new PopupChooser(os, QStringList() << ACTION_PROJECT__EXPORT_IMPORT_MENU_ACTION << ACTION_EXPORT_SEQUENCE));
    GTUtilsDialog::waitForDialog(os, new ExportSelectedRegionFiller(os, testDir + "_common_data/scenarios/sandbox/", "et0009_export.fasta"));
    GTMouseDriver::click(Qt::RightButton);
    GTGlobals::sleep(10000);

    QFileInfo f(ExportSelectedRegionFiller::defaultExportPath);
    QString baseName = f.baseName();

    CHECK_SET_ERR(baseName == "long_name_with_bad_symbols__new", "Exporting basename don't match");
    CHECK_OP(os, );
    CHECK_SET_ERR(!lt.hasErrors(), "Errors in log: " + lt.getJoinedErrorString());
}

}  // namespace GUITest_common_scenarios_shared_database
}  // namespace U2
