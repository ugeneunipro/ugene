/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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
#include "AppSettingsDialogFiller.h"
#include <base_dialogs/ColorDialogFiller.h>
#include <drivers/GTKeyboardDriver.h>
#include <drivers/GTMouseDriver.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTListWidget.h>
#include <primitives/GTTreeWidget.h>
#include <primitives/GTWidget.h>
#include <utils/GTThread.h>

#include <QAbstractButton>
#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFile>
#include <QToolButton>
#include <QTreeWidget>

#include <U2Core/Log.h>

namespace U2 {
using namespace HI;

#define GT_CLASS_NAME "AppSettingsDialogFiller"
QMap<AppSettingsDialogFiller::Tabs, QString> AppSettingsDialogFiller::initMap() {
    QMap<Tabs, QString> result;
    result.insert(General, "  General");
    result.insert(Resources, "  Resources");
    result.insert(Network, "  Network");
    result.insert(FileFormat, "  File Format");
    result.insert(Directories, "  Directories");
    result.insert(Logging, "  Logging");
    result.insert(AlignmentColorScheme, "  Alignment Color Scheme");
    result.insert(GenomeAligner, "  Genome Aligner");
    result.insert(WorkflowDesigner, "  Workflow Designer");
    result.insert(ExternalTools, "  External Tools");
    return result;
}

const QMap<AppSettingsDialogFiller::Tabs, QString> AppSettingsDialogFiller::tabMap = initMap();

AppSettingsDialogFiller::AppSettingsDialogFiller(CustomScenario* customScenario)
    : Filler("AppSettingsDialog", customScenario),
      itemStyle(none),
      r(-1),
      g(-1),
      b(-1) {
}

void AppSettingsDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto tree = GTWidget::findTreeWidget("tree");

    QList<QTreeWidgetItem*> items = GTTreeWidget::getItems(tree->invisibleRootItem());
    foreach (QTreeWidgetItem* item, items) {
        if (item->text(0) == "  Workflow Designer") {
            GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(item));
            GTMouseDriver::click();
        }
    }
    if (itemStyle != none) {
        auto styleCombo = GTWidget::findComboBox("styleCombo", dialog);
        GTComboBox::selectItemByIndex(styleCombo, itemStyle);
    }

    if (r != -1) {
        GTUtilsDialog::waitForDialog(new ColorDialogFiller(r, g, b));
        auto colorWidget = GTWidget::findWidget("colorWidget", dialog);
        GTWidget::click(colorWidget);
    }

    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}

void AppSettingsDialogFiller::setExternalToolPath(const QString& toolName, const QString& toolPath) {
    auto dialog = GTWidget::getActiveModalWidget();

    openTab(ExternalTools);

    auto treeWidget = GTWidget::findTreeWidget("twIntegratedTools", dialog);
    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    for (QTreeWidgetItem* item : qAsConst(listOfItems)) {
        if (item->text(0) == toolName) {
            GTTreeWidget::scrollToItem(item);
            QWidget* itemWid = treeWidget->itemWidget(item, 1);
            GTLineEdit::setText("PathLineEdit", toolPath, itemWid);
            GTTreeWidget::click(item, 0);
            return;
        }
    }
    GT_FAIL("tool " + toolName + " not found in tree view", );
}

void AppSettingsDialogFiller::setExternalToolPath(const QString& toolName, const QString& path, const QString& name) {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    openTab(ExternalTools);

    auto treeWidget = GTWidget::findTreeWidget("twIntegratedTools", dialog);
    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    bool set = false;
    foreach (QTreeWidgetItem* item, listOfItems) {
        if (item->text(0) == toolName) {
            treeWidget->scrollToItem(item);
            GTThread::waitForMainThread();
            GTFileDialogUtils* ob = new GTFileDialogUtils(path, name, (GTFileDialogUtils::Button)GTFileDialog::Open, GTGlobals::UseMouse);
            GTUtilsDialog::waitForDialog(ob);

            QWidget* itemWid = treeWidget->itemWidget(item, 1);
            GT_CHECK(itemWid, "itemWid is NULL");

            QLineEdit* lineEdit = itemWid->findChild<QLineEdit*>("PathLineEdit");
            GT_CHECK(lineEdit, "lineEdit is NULL");

            QToolButton* clearToolPathButton = lineEdit->parentWidget()->findChild<QToolButton*>("ResetExternalTool");
            GT_CHECK(clearToolPathButton, "clearToolPathButton is NULL");

            GTWidget::click(clearToolPathButton);
            set = true;
        }
    }
    GT_CHECK(set, "tool " + toolName + " not found in tree view");
}

