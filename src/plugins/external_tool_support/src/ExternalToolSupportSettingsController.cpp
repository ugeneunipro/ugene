/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QMessageBox>
#include <QToolButton>

#include <U2Core/AppContext.h>
#include <U2Core/L10n.h>
#include <U2Core/MultiTask.h>
#include <U2Core/ScriptingToolRegistry.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/GUIUtils.h>

#include "ExternalToolSupportSettings.h"
#include "ExternalToolSupportSettingsController.h"
#include "blast/BlastAllSupport.h"
#include "blast/FormatDBSupport.h"
#include "blast_plus/BlastPlusSupport.h"
#include "utils/ExternalToolValidateTask.h"

namespace U2 {

static const int TOOLKIT_TYPE = QTreeWidgetItem::UserType + 1;

/////////////////////////////////////////////
////ExternalToolSupportSettingsPageController
ExternalToolSupportSettingsPageController::ExternalToolSupportSettingsPageController(QObject* p)
: AppSettingsGUIPageController(tr("External Tools"), ExternalToolSupportSettingsPageId, p) {}


AppSettingsGUIPageState* ExternalToolSupportSettingsPageController::getSavedState() {
    ExternalToolSupportSettingsPageState* state = new ExternalToolSupportSettingsPageState();
    state->externalTools = AppContext::getExternalToolRegistry()->getAllEntries();
    return state;
}

void ExternalToolSupportSettingsPageController::saveState(AppSettingsGUIPageState* s) {
    Q_UNUSED(s);
    ExternalToolSupportSettings::setExternalTools();
}

AppSettingsGUIPageWidget* ExternalToolSupportSettingsPageController::createWidget(AppSettingsGUIPageState* state) {
    ExternalToolSupportSettingsPageWidget* r = new ExternalToolSupportSettingsPageWidget(this);
    r->setState(state);
    return r;
}

const QString ExternalToolSupportSettingsPageController::helpPageId = QString("21433149");

/////////////////////////////////////////////
////ExternalToolSupportSettingsPageWidget
const QString ExternalToolSupportSettingsPageWidget::INSTALLED = QObject::tr("Installed");
const QString ExternalToolSupportSettingsPageWidget::NOT_INSTALLED = QObject::tr("Not installed");

ExternalToolSupportSettingsPageWidget::ExternalToolSupportSettingsPageWidget(ExternalToolSupportSettingsPageController* ctrl) {
    Q_UNUSED(ctrl);

    setupUi(this);
    versionLabel->hide();
    binaryPathLabel->hide();
}

QWidget* ExternalToolSupportSettingsPageWidget::createPathEditor(QWidget* parent, const QString& path) const {
    QWidget* widget = new QWidget(parent);

    PathLineEdit* toolPathEdit = new PathLineEdit("" ,"executable", false, widget);
    toolPathEdit->setObjectName("PathLineEdit");
    toolPathEdit->setFrame(false);
    toolPathEdit->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    toolPathEdit->setText(QDir::toNativeSeparators(path));

    widget->setFocusProxy(toolPathEdit);
    connect(toolPathEdit, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(sl_onPathEditWidgetClick()));
    connect(toolPathEdit, SIGNAL(textEdited(QString)), this, SLOT(sl_onPathEditWidgetClick()));
    connect(toolPathEdit, SIGNAL(selectionChanged()), this, SLOT(sl_onPathEditWidgetClick()));
    connect(toolPathEdit, SIGNAL(editingFinished()), this, SLOT(sl_toolPathChanged()));

    QToolButton* selectToolPathButton = new QToolButton(widget);
    selectToolPathButton->setVisible(true);
    selectToolPathButton->setText("...");
    selectToolPathButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));

    connect(selectToolPathButton, SIGNAL(clicked()), this, SLOT(sl_onPathEditWidgetClick()));
    connect(selectToolPathButton, SIGNAL(clicked()), toolPathEdit, SLOT(sl_onBrowse()));

    QToolButton* clearToolPathButton = new QToolButton(widget);
    clearToolPathButton->setObjectName("ClearToolPathButton");
    clearToolPathButton->setVisible(true);
    clearToolPathButton->setIcon(QIcon(":external_tool_support/images/cancel.png"));
    clearToolPathButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    clearToolPathButton->setEnabled(!path.isEmpty());

    connect(clearToolPathButton, SIGNAL(clicked()), this, SLOT(sl_onPathEditWidgetClick()));
    connect(clearToolPathButton, SIGNAL(clicked()), toolPathEdit, SLOT(sl_clear()));


    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(toolPathEdit);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(selectToolPathButton);
    buttonsLayout->addWidget(clearToolPathButton);

    layout->addLayout(buttonsLayout);
    buttonsWidth = buttonsLayout->minimumSize().width();
    descriptionTextBrowser->setOpenLinks(false);
    connect(descriptionTextBrowser, SIGNAL(anchorClicked(const QUrl&)), SLOT(sl_onClickLink(const QUrl&)));

    return widget;
}

