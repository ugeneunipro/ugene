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

#include "ExternalToolSupportSettingsController.h"

#include <QMessageBox>
#include <QToolButton>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/CustomExternalTool.h>
#include <U2Core/FileFilters.h>
#include <U2Core/L10n.h>
#include <U2Core/Settings.h>
#include <U2Core/Theme.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ShowHideSubgroupWidget.h>

#include "ExternalToolSupportSettings.h"
#include "custom_tools/ImportCustomToolsTask.h"
#include "custom_tools/ImportExternalToolDialog.h"

namespace U2 {

static const int TOOLKIT_TYPE = QTreeWidgetItem::UserType + 1;

/////////////////////////////////////////////
////ExternalToolSupportSettingsPageController
ExternalToolSupportSettingsPageController::ExternalToolSupportSettingsPageController(QObject* p)
    : AppSettingsGUIPageController(tr("External Tools"), ExternalToolSupportSettingsPageId, p) {
}

AppSettingsGUIPageState* ExternalToolSupportSettingsPageController::getSavedState() {
    return new ExternalToolSupportSettingsPageState(AppContext::getExternalToolRegistry()->getAllEntries());
}

void ExternalToolSupportSettingsPageController::saveState(AppSettingsGUIPageState* /*state*/) {
    ExternalToolSupportSettings::saveExternalToolsToAppConfig();
}

AppSettingsGUIPageWidget* ExternalToolSupportSettingsPageController::createWidget(AppSettingsGUIPageState* state) {
    ExternalToolSupportSettingsPageWidget* r = new ExternalToolSupportSettingsPageWidget(this);
    r->setState(state);
    return r;
}

const QString& ExternalToolSupportSettingsPageController::getHelpPageId() const {
    return helpPageId;
}

const QString ExternalToolSupportSettingsPageController::helpPageId = QString("65929361");

//////////////////////////////////////////////
////ExternalToolSupportSettingsPageState

ExternalToolSupportSettingsPageState::ExternalToolSupportSettingsPageState(const QList<ExternalTool*>& ets)
    : externalTools(ets) {
}

QList<ExternalTool*> ExternalToolSupportSettingsPageState::getExternalTools() const {
    return externalTools;
}

/////////////////////////////////////////////
////ExternalToolSupportSettingsPageWidget

const QString ExternalToolSupportSettingsPageWidget::INSTALLED = QObject::tr("Installed");
const QString ExternalToolSupportSettingsPageWidget::NOT_INSTALLED = QObject::tr("Not installed");
const QString ExternalToolSupportSettingsPageWidget::ET_DOWNLOAD_INFO = QObject::tr("<html><head/><body><p>Download <a href=\"http://ugene.net/download-all.html\"><span style=\" text-decoration: underline; color:#1866af;\">tools executables</span></a> and configure the tools paths. </p></body></html>");

const QString ExternalToolSupportSettingsPageWidget::SUPPORTED_ID = "integrated tools";
const QString ExternalToolSupportSettingsPageWidget::CUSTOM_ID = "custom tools";
const QString ExternalToolSupportSettingsPageWidget::INFORMATION_ID = "info";

ExternalToolSupportSettingsPageWidget::ExternalToolSupportSettingsPageWidget(ExternalToolSupportSettingsPageController* ctrl) {
    Q_UNUSED(ctrl);

    setupUi(this);
    defaultDescriptionText = descriptionTextBrowser->toPlainText();

    selectToolPackLabel->setText(ET_DOWNLOAD_INFO);
    versionLabel->hide();
    binaryPathLabel->hide();

    supportedToolsShowHideWidget = new ShowHideSubgroupWidget(SUPPORTED_ID, tr("Supported tools"), integratedToolsInnerWidget, true);
    integratedToolsContainerWidget->layout()->addWidget(supportedToolsShowHideWidget);

    customToolsShowHideWidget = new ShowHideSubgroupWidget(CUSTOM_ID, tr("Custom tools"), customToolsInnerWidget, false);
    customToolsContainerWidget->layout()->addWidget(customToolsShowHideWidget);

    infoShowHideWidget = new ShowHideSubgroupWidget(INFORMATION_ID, tr("Additional information"), infoInnerWidget, true);
    infoContainerWidget->layout()->addWidget(infoShowHideWidget);

    twIntegratedTools->setColumnWidth(0, this->geometry().width() / 3);
    twCustomTools->setColumnWidth(0, this->geometry().width() / 3);

    twIntegratedTools->installEventFilter(this);
    twCustomTools->installEventFilter(this);

    connect(pbImport, SIGNAL(clicked()), SLOT(sl_importCustomToolButtonClicked()));
    connect(pbDelete, SIGNAL(clicked()), SLOT(sl_deleteCustomToolButtonClicked()));
    connect(selectToolPackButton, SIGNAL(clicked()), this, SLOT(sl_onBrowseToolPackPath()));
    connect(selectToolPackLabel, SIGNAL(linkActivated(QString)), this, SLOT(sl_linkActivated(QString)));
    connect(twCustomTools, SIGNAL(itemSelectionChanged()), SLOT(sl_itemSelectionChanged()));
    connect(twIntegratedTools, SIGNAL(itemSelectionChanged()), SLOT(sl_itemSelectionChanged()));

    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    connect(etRegistry, SIGNAL(si_toolAdded(const QString&)), SLOT(sl_externalToolAdded(const QString&)));
    connect(etRegistry, SIGNAL(si_toolIsAboutToBeRemoved(const QString&)), SLOT(sl_externalToolIsAboutToBeRemoved(const QString&)));
}

ExternalToolSupportSettingsPageWidget::~ExternalToolSupportSettingsPageWidget() {
    saveShowHideSubgroupsState();
}

QWidget* ExternalToolSupportSettingsPageWidget::createPathEditor(QWidget* parent, const QString& path) const {
    QWidget* widget = new QWidget(parent);

    PathLineEdit* toolPathEdit = new PathLineEdit("", "executable", false, widget);
    toolPathEdit->setObjectName("PathLineEdit");
    toolPathEdit->setFrame(false);
    toolPathEdit->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    toolPathEdit->setText(QDir::toNativeSeparators(path));

    widget->setFocusProxy(toolPathEdit);
    connect(toolPathEdit, SIGNAL(si_focusIn()), this, SLOT(sl_onPathEditWidgetClick()));
    connect(toolPathEdit, SIGNAL(editingFinished()), this, SLOT(sl_toolPathChanged()));

    QToolButton* selectToolPathButton = new QToolButton(widget);
    selectToolPathButton->setObjectName("ResetExternalTool");
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

QTreeWidgetItem* ExternalToolSupportSettingsPageWidget::createToolkitItem(QTreeWidget* treeWidget, const QString& toolkitName, const QIcon& icon) {
    QTreeWidgetItem* toolkitItem = new QTreeWidgetItem({toolkitName}, TOOLKIT_TYPE);
    toolkitItem->setData(0, Qt::ItemDataRole::UserRole, toolkitName);
    toolkitItem->setIcon(0, icon);
    treeWidget->addTopLevelItem(toolkitItem);

    // draw widget for path select button
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
    treeWidget->setItemWidget(toolkitItem, 1, widget);

    toolkitItem->setExpanded(true);
    return toolkitItem;
}

void ExternalToolSupportSettingsPageWidget::sl_onClickLink(const QUrl& url) {
    const QAbstractItemModel* model = twIntegratedTools->selectionModel()->model();
    QModelIndexList items = model->match(model->index(0, 0), Qt::DisplayRole, QVariant::fromValue(url.toEncoded()), 2, Qt::MatchRecursive);
    if (items.isEmpty()) {
        return;
    }
    twIntegratedTools->setCurrentIndex(items[0]);
}

void ExternalToolSupportSettingsPageWidget::sl_importCustomToolButtonClicked() {
    // UGENE-6553 temporary removed
    // QObjectScopedPointer<ImportExternalToolDialog> dialog = new ImportExternalToolDialog(this);
    // dialog->exec();

    LastUsedDirHelper lod("import external tool");
    QString filter = FileFilters::createFileFilter(tr("UGENE external tool config file"), {"xml"}, false);
    lod.url = U2FileDialog::getOpenFileName(this, tr("Select configuration file to import"), lod.dir, filter);
    if (!lod.url.isEmpty()) {
        AppContext::getTaskScheduler()->registerTopLevelTask(new ImportCustomToolsTask(QDir::toNativeSeparators(lod.url)));
    }
}

void ExternalToolSupportSettingsPageWidget::sl_deleteCustomToolButtonClicked() {
    QList<QTreeWidgetItem*> selectedItems = twCustomTools->selectedItems();
    CHECK(!selectedItems.isEmpty(), );
    const QString toolId = externalToolsItems.key(selectedItems.first());
    CHECK(!toolId.isEmpty(), );

    CustomExternalTool* tool = qobject_cast<CustomExternalTool*>(AppContext::getExternalToolRegistry()->getById(toolId));
    SAFE_POINT(nullptr != tool, "Can't get CustomExternalTool from the registry", );
    const QString configFilePath = tool->getConfigFilePath();

    AppContext::getExternalToolRegistry()->unregisterEntry(toolId);
    QFile configFile(configFilePath);
    const bool fileRemoved = configFile.remove();

    if (!fileRemoved) {
        coreLog.details(tr("Can't remove custom external tool config file from the storage folder: %1").arg(configFilePath));
    }
}

void ExternalToolSupportSettingsPageWidget::sl_externalToolAdded(const QString& id) {
    ExternalTool* tool = AppContext::getExternalToolRegistry()->getById(id);

    ExternalToolInfo info;
    info.id = id;
    info.dirName = tool->getDirName();
    info.name = tool->getName();
    info.path = tool->getPath();
    info.description = tool->getDescription();
    info.isValid = tool->isValid();
    info.version = tool->getVersion();
    info.isModule = tool->isModule();
    externalToolsInfo.insert(info.id, info);

    connect(tool, SIGNAL(si_toolValidationStatusChanged(bool)), SLOT(sl_toolValidationStatusChanged(bool)));

    QTreeWidget* treeWidget = tool->isCustom() ? twCustomTools : twIntegratedTools;
    appendToolItem(treeWidget->invisibleRootItem(), tool);
}

void ExternalToolSupportSettingsPageWidget::sl_externalToolIsAboutToBeRemoved(const QString& id) {
    externalToolsInfo.remove(id);

    ExternalTool* tool = AppContext::getExternalToolRegistry()->getById(id);
    disconnect(tool, SIGNAL(si_toolValidationStatusChanged(bool)), this, SLOT(sl_toolValidationStatusChanged(bool)));

    QTreeWidgetItem* item = externalToolsItems.value(id, nullptr);
    if (nullptr != item) {
        QTreeWidgetItem* parentItem = item->parent();
        if (nullptr == parentItem) {
            parentItem = item->treeWidget()->invisibleRootItem();
        }
        parentItem->takeChild(parentItem->indexOfChild(item));
        delete item;

        if (TOOLKIT_TYPE == parentItem->type() && 0 == parentItem->childCount()) {
            QTreeWidgetItem* grandParentItem = parentItem->treeWidget()->invisibleRootItem();
            grandParentItem->takeChild(grandParentItem->indexOfChild(parentItem));
        }
        externalToolsItems.remove(id);
    }
}

void ExternalToolSupportSettingsPageWidget::sl_linkActivated(const QString& url) {
    GUIUtils::runWebBrowser(url);
}

static void extractCustomTools(QList<QList<ExternalTool*>>& toolKitList, QList<ExternalTool*>& customTools) {
    for (QList<ExternalTool*>& toolList : toolKitList) {
        for (ExternalTool* tool : toolList) {
            if (tool->isCustom()) {
                customTools << tool;
                toolList.removeAll(tool);
            }
        }
    }
}

/** Returns master ExternalTool if tools list contains only 1 tool and 0+ modules or nullptr otherwise. */
static ExternalTool* isMasterWithModules(const QList<ExternalTool*>& toolsList) {
    ExternalTool* master = nullptr;
    for (ExternalTool* tool : qAsConst(toolsList)) {
        if (tool->isModule()) {
            continue;
        }
        if (master != nullptr) {
            return nullptr;
        }
        master = tool;
    }
    return master;
}

void ExternalToolSupportSettingsPageWidget::setState(AppSettingsGUIPageState* s) {
    ExternalToolSupportSettingsPageState* state = qobject_cast<ExternalToolSupportSettingsPageState*>(s);
    SAFE_POINT(state != nullptr, "ExternalToolSupportSettingsPageState is absent", );

    const QList<ExternalTool*> toolList = state->getExternalTools();
    for (ExternalTool* tool : qAsConst(toolList)) {
        ExternalToolInfo info;
        info.id = tool->getId();
        info.dirName = tool->getDirName();
        info.name = tool->getName();
        info.path = tool->getPath();
        info.description = tool->getDescription();
        info.isValid = tool->isValid();
        info.version = tool->getVersion();
        info.isModule = tool->isModule();
        externalToolsInfo.insert(info.id, info);

        connect(tool, SIGNAL(si_toolValidationStatusChanged(bool)), SLOT(sl_toolValidationStatusChanged(bool)));
    }

    QList<QList<ExternalTool*>> toolsPerToolkitList = AppContext::getExternalToolRegistry()->getAllEntriesSortedByToolKits();
    QList<ExternalTool*> customToolList;
    extractCustomTools(toolsPerToolkitList, customToolList);

    QTreeWidgetItem* rootItem = twIntegratedTools->invisibleRootItem();
    for (QList<ExternalTool*> toolsList : qAsConst(toolsPerToolkitList)) {
        CHECK_CONTINUE(!toolsList.isEmpty());

        if (toolsList.length() == 1) {
            appendToolItem(rootItem, toolsList[0]);
            continue;
        }
        ExternalTool* groupTool = isMasterWithModules(toolsList);
        if (groupTool != nullptr) {
            QTreeWidgetItem* masterItem = appendToolItem(rootItem, groupTool);
            masterItem->setExpanded(false);
            toolsList.removeOne(groupTool);
            for (ExternalTool* tool : qAsConst(toolsList)) {
                appendToolItem(masterItem, tool, true);
            }
        } else {
            groupTool = toolsList[0];
            QTreeWidgetItem* toolkitItem = createToolkitItem(twIntegratedTools, groupTool->getToolKitName(), groupTool->getIcon());
            for (ExternalTool* tool : qAsConst(toolsList)) {
                appendToolItem(toolkitItem, tool);
            }
        }
    }

    QTreeWidgetItem* customToolsRootItem = twCustomTools->invisibleRootItem();
    for (ExternalTool* tool : qAsConst(customToolList)) {
        appendToolItem(customToolsRootItem, tool);
    }

    Settings* settings = AppContext::getSettings();
    supportedToolsShowHideWidget->setSubgroupOpened(settings->getValue(ExternalToolSupportSettingsPageWidget::SUPPORTED_ID, true).toBool());
    customToolsShowHideWidget->setSubgroupOpened(settings->getValue(ExternalToolSupportSettingsPageWidget::CUSTOM_ID, false).toBool());
    infoShowHideWidget->setSubgroupOpened(settings->getValue(ExternalToolSupportSettingsPageWidget::INFORMATION_ID, true).toBool());
}

QTreeWidgetItem* ExternalToolSupportSettingsPageWidget::appendToolItem(QTreeWidgetItem* rootItem, const ExternalTool* tool, bool isModule) {
    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << tool->getName());
    item->setData(0, Qt::ItemDataRole::UserRole, tool->getId());

    externalToolsItems.insert(tool->getId(), item);
    rootItem->addChild(item);

    const ExternalToolInfo& toolInfo = externalToolsInfo.value(tool->getId());
    QTreeWidget* treeWidget = rootItem->treeWidget();

    QWidget* toolItemWidget = isModule ? new QLabel(toolInfo.isValid ? INSTALLED : NOT_INSTALLED)
                                       : createPathEditor(treeWidget, toolInfo.path);

    treeWidget->setItemWidget(item, 1, toolItemWidget);

    QIcon icon = toolInfo.path.isEmpty() ? tool->getGrayIcon()
                                         : (toolInfo.isValid ? tool->getIcon()
                                                             : tool->getWarnIcon());
    item->setIcon(0, icon);
    return item;
}

void ExternalToolSupportSettingsPageWidget::setToolState(ExternalTool* tool) {
    QTreeWidgetItem* item = externalToolsItems.value(tool->getId(), nullptr);
    SAFE_POINT(nullptr != item, QString("Tree item for the tool %1 not found").arg(tool->getName()), );

    externalToolsInfo[tool->getId()].isValid = tool->isValid();
    QLabel* moduleToolLabel = qobject_cast<QLabel*>(twIntegratedTools->itemWidget(item, 1));
    QString moduleToolState;
    QString toolStateDesc;

    if (tool->isValid()) {
        item->setIcon(0, AppContext::getExternalToolRegistry()->getById(tool->getId())->getIcon());
        moduleToolState = INSTALLED;
    } else if (!tool->getPath().isEmpty()) {
        toolStateDesc = getToolStateDescription(tool);
        item->setIcon(0, AppContext::getExternalToolRegistry()->getById(tool->getId())->getWarnIcon());
        moduleToolState = NOT_INSTALLED;
    } else {
        item->setIcon(0, AppContext::getExternalToolRegistry()->getById(tool->getId())->getGrayIcon());
        moduleToolState = "";
    }

    if (moduleToolLabel) {
        moduleToolLabel->setText(moduleToolState);
    }

    externalToolsInfo[tool->getId()].path = tool->getPath();
    if (!tool->getVersion().isEmpty()) {
        externalToolsInfo[tool->getId()].version = tool->getVersion();
    } else {
        externalToolsInfo[tool->getId()].version = "unknown";
    }

    QList<QTreeWidgetItem*> selectedItems = twIntegratedTools->selectedItems();
    CHECK(selectedItems.length() > 0, );
    QString selectedName = selectedItems.at(0)->text(0);

    if (selectedName == tool->getName()) {
        setDescription(tool);
    }
}

QString ExternalToolSupportSettingsPageWidget::getToolLink(const QString& toolName) const {
    return "<a href='" + toolName + "'>" + toolName + "</a>";
}

QString ExternalToolSupportSettingsPageWidget::getToolStateDescription(ExternalTool* tool) const {
    QString result;

    SAFE_POINT(tool, "Tool pointer is NULL", result);
    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    SAFE_POINT(etRegistry, "External tool registry is NULL", result);
    ExternalToolManager* etManager = etRegistry->getManager();
    SAFE_POINT(etManager, "External tool manager is NULL", result);

    ExternalToolManager::ExternalToolState state = etManager->getToolState(tool->getId());

    if (state == ExternalToolManager::NotValidByDependency) {
        QString text = tr("External tool '%1' cannot be validated as it "
                          "depends on other tools, some of which are not valid. "
                          "The list of tools is the following: ")
                           .arg(tool->getName());

        QStringList invalidDependencies;
        QStringList dependencies = tool->getDependencies();
        for (const QString& masterId : qAsConst(dependencies)) {
            if (etManager->getToolState(masterId) != ExternalToolManager::Valid) {
                QString masterName = AppContext::getExternalToolRegistry()->getToolNameById(masterId);
                if (tool->getId() != masterId && tool->getToolKitName() != masterName) {
                    invalidDependencies << getToolLink(masterName);
                } else {
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
                                                            "the '%1' module tool")
                                                        .arg(tool->getName()),
                       result);
            QString masterId = toolDependencies.first();
            QString text = tr("'%1' is %2 module and it is not installed. "
                              "Install it and restart UGENE or set another "
                              "%2 with already installed '%1' module.")
                               .arg(tool->getName())
                               .arg(AppContext::getExternalToolRegistry()->getToolNameById(masterId));

            result = warn(text) + "<br><br>";
        }

        if (tool->hasAdditionalErrorMessage()) {
            result += warn(tool->getAdditionalErrorMessage()) + "<br><br>";
        }
    }
    return result;
}

void ExternalToolSupportSettingsPageWidget::resetDescription() {
    descriptionTextBrowser->setPlainText(defaultDescriptionText);
}

void ExternalToolSupportSettingsPageWidget::setDescription(ExternalTool* tool) {
    QString desc = tr("No description");
    if (tool != nullptr) {
        desc = getToolStateDescription(tool);
        if (desc.size() == 0) {
            desc = tool->getDescription();
        } else {
            desc += tool->getDescription();
        }
        if (tool->isValid()) {
            desc += tr("<br><br>Version: ");
            if (!externalToolsInfo[tool->getId()].version.isEmpty()) {
                desc += externalToolsInfo[tool->getId()].version;
            } else {
                desc += tr("unknown");
            }
        }

        if (!externalToolsInfo[tool->getId()].path.isEmpty()) {
            desc += tr("<br><br>Binary path: ");
            desc += externalToolsInfo[tool->getId()].path;
        }
    }
    descriptionTextBrowser->setText(desc + "<a href='1'></a>");
}

QString ExternalToolSupportSettingsPageWidget::warn(const QString& text) const {
    return "<span style=\"color:" + Theme::errorColorLabelStr() + "; font:bold;\">" + text + "</span>";
}

bool ExternalToolSupportSettingsPageWidget::eventFilter(QObject* watched, QEvent* event) {
    CHECK(event->type() == QEvent::FocusIn, false);

    QTreeWidgetItem* item = nullptr;
    QList<QTreeWidgetItem*> selectedItems;
    if (watched == twIntegratedTools) {
        selectedItems = twIntegratedTools->selectedItems();
    } else if (watched == twCustomTools) {
        selectedItems = twCustomTools->selectedItems();
    }

    if (!selectedItems.isEmpty()) {
        item = selectedItems.first();
    }

    bool itemSelected = item != nullptr;
    if (itemSelected) {
        QString toolId = externalToolsItems.key(item);
        setDescription(AppContext::getExternalToolRegistry()->getById(toolId));
    } else {
        resetDescription();
    }

    return false;
}

void ExternalToolSupportSettingsPageWidget::saveShowHideSubgroupsState() const {
    Settings* settings = AppContext::getSettings();
    settings->setValue(ExternalToolSupportSettingsPageWidget::SUPPORTED_ID, QVariant(supportedToolsShowHideWidget->isSubgroupOpened()));
    settings->setValue(ExternalToolSupportSettingsPageWidget::CUSTOM_ID, QVariant(customToolsShowHideWidget->isSubgroupOpened()));
    settings->setValue(ExternalToolSupportSettingsPageWidget::INFORMATION_ID, QVariant(infoShowHideWidget->isSubgroupOpened()));
}

AppSettingsGUIPageState* ExternalToolSupportSettingsPageWidget::getState(QString& err) const {
    Q_UNUSED(err);

    QList<ExternalTool*> externalTools;
    const QList<ExternalToolInfo> toolInfoList = externalToolsInfo.values();
    for (const ExternalToolInfo& info : qAsConst(toolInfoList)) {
        auto externalTool = new ExternalTool(info.id, info.dirName, info.name, info.path);
        externalTool->setValid(info.isValid);
        externalTool->setVersion(info.version);
        externalTools.append(externalTool);
    }
    return new ExternalToolSupportSettingsPageState(externalTools);
}

void ExternalToolSupportSettingsPageWidget::sl_toolPathChanged() {
    PathLineEdit* s = qobject_cast<PathLineEdit*>(sender());

    if (!s || !s->isModified()) {
        return;
    }

    QWidget* par = s->parentWidget();
    QString path = s->text();
    s->setModified(false);

    QList<QTreeWidgetItem*> listOfItems = twIntegratedTools->findItems("", Qt::MatchContains | Qt::MatchRecursive) << twCustomTools->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    SAFE_POINT(listOfItems.length() != 0, "ExternalToolSupportSettings, NO items are selected", );

    twIntegratedTools->clearSelection();
    foreach (QTreeWidgetItem* item, listOfItems) {
        QWidget* itemWid = item->treeWidget()->itemWidget(item, 1);
        if (par == itemWid) {  // may be no good method for check QTreeWidgetItem
            emit si_setLockState(true);
            QString toolId = item->data(0, Qt::ItemDataRole::UserRole).toString();
            if (path.isEmpty()) {
                item->setIcon(0, AppContext::getExternalToolRegistry()->getById(toolId)->getGrayIcon());
            }

            ExternalToolManager* etManager = AppContext::getExternalToolRegistry()->getManager();
            SAFE_POINT(etManager != nullptr, "External tool manager is null", );

            ExternalToolValidationListener* listener = new ExternalToolValidationListener(toolId);
            connect(listener, SIGNAL(si_validationComplete()), SLOT(sl_validationComplete()));
            StrStrMap pathMap;
            pathMap[toolId] = path;
            etManager->validate(QStringList() << toolId, pathMap, listener);
        }
    }
}

void ExternalToolSupportSettingsPageWidget::sl_validationComplete() {
    ExternalToolValidationListener* listener = qobject_cast<ExternalToolValidationListener*>(sender());
    SAFE_POINT(nullptr != listener, "Unexpected message sender", );

    listener->deleteLater();

    ExternalToolRegistry* etRegistry = AppContext::getExternalToolRegistry();
    CHECK(etRegistry, );

    foreach (const QString& toolId, listener->getToolIds()) {
        ExternalTool* tool = etRegistry->getById(toolId);
        SAFE_POINT(nullptr != tool, QString("External tool %1 not found in the registry.").arg(toolId), );
        setToolState(tool);
    }
    emit si_setLockState(false);
}

void ExternalToolSupportSettingsPageWidget::sl_toolValidationStatusChanged(bool isValid) {
    Q_UNUSED(isValid);

    ExternalTool* s = qobject_cast<ExternalTool*>(sender());
    SAFE_POINT(nullptr != s, "Unexpected message sender", );

    setToolState(s);
}

void ExternalToolSupportSettingsPageWidget::sl_itemSelectionChanged() {
    QTreeWidget* treeWidget = qobject_cast<QTreeWidget*>(sender());
    QList<QTreeWidgetItem*> selectedItems = treeWidget->selectedItems();

    pbDelete->setEnabled(!twCustomTools->selectedItems().isEmpty());

    if (selectedItems.length() == 0) {
        descriptionTextBrowser->setText(tr("Select an external tool to view more information about it."));
        return;
    }
    SAFE_POINT(selectedItems.length() != 0, "ExternalToolSupportSettings, no items selected", );

    QString id = selectedItems.at(0)->data(0, Qt::ItemDataRole::UserRole).toString();

    if (selectedItems.at(0)->type() == TOOLKIT_TYPE) {
        QString text = AppContext::getExternalToolRegistry()->getToolkitDescription(id);
        if (!text.isEmpty()) {
            descriptionTextBrowser->setText(text);
            return;
        }
    }

    // no description or tool custom description
    ExternalTool* tool = AppContext::getExternalToolRegistry()->getById(id);
    setDescription(tool);
}

void ExternalToolSupportSettingsPageWidget::sl_onPathEditWidgetClick() {
    QWidget* s = qobject_cast<QWidget*>(sender());
    SAFE_POINT(s != nullptr, "Unexpected message sender", );

    QList<QTreeWidgetItem*> listOfItems = twIntegratedTools->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    listOfItems << twCustomTools->findItems("", Qt::MatchContains | Qt::MatchRecursive);
    SAFE_POINT(listOfItems.length() != 0, "No items were found in the tree", );

    twIntegratedTools->clearSelection();
    twCustomTools->clearSelection();
    foreach (QTreeWidgetItem* item, listOfItems) {
        QWidget* par = s->parentWidget();
        QWidget* itemWid = item->treeWidget()->itemWidget(item, 1);
        if (itemWid == par) {
            item->setSelected(true);
        }
    }
}

// looks in selected folder +1 level 1 subfolders
void ExternalToolSupportSettingsPageWidget::sl_onBrowseToolKitPath() {
    LastUsedDirHelper lod("toolkit path");
    QString dir;

    lod.url = dir = U2FileDialog::getExistingDirectory(this, tr("Choose Folder With Executables"), lod.dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        assert(twIntegratedTools->selectedItems().isEmpty() == 0);
        QString toolKitName = twIntegratedTools->selectedItems().first()->text(0);
        QList<QTreeWidgetItem*> listOfItems = twIntegratedTools->findItems("", Qt::MatchContains | Qt::MatchRecursive);
        assert(listOfItems.length() != 0);

        QStringList toolIds;
        StrStrMap toolPaths;
        foreach (QTreeWidgetItem* item, listOfItems) {
            if (!externalToolsItems.values().contains(item)) {
                continue;
            }
            QString itemId = item->data(0, Qt::UserRole).toString();
            ExternalTool* tool = AppContext::getExternalToolRegistry()->getById(itemId);
            if (tool != nullptr && tool->getToolKitName() == toolKitName) {
                QWidget* itemWid = twIntegratedTools->itemWidget(item, 1);
                PathLineEdit* lineEdit = itemWid->findChild<PathLineEdit*>("PathLineEdit");
                LimitedDirIterator it(dir);
                bool fileNotFound = true;
                QString executableFileName = tool->getExecutableFileName();
                while (it.hasNext() && fileNotFound) {
                    it.next();
                    QString fpath = it.filePath() + QDir::separator() + executableFileName;

                    QFileInfo info(fpath);
                    if (info.exists() && info.isFile()) {
                        QString path = QDir::toNativeSeparators(fpath);
                        lineEdit->setText(path);
                        lineEdit->setModified(false);
                        externalToolsInfo[itemId].path = path;
                        QToolButton* clearToolPathButton = itemWid->findChild<QToolButton*>("ClearToolPathButton");
                        assert(clearToolPathButton);
                        clearToolPathButton->setEnabled(true);
                        toolIds << itemId;
                        toolPaths.insert(itemId, path);
                        fileNotFound = false;
                    }
                }
            }
        }
        if (!toolIds.isEmpty()) {
            emit si_setLockState(true);
            ExternalToolManager* etManager = AppContext::getExternalToolRegistry()->getManager();
            ExternalToolValidationListener* listener = new ExternalToolValidationListener(toolIds);
            connect(listener, SIGNAL(si_validationComplete()), SLOT(sl_validationComplete()));
            etManager->validate(toolIds, toolPaths, listener);
        }
    }
}

void ExternalToolSupportSettingsPageWidget::sl_onBrowseToolPackPath() {
    LastUsedDirHelper lod("toolpack path");
    QString dirPath;
    lod.dir = dirPath = U2FileDialog::getExistingDirectory(this, tr("Select Folder With External Tools Package"), lod.dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dirPath.isEmpty()) {
        QDir dir = QDir(dirPath);
        QList<QTreeWidgetItem*> listOfItems = twIntegratedTools->findItems("", Qt::MatchContains | Qt::MatchRecursive);
        assert(listOfItems.length() != 0);
        QStringList toolIds;
        StrStrMap toolPaths;
        bool isValidExternalToolsFolder = false;

        const QList<ExternalTool*> toolList = AppContext::getExternalToolRegistry()->getAllEntries();
        for (const ExternalTool* externalTool : qAsConst(toolList)) {
            if (externalTool->isModule()) {
                continue;
            }
            QTreeWidgetItem* item = externalToolsItems.value(externalTool->getId(), nullptr);
            SAFE_POINT(item != nullptr, QString("Tree item not found for the tool %1").arg(externalTool->getName()), );

            const QStringList dirList = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            for (QString dirName : qAsConst(dirList)) {
                if (externalTool->getDirName() != dirName) {
                    continue;
                }
                isValidExternalToolsFolder = true;
                QWidget* itemWid = twIntegratedTools->itemWidget(item, 1);
                PathLineEdit* lineEdit = itemWid->findChild<PathLineEdit*>("PathLineEdit");

                LimitedDirIterator it(dirPath + "/" + dirName);
                QString executableFileName = AppContext::getExternalToolRegistry()->getById(externalTool->getId())->getExecutableFileName();
                while (it.hasNext()) {
                    it.next();
                    QString executableFilePath = it.filePath() + "/" + executableFileName;
                    if (QFileInfo(executableFilePath).isFile()) {
                        QString path = QDir::toNativeSeparators(executableFilePath);
                        lineEdit->setText(path);
                        lineEdit->setModified(false);
                        externalToolsInfo[item->data(0, Qt::UserRole).toString()].path = path;

                        QToolButton* clearToolPathButton = itemWid->findChild<QToolButton*>("ClearToolPathButton");
                        SAFE_POINT(clearToolPathButton != nullptr, "ClearToolPathButton not found!", );
                        clearToolPathButton->setEnabled(true);

                        toolIds << externalTool->getId();
                        toolPaths.insert(externalTool->getId(), path);
                        break;
                    }
                }
            }
        }

        if (!isValidExternalToolsFolder) {
            QMessageBox::warning(this, L10N::warningTitle(), tr("Not a valid external tools folder"), QMessageBox::Ok);
        }
        if (!toolIds.isEmpty()) {
            emit si_setLockState(true);
            ExternalToolManager* etManager = AppContext::getExternalToolRegistry()->getManager();
            ExternalToolValidationListener* listener = new ExternalToolValidationListener(toolIds);
            connect(listener, SIGNAL(si_validationComplete()), SLOT(sl_validationComplete()));
            etManager->validate(toolIds, toolPaths, listener);
        }
    }
}

////////////////////////////////////////
// PathLineEdit
void PathLineEdit::sl_onBrowse() {
    LastUsedDirHelper lod(type);

    QString name;
    if (text().isEmpty()) {
        lod.url = name = U2FileDialog::getOpenFileName(nullptr, tr("Select a file"), lod.dir, FileFilter, 0, QFileDialog::DontConfirmOverwrite);
    } else {
        lod.url = name = U2FileDialog::getOpenFileName(nullptr, tr("Select a file"), text(), FileFilter, 0, QFileDialog::DontConfirmOverwrite);
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

void PathLineEdit::sl_clear() {
    QToolButton* s = qobject_cast<QToolButton*>(sender());
    assert(s);
    setText("");
    s->setEnabled(false);
    setModified(true);
    emit editingFinished();
}

void PathLineEdit::focusInEvent(QFocusEvent* /*event*/) {
    emit si_focusIn();
}

}  // namespace U2