QString AppSettingsDialogFiller::getExternalToolPath(const QString& toolName) {
    auto dialog = GTWidget::getActiveModalWidget();

    openTab(ExternalTools);

    auto treeWidget = GTWidget::findTreeWidget("twIntegratedTools", dialog);
    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);

    for (QTreeWidgetItem* item : qAsConst(listOfItems)) {
        if (item->text(0) == toolName) {
            auto itemWid = treeWidget->itemWidget(item, 1);
            return GTWidget::findLineEdit("PathLineEdit", itemWid)->text();
        }
    }
    return "";
}

bool AppSettingsDialogFiller::isExternalToolValid(const QString& toolName) {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    openTab(ExternalTools);

    auto treeWidget = GTWidget::findTreeWidget("twIntegratedTools", dialog);
    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    for (QTreeWidgetItem* item : qAsConst(listOfItems)) {
        if (item->text(0) == toolName) {
            GTTreeWidget::click(item);
            GTMouseDriver::doubleClick();
            auto descriptionTextBrowser = GTWidget::findTextBrowser("descriptionTextBrowser", dialog);
            return descriptionTextBrowser->toPlainText().contains("Version:");
        }
    }
    GT_FAIL("external tool " + toolName + " not found in tree view", false);
}

void AppSettingsDialogFiller::clearToolPath(const QString& toolName) {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    openTab(ExternalTools);

    auto treeWidget = GTWidget::findTreeWidget("twIntegratedTools", dialog);
    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    foreach (QTreeWidgetItem* item, listOfItems) {
        if (item->text(0) == toolName) {
            QWidget* itemWid = treeWidget->itemWidget(item, 1);
            QToolButton* clearPathButton = itemWid->findChild<QToolButton*>("ClearToolPathButton");
            CHECK_SET_ERR(clearPathButton != nullptr, "Clear path button not found");
            treeWidget->scrollToItem(item);
            GTThread::waitForMainThread();
            if (clearPathButton->isEnabled()) {
                GTWidget::click(clearPathButton);
            }
        }
    }
}

bool AppSettingsDialogFiller::isToolDescriptionContainsString(const QString& toolName, const QString& checkIfContains) {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    clickOnTool(toolName);

    auto textBrowser = GTWidget::findTextBrowser("descriptionTextBrowser", dialog);

    QString plainText = textBrowser->toPlainText();
    return plainText.contains(checkIfContains);
}

void AppSettingsDialogFiller::setTemporaryDirPath(const QString& path) {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    openTab(Directories);

    GTLineEdit::setText("tmpDirPathEdit", path, dialog);
}

void AppSettingsDialogFiller::setDocumentsDirPath(const QString& path) {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    openTab(Directories);

    GTLineEdit::setText("documentsDirectoryEdit", path, dialog);
}

void AppSettingsDialogFiller::setWorkflowOutputDirPath(const QString& path) {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    openTab(WorkflowDesigner);

    GTLineEdit::setText("workflowOutputEdit", path, dialog);
}

void AppSettingsDialogFiller::openTab(Tabs tab) {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    QString itemText = tabMap.value(tab);
    GT_CHECK(!itemText.isEmpty(), "tree element for item not found");

    auto mainTree = GTWidget::findTreeWidget("tree", dialog);
    if (mainTree->selectedItems().first()->text(0) != itemText) {
        GTTreeWidget::click(GTTreeWidget::findItem(mainTree, itemText));
    }
    GTGlobals::sleep(300);
    GTThread::waitForMainThread();
}

void AppSettingsDialogFiller::clickOnTool(const QString& toolName) {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    openTab(ExternalTools);

    auto treeWidget = GTWidget::findTreeWidget("twIntegratedTools", dialog);
    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    foreach (QTreeWidgetItem* item, listOfItems) {
        if (item->text(0) == toolName) {
            GTTreeWidget::click(item);
            return;
        }
    }
}

void AppSettingsDialogFiller::setExternalToolsDir(const QString& dirPath) {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    openTab(ExternalTools);

    auto selectExToolsDirButton = GTWidget::findWidget("selectToolPackButton", dialog);
    while (!selectExToolsDirButton->isEnabled()) {
        uiLog.trace("selectToolPackButton is disabled");
        GTGlobals::sleep(100);
    }

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(dirPath, "", GTFileDialogUtils::Choose));
    GTWidget::click(selectExToolsDirButton);
}