void ExternalToolSupportSettingsPageWidget::sl_onClickLink(const QUrl& url) {
    const QAbstractItemModel* model = treeWidget->selectionModel()->model();
    QModelIndexList items = model->match(model->index(0, 0), Qt::DisplayRole, QVariant::fromValue(url.toEncoded()), 2, Qt::MatchRecursive);
    if (items.isEmpty()){
        return;
    }
    treeWidget->setCurrentIndex(items[0]);
}

void ExternalToolSupportSettingsPageWidget::sl_linkActivated(const QString& url) {
    GUIUtils::runWebBrowser(url);
}

void ExternalToolSupportSettingsPageWidget::setState(AppSettingsGUIPageState* s) {
    ExternalToolSupportSettingsPageState* state = qobject_cast<ExternalToolSupportSettingsPageState*>(s);
    connect(selectToolPackButton, SIGNAL(clicked()), this, SLOT(sl_onBrowseToolPackPath()));
    connect(linkLabel, SIGNAL(linkActivated(QString)), this, SLOT(sl_linkActivated(QString)));

    foreach (ExternalTool* tool, state->externalTools) {
        ExternalToolInfo info;
        info.name = tool->getName();
        info.path = tool->getPath();
        info.description = tool->getDescription();
        info.valid = tool->isValid();
        info.version = tool->getVersion();
        info.isModule = tool->isModule();
        externalToolsInfo.insert(info.name, info);

        connect(tool, SIGNAL(si_toolValidationStatusChanged(bool)), SLOT(sl_toolValidationStatusChanged(bool)));
    }
    treeWidget->setColumnWidth(0, this->geometry().width() / 3);

    QList<QList<ExternalTool*> > listToolKits = AppContext::getExternalToolRegistry()->getAllEntriesSortedByToolKits();
    foreach (QList<ExternalTool*> toolsList, listToolKits) {
        if (toolsList.length() > 1) {
            ExternalTool* masterOfGroup = isMasterWithModules(toolsList);

            if (NULL != masterOfGroup) {
                QTreeWidgetItem* rootItem = insertChild(treeWidget->invisibleRootItem(), masterOfGroup->getName(), treeWidget->topLevelItemCount());

                foreach (ExternalTool* tool, toolsList) {
                    if (tool != masterOfGroup) {
                        insertChild(rootItem, tool->getName(), 0, true);
                    }
                }
                rootItem->setExpanded(false);
            } else {
                QTreeWidgetItem* rootItem = new QTreeWidgetItem((QStringList)toolsList.first()->getToolKitName(), TOOLKIT_TYPE);

                rootItem->setIcon(0, toolsList.first()->getIcon());
                treeWidget->insertTopLevelItem(0, rootItem);
                foreach (ExternalTool* tool, toolsList) {
                    insertChild(rootItem, tool->getName(), 0);
                }
                rootItem->setExpanded(true);

                //draw widget for path select button
                QWidget* widget = new QWidget(treeWidget);
                QToolButton* selectToolKitPathButton = new QToolButton(widget);
                selectToolKitPathButton->setVisible(true);
                selectToolKitPathButton->setText("...");
                selectToolKitPathButton->setMinimumWidth(buttonsWidth);
                selectToolKitPathButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));

                connect(selectToolKitPathButton, SIGNAL(clicked()), this, SLOT(sl_onPathEditWidgetClick()));
                connect(selectToolKitPathButton, SIGNAL(clicked()), this, SLOT(sl_onBrowseToolKitPath()));

                QHBoxLayout* layout = new QHBoxLayout(widget);
                layout->setSpacing(0);
                layout->setMargin(0);
                layout->addStretch();
                layout->addWidget(selectToolKitPathButton);
                treeWidget->setItemWidget(rootItem, 1, widget);
            }

        } else {
            QTreeWidgetItem* item = new QTreeWidgetItem((QStringList) toolsList.first()->getName());
            externalToolsItems.insert(toolsList.first()->getName(), item);
            treeWidget->addTopLevelItem(item);
            treeWidget->setItemWidget(item, 1, createPathEditor(treeWidget, toolsList.first()->getPath()));

            if (toolsList.first()->getPath().isEmpty()) {
                item->setIcon(0, toolsList.first()->getGrayIcon());
            } else if (toolsList.first()->isValid()) {
                item->setIcon(0, toolsList.first()->getIcon());
            } else {
                item->setIcon(0, toolsList.first()->getWarnIcon());
            }
        }
    }
    connect(treeWidget, SIGNAL(itemSelectionChanged()), SLOT(sl_itemSelectionChanged()));
}

QTreeWidgetItem* ExternalToolSupportSettingsPageWidget::insertChild(QTreeWidgetItem* rootItem, const QString& name, int pos, bool isModule) {
    QTreeWidgetItem* item = new QTreeWidgetItem((QStringList)name);
    externalToolsItems.insert(name, item);

    rootItem->insertChild(pos, item);

    if (isModule) {
        QString toolStatus = externalToolsInfo.value(name).valid ? INSTALLED : NOT_INSTALLED;
        treeWidget->setItemWidget(item, 1, new QLabel(toolStatus));
    } else {
        treeWidget->setItemWidget(item, 1, createPathEditor(treeWidget, externalToolsInfo[name].path));
    }

    if (externalToolsInfo.value(name).path.isEmpty()) {
        item->setIcon(0, AppContext::getExternalToolRegistry()->getByName(name)->getGrayIcon());
    } else if (externalToolsInfo.value(name).valid) {
        item->setIcon(0, AppContext::getExternalToolRegistry()->getByName(name)->getIcon());
    } else {
        item->setIcon(0, AppContext::getExternalToolRegistry()->getByName(name)->getWarnIcon());
    }

    return item;
}

ExternalTool* ExternalToolSupportSettingsPageWidget::isMasterWithModules(const QList<ExternalTool*>& toolsList) const {
    ExternalTool* master = NULL;
    foreach (ExternalTool* tool, toolsList) {
        if (tool->isModule()) {
            continue;
        }
        if (NULL == master) {
            master = tool;
        } else {
            return NULL;
        }
    }
    return master;
}