#undef GT_CLASS_NAME

NewColorSchemeCreator::NewColorSchemeCreator(QString _schemeName, alphabet _al, Action _act, bool cancel)
    : Filler("AppSettingsDialog"), schemeName(_schemeName), al(_al), act(_act), cancel(cancel) {
}

NewColorSchemeCreator::NewColorSchemeCreator(CustomScenario* c)
    : Filler("AppSettingsDialog", c),
      al(nucl),
      act(Create),
      cancel(true) {
}

#define GT_CLASS_NAME "NewColorSchemeCreator"
void NewColorSchemeCreator::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto tree = GTWidget::findTreeWidget("tree");

    QList<QTreeWidgetItem*> items = GTTreeWidget::getItems(tree->invisibleRootItem());
    foreach (QTreeWidgetItem* item, items) {
        if (item->text(0) == "  Alignment Color Scheme") {
            GTMouseDriver::moveTo(GTTreeWidget::getItemCenter(item));
            GTMouseDriver::click();
        }
    }

    switch (act) {
        case Delete: {
            auto colorSchemas = GTWidget::findListWidget("colorSchemas", dialog);
            GTListWidget::click(colorSchemas, schemeName);
            GTGlobals::sleep(500);

            auto deleteSchemaButton = GTWidget::findWidget("deleteSchemaButton", dialog);
            while (!deleteSchemaButton->isEnabled()) {
                uiLog.trace("deleteSchemaButton is disabled");
                GTGlobals::sleep(100);
            }
            GTWidget::click(deleteSchemaButton);
            break;
        }
        case Create: {
            auto addSchemaButton = GTWidget::findWidget("addSchemaButton");

            GTUtilsDialog::waitForDialog(new CreateAlignmentColorSchemeDialogFiller(schemeName, al));
            GTWidget::click(addSchemaButton);
            break;
        }
        case Change: {
            GTListWidget::click(GTWidget::findListWidget("colorSchemas", dialog), schemeName);

            class Scenario : public CustomScenario {
            public:
                void run() {
                    QWidget* dialog = GTWidget::getActiveModalWidget();
                    GTUtilsDialog::waitForDialog(new ColorDialogFiller(255, 0, 0));
                    GTWidget::click(GTWidget::findWidget("alphabetColorsFrame", dialog), Qt::LeftButton, QPoint(5, 5));

                    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
                }
            };

            GTUtilsDialog::waitForDialog(new ColorSchemeDialogFiller(new Scenario));
            GTWidget::click(GTWidget::findWidget("changeSchemaButton", dialog));
        }
    }

    GTUtilsDialog::clickButtonBox(dialog, cancel ? QDialogButtonBox::Cancel : QDialogButtonBox::Ok);
}
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "CreateAlignmentColorSchemeDialogFiller"
void CreateAlignmentColorSchemeDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    auto w = GTWidget::findWidget("schemeName", dialog);
    auto schemeNameLine = qobject_cast<QLineEdit*>(w);
    GT_CHECK(schemeNameLine, "schemeName lineEdit not found ");

    GTLineEdit::setText(schemeNameLine, schemeName);

    auto alphabetComboBox = GTWidget::findComboBox("alphabetComboBox", dialog);

    GTComboBox::selectItemByIndex(alphabetComboBox, al);
    GTGlobals::sleep(500);

    GTUtilsDialog::waitForDialog(new ColorSchemeDialogFiller());

    GTUtilsDialog::clickButtonBox(QDialogButtonBox::Ok);
}
#undef GT_CLASS_NAME

#define GT_CLASS_NAME "ColorSchemeDialogFiller"
void ColorSchemeDialogFiller::commonScenario() {
    QWidget* dialog = GTWidget::getActiveModalWidget();

    QList<QAbstractButton*> list = dialog->findChildren<QAbstractButton*>();
    foreach (QAbstractButton* b, list) {
        if (b->text().contains("ok", Qt::CaseInsensitive)) {
            GTWidget::click(b);
            return;
        }
    }
    GTKeyboardDriver::keyClick(Qt::Key_Enter);  // if ok button not found
}
#undef GT_CLASS_NAME
// QDialogButtonBox *buttonBox;
}  // namespace U2