void ExternalToolSupportSettingsPageWidget::setToolState(ExternalTool* tool) {
    QTreeWidgetItem* item = externalToolsItems.value(tool->getName(), NULL);
    SAFE_POINT(NULL != item, QString("Tree item for the tool %1 not found").arg(tool->getName()), );

    externalToolsInfo[tool->getName()].valid = tool->isValid();
    QLabel* moduleToolLabel = qobject_cast<QLabel*>(treeWidget->itemWidget(item, 1));
    QString moduleToolState;
    QString toolStateDesc;

    if (tool->isValid()) {
        item->setIcon(0, AppContext::getExternalToolRegistry()->getByName(tool->getName())->getIcon());
        moduleToolState = INSTALLED;
    } else if (!tool->getPath().isEmpty()) {
        toolStateDesc = getToolStateDescription(tool);
        item->setIcon(0, AppContext::getExternalToolRegistry()->getByName(tool->getName())->getWarnIcon());
        moduleToolState = NOT_INSTALLED;
    } else {
        item->setIcon(0, AppContext::getExternalToolRegistry()->getByName(tool->getName())->getGrayIcon());
        moduleToolState = "";
    }

    if (moduleToolLabel) {
        moduleToolLabel->setText(moduleToolState);
    }

    externalToolsInfo[tool->getName()].path = tool->getPath();
    if (!tool->getVersion().isEmpty()) {
        externalToolsInfo[tool->getName()].version = tool->getVersion();
    } else {
        externalToolsInfo[tool->getName()].version = "unknown";
    }

    QList<QTreeWidgetItem*> selectedItems = treeWidget->selectedItems();
    CHECK(selectedItems.length() > 0, );
    QString selectedName = selectedItems.at(0)->text(0);

    if (selectedName == tool->getName()) {
        setDescription(tool);
    }
}

QString ExternalToolSupportSettingsPageWidget::getToolLink(const QString &toolName) const {
    return "<a href='" + toolName + "'>" + toolName + "</a>";
}

QString ExternalToolSupportSettingsPageWidget::getToolStateDescription(ExternalTool *tool) const {
    QString result;

    SAFE_POINT(tool, "Tool pointer is NULL", result);
    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    SAFE_POINT(etRegistry, "External tool registry is NULL", result);
    ExternalToolManager* etManager = etRegistry->getManager();
    SAFE_POINT(etManager, "External tool manager is NULL", result);

    ExternalToolManager::ExternalToolState state = etManager->getToolState(tool->getName());

    if (state == ExternalToolManager::NotValidByDependency) {
        QString text = tr("External tool '%1' cannot be validated as it "
                          "depends on other tools, some of which are not valid. "
                          "The list of tools is the following: ").arg(tool->getName());

        QStringList invalidDependencies;
        QStringList dependencies = tool->getDependencies();
        foreach (const QString& masterName, dependencies) {
            if (ExternalToolManager::Valid != etManager->getToolState(masterName)) {
                if (tool->getName() != masterName && tool->getToolKitName() != masterName) {
                    invalidDependencies << getToolLink(masterName);
                }else {
                    invalidDependencies << masterName;
                }
            }
        }
        result = warn(text + invalidDependencies.join(", ")) + "<br><br>";
    }

    if (state == ExternalToolManager::NotValid) {
        if (tool->isModule()) {
            QStringList toolDependencies = tool->getDependencies();
            SAFE_POINT(!toolDependencies.isEmpty(), QString("Empty dependency list for "
                                                            "the '%1' module tool").arg(tool->getName()), result);
            QString masterName = toolDependencies.first();
            QString text = tr("'%1' is %2 module and it is not installed. "
                              "Install it and restart UGENE or set another "
                              "%2 with already installed '%1' module.")
                           .arg(tool->getName()).arg(masterName);

            result = warn(text) + "<br><br>";
        }
    }

    return result;
}

void ExternalToolSupportSettingsPageWidget::setDescription(ExternalTool* tool) {
    QString desc = tr("No description");

    if (tool) {
        desc = getToolStateDescription(tool);
        if (desc.size() == 0){
            desc = tool->getDescription();
        }else{
            desc += tool->getDescription();
        }
        if (tool->isValid()) {
            desc += tr("<br><br>Version: ");
            if (!externalToolsInfo[tool->getName()].version.isEmpty()) {
                desc += externalToolsInfo[tool->getName()].version;
            } else {
                desc += tr("unknown");
            }
        }

        if (!externalToolsInfo[tool->getName()].path.isEmpty()) {
            desc += tr("<br><br>Binary path: ");
            desc += externalToolsInfo[tool->getName()].path;
        }
    }
    descriptionTextBrowser->setText(desc + "<a href='1'></a>");
}

QString ExternalToolSupportSettingsPageWidget::warn(const QString& text) const {
    return "<span style=\"color:" + L10N::errorColorLabelStr( ) + "; font:bold;\">" + text + "</span>";
}

AppSettingsGUIPageState* ExternalToolSupportSettingsPageWidget::getState(QString& err) const {
    Q_UNUSED(err);

    ExternalToolSupportSettingsPageState* state = new ExternalToolSupportSettingsPageState();
    foreach (ExternalToolInfo info, externalToolsInfo){
        ExternalTool* externalTool = new ExternalTool(info.name, info.path);
        externalTool->setValid(info.valid);
        externalTool->setVersion(info.version);
        state->externalTools.append(externalTool);
    }
    return state;
}

void ExternalToolSupportSettingsPageWidget::sl_toolPathChanged() {
    PathLineEdit* s = qobject_cast<PathLineEdit*>(sender());

    if (!s || !s->isModified()) {
        return;
    }

    QWidget* par = s->parentWidget();
    QString path = s->text();
    s->setModified(false);

    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    SAFE_POINT(listOfItems.length() != 0, "ExternalToolSupportSettings, NO items are selected", );

    treeWidget->clearSelection();
    foreach (QTreeWidgetItem* item, listOfItems) {
        QWidget* itemWid = treeWidget->itemWidget(item, 1);
        if (par == itemWid) {       //may be no good method for check QTreeWidgetItem
            QString toolName = item->text(0);
            if (path.isEmpty()) {
                item->setIcon(0, AppContext::getExternalToolRegistry()->getByName(toolName)->getGrayIcon());
            }

            ExternalToolManager* etManager = AppContext::getExternalToolRegistry()->getManager();
            SAFE_POINT(NULL != etManager, "External tool manager is null", );

            ExternalToolValidationListener* listener = new ExternalToolValidationListener(toolName);
            connect(listener, SIGNAL(si_validationComplete()), SLOT(sl_validationComplete()));
            etManager->validate(toolName, path, listener);
        }
    }
}

void ExternalToolSupportSettingsPageWidget::sl_validationComplete() {
    ExternalToolValidationListener* listener = qobject_cast<ExternalToolValidationListener*>(sender());
    SAFE_POINT(NULL != listener, "Unexpected message sender", );

    listener->deleteLater();

    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    CHECK(etRegistry, );

    foreach (const QString& toolName, listener->getToolNames()){
        ExternalTool* tool = etRegistry->getByName(toolName);
        SAFE_POINT(NULL != tool, QString("External tool %1 not found in the registry.").arg(toolName), );
        setToolState(tool);
    }
}

void ExternalToolSupportSettingsPageWidget::sl_toolValidationStatusChanged(bool isValid) {
    Q_UNUSED(isValid);

    ExternalTool* s = qobject_cast<ExternalTool*>(sender());
    SAFE_POINT(NULL != s, "Unexpected message sender", );

    setToolState(s);
}

void ExternalToolSupportSettingsPageWidget::sl_itemSelectionChanged() {
    QList<QTreeWidgetItem*> selectedItems = treeWidget->selectedItems();
    if (selectedItems.length() == 0) {
        descriptionTextBrowser->setText(tr("Select an external tool to view more information about it."));
        return;
    }
    SAFE_POINT(selectedItems.length() != 0, "ExternalToolSupportSettings, NO items're selected", );

    QString name = selectedItems.at(0)->text(0);

    if (selectedItems.at(0)->type() == TOOLKIT_TYPE) {
        QString text = AppContext::getExternalToolRegistry()->getToolkitDescription(name);
        if (!text.isEmpty()) {
            descriptionTextBrowser->setText(text);
            return;
        }
    }

    //no description or tool custom description
    ExternalTool* tool = AppContext::getExternalToolRegistry()->getByName(name);
    setDescription(tool);
}

void ExternalToolSupportSettingsPageWidget::sl_onPathEditWidgetClick() {
    QWidget* s = qobject_cast<QWidget*>(sender());
    SAFE_POINT(NULL != s, "Unexpected message sender", );

    QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    SAFE_POINT(listOfItems.length() != 0, "No items were found in the tree", );

    treeWidget->clearSelection();
    foreach (QTreeWidgetItem* item, listOfItems) {
        QWidget* par = s->parentWidget();
        QWidget* itemWid = treeWidget->itemWidget(item, 1);
        if (par == itemWid) {
            item->setSelected(true);
        }
    }
}

//looks in selected folder +1 level 1 subfolders
void ExternalToolSupportSettingsPageWidget::sl_onBrowseToolKitPath(){
    LastUsedDirHelper lod("toolkit path");
    QString dir;

    lod.url = dir = U2FileDialog::getExistingDirectory(this, tr("Choose Folder With Executables"), lod.dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        assert(treeWidget->selectedItems().isEmpty() == 0);
        QString toolKitName = treeWidget->selectedItems().first()->text(0);
        QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("", Qt::MatchContains | Qt::MatchRecursive);
        assert(listOfItems.length() != 0);

        QStringList toolNames;
        StrStrMap toolPaths;
        foreach (QTreeWidgetItem* item, listOfItems) {
            if (!externalToolsItems.values().contains(item)) {
                continue;
            }
            QString toolItemText = item->text(0);
            if (AppContext::getExternalToolRegistry()->getByName(toolItemText) != NULL) {
                if (AppContext::getExternalToolRegistry()->getByName(item->text(0))->getToolKitName() == toolKitName) {
                    QWidget* itemWid = treeWidget->itemWidget(item, 1);
                    PathLineEdit* lineEdit = itemWid->findChild<PathLineEdit*>("PathLineEdit");
                    LimitedDirIterator it(dir);
                    bool fileNotFound = true;
                    QString executableFileName = AppContext::getExternalToolRegistry()->getByName(item->text(0))->getExecutableFileName();
                    while (it.hasNext() && fileNotFound) {
                        it.next();
                        QString fpath = it.filePath() + QDir::separator() + executableFileName;

                        QFileInfo info(fpath);
                        if (info.exists() && info.isFile()) {
                            QString path = QDir::toNativeSeparators(fpath);
                            lineEdit->setText(path);
                            lineEdit->setModified(false);
                            externalToolsInfo[item->text(0)].path = path;
                            QToolButton* clearToolPathButton = itemWid->findChild<QToolButton*>("ClearToolPathButton");
                            assert(clearToolPathButton);
                            clearToolPathButton->setEnabled(true);
                            toolNames << item->text(0);
                            toolPaths.insert(item->text(0), path);
                            fileNotFound = false;
                        }
                    }
                }
            }
        }
        if (!toolNames.isEmpty()) {
            ExternalToolManager* etManager = AppContext::getExternalToolRegistry()->getManager();
            ExternalToolValidationListener* listener = new ExternalToolValidationListener(toolNames);
            connect(listener, SIGNAL(si_validationComplete()), SLOT(sl_validationComplete()));
            etManager->validate(toolNames, toolPaths, listener);
        }
    }
}

void ExternalToolSupportSettingsPageWidget::sl_onBrowseToolPackPath() {
    LastUsedDirHelper lod("toolpack path");
    QString dirPath;
    lod.dir = dirPath = U2FileDialog::getExistingDirectory(this, tr("Choose Folder With External Tools Pack"), lod.dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dirPath.isEmpty()) {
        QDir dir = QDir(dirPath);
        QList<QTreeWidgetItem*> listOfItems = treeWidget->findItems("" , Qt::MatchContains | Qt::MatchRecursive);
        assert(listOfItems.length() != 0);
        QStringList toolNames;
        StrStrMap toolPaths;
        bool isPathValid = false;

        foreach (ExternalTool* et, AppContext::getExternalToolRegistry()->getAllEntries()) {
            if (et->isModule()) {
                continue;
            }
            QTreeWidgetItem* item = externalToolsItems.value(et->getName(), NULL);
            SAFE_POINT(NULL != item, QString("Tree item not found for the tool %1").arg(et->getName()), );

            foreach (QString dirName, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                QString toolKitName = et->getToolKitName();
                if (dirName.contains(toolKitName, Qt::CaseInsensitive)) {
                    isPathValid = true;
                    QWidget* itemWid = treeWidget->itemWidget(item, 1);
                    PathLineEdit* lineEdit = itemWid->findChild<PathLineEdit*>("PathLineEdit");

                    QString toolPath = dirPath + QDir::separator() + dirName;
                    QDir toolDir(toolPath);
                    LimitedDirIterator it(toolDir);
                    bool fileNotFound = true;
                    QString executableFileName = AppContext::getExternalToolRegistry()->getByName(item->text(0))->getExecutableFileName();
                    while (it.hasNext() && fileNotFound) {
                        it.next();
                        QString fName = it.filePath() + QDir::separator() + executableFileName;
                        QFileInfo info(fName);
                        if (info.exists() && info.isFile()) {
                            QString path = QDir::toNativeSeparators(fName);
                            lineEdit->setText(path);
                            lineEdit->setModified(false);
                            externalToolsInfo[item->text(0)].path = path;
                            QToolButton* clearToolPathButton = itemWid->findChild<QToolButton*>("ClearToolPathButton");
                            assert(clearToolPathButton);
                            clearToolPathButton->setEnabled(true);

                            QString toolName = item->text(0);
                            toolNames << toolName;
                            toolPaths.insert(toolName, path);
                            fileNotFound = false;
                        }
                    }
                }
            }
        }

        if (!isPathValid) {
            QMessageBox::warning(this, L10N::warningTitle(),
                                            tr("Not a valid external tools folder"),
                                            QMessageBox::Ok);
        }
        if (!toolNames.isEmpty()) {
            ExternalToolManager* etManager = AppContext::getExternalToolRegistry()->getManager();
            ExternalToolValidationListener* listener = new ExternalToolValidationListener(toolNames);
            connect(listener, SIGNAL(si_validationComplete()), SLOT(sl_validationComplete()));
            etManager->validate(toolNames, toolPaths, listener);
        }
    }
}

////////////////////////////////////////
//PathLineEdit
void PathLineEdit::sl_onBrowse() {
    LastUsedDirHelper lod(type);

    QString name;
    if (text().isEmpty()) {
        lod.url = name = U2FileDialog::getOpenFileName(NULL, tr("Select a file"), lod.dir, FileFilter, 0, QFileDialog::DontConfirmOverwrite);
    } else {
        lod.url = name = U2FileDialog::getOpenFileName(NULL, tr("Select a file"), text(), FileFilter, 0, QFileDialog::DontConfirmOverwrite);
    }
    if (!name.isEmpty()) {
        setText(QDir::toNativeSeparators(name));
        setModified(true);
        emit editingFinished();
    }
    QToolButton* clearToolPathButton = this->parentWidget()->findChild<QToolButton*>("ClearToolPathButton");
    assert(clearToolPathButton);
    clearToolPathButton->setEnabled(!text().isEmpty());
    setFocus();
}

void PathLineEdit::sl_clear(){
    QToolButton* s = qobject_cast<QToolButton*>(sender());
    assert(s);
    setText("");
    s->setEnabled(false);
    setModified(true);
    emit editingFinished();
}

} //namespace

