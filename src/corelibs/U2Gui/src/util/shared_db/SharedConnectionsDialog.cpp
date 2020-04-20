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

#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>

#include <U2Core/AddDocumentTask.h>
#include <U2Core/AppContext.h>
#include <U2Core/ConnectSharedDatabaseTask.h>
#include <U2Core/Counter.h>
#include <U2Core/L10n.h>
#include <U2Core/PasswordStorage.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Settings.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/Version.h>

#include <U2Formats/MysqlDbiUtils.h>
#include <U2Formats/MysqlUpgradeTask.h>

#include <U2Gui/AuthenticationDialog.h>
#include <U2Gui/HelpButton.h>
#include <U2Core/QObjectScopedPointer.h>

#include "EditConnectionDialog.h"
#include "SharedConnectionsDialog.h"
#include "ui_SharedConnectionsDialog.h"


static const char * NON_INITED_DB_MB_TITLE = "UGENE Shared Database Setup";
static const char * NON_INITED_DB_MB_TEXT = "UGENE has detected that the database "
    "you are connecting to is not initialized. Do you want to set it up now for use with UGENE?";

static const char * UNABLE_TO_CONNECT_TITLE = "Could Not Connect";
static const char * UNABLE_TO_CONNECT_TEXT = "The database has been set up "
    "for a more recent version of UGENE, this means that this version of UGENE is not compatible "
    "with the database and will not connect to it. Upgrade UGENE to at least %1 version "
    "to make use of the database";

static const char * DATABASE_UPGRADE_TITLE = "Database Upgrade";
static const char * DATABASE_UPGRADE_TEXT = "The database you are trying to connect to was created by an older UGENE version. "
        "It has to be upgraded to be compatible with your current UGENE version. You may need administration privileges to perform "
        "the upgrade. Note that after it has been completed previous UGENE versions may not be able to work with the database.";

static const char * DATABASE_UPGRADE_ERROR_TITLE = "Database Upgrade Error";
static const char * DATABASE_UPGRADE_ERROR_TEXT = "UGENE has failed to upgrade the database. Probably, you don't have enough permissions."
        "\n\n"
        "An error message:"
        "\n";

static const char * CONNECTION_DUPLICATE_TITLE = "Connection Duplicate Detected";
static const char * CONNECTION_DUPLICATE_TEXT = "You already have a connection to the database that you have specified. "
    "Existing connection name is \"%1\"";

namespace U2 {

const QString SharedConnectionsDialog::SETTINGS_RECENT = "/shared_database/recent_connections/";

SharedConnectionsDialog::SharedConnectionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui_SharedConnectionsDialog)
{
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "24749084");

    init();
    connectSignals();
    updateState();
}

SharedConnectionsDialog::~SharedConnectionsDialog() {
    delete ui;
}

void SharedConnectionsDialog::sl_selectionChanged() {
    updateState();
}

void SharedConnectionsDialog::sl_itemDoubleClicked(const QModelIndex& index) {
    ui->lwConnections->setCurrentIndex(index);
    if (!isConnected(ui->lwConnections->currentItem())) {
        sl_connectClicked();
    }
}

void SharedConnectionsDialog::sl_connectClicked() {
    QListWidgetItem *selectedItem = ui->lwConnections->currentItem();
    SAFE_POINT(NULL != selectedItem, "Invalid list item detected", );
    const QString connectionName = selectedItem->data(Qt::DisplayRole).toString();
    QString fullDbiUrl = getCurrentFullDbiUrl();

    countConnectionsToPublicDatabase(fullDbiUrl);

    if (!AppContext::getPasswordStorage()->contains(fullDbiUrl) && !askCredentials(fullDbiUrl)) {
        return;
    }

    // TODO: don't forget to change this when new DB providers will be introduced
    const U2DbiRef dbiRef(MYSQL_DBI_ID, fullDbiUrl);

    bool initializeDb = false;
    bool isInitialized = checkDatabaseAvailability(dbiRef, initializeDb);
    if (!isInitialized && !initializeDb) {
        return;
    }
    
    Task *dbLoadTask = new AddDocumentTask(new ConnectSharedDatabaseTask(dbiRef, connectionName, initializeDb));
    connect(dbLoadTask, SIGNAL(si_stateChanged()), SLOT(sl_connectionComplete()));
    connectionTasks.insert(ui->lwConnections->currentItem(), dbLoadTask);
    AppContext::getTaskScheduler()->registerTopLevelTask(dbLoadTask);

    accept();
}

void SharedConnectionsDialog::sl_disconnectClicked() {
    const QString dbiUrl = ui->lwConnections->currentItem()->data(UrlRole).toString();
    const QString userName = ui->lwConnections->currentItem()->data(LoginRole).toString();
    const QString fullDbiUrl = U2DbiUtils::createFullDbiUrl(userName, dbiUrl);

    cancelConnection(ui->lwConnections->currentItem());

    Document* doc = AppContext::getProject()->findDocumentByURL(fullDbiUrl);
    if (NULL != doc) {
        AppContext::getProject()->removeDocument(doc);
    }

    updateState();
}

void SharedConnectionsDialog::sl_editClicked() {
    const QString dbiUrl = ui->lwConnections->currentItem()->data(UrlRole).toString();
    const QString userName = ui->lwConnections->currentItem()->data(LoginRole).toString();
    const QString connectionName = ui->lwConnections->currentItem()->text();

    QObjectScopedPointer<EditConnectionDialog> editDialog = new EditConnectionDialog(this, dbiUrl, userName, connectionName);
    editDialog->setReadOnly(U2DbiUtils::PUBLIC_DATABASE_URL == U2DbiUtils::createFullDbiUrl(userName, dbiUrl));
    const int dialogResult = editDialog->exec();
    CHECK(!editDialog.isNull(), );

    if (QDialog::Accepted == dialogResult) {
        QListWidgetItem* item = ui->lwConnections->currentItem();
        const QString login = editDialog->getUserName();
        const QString shortDbUrl = editDialog->getShortDbiUrl();

        checkDbConnectionDuplicate(shortDbUrl, login, item->data(Qt::DisplayRole).toString());

        if (connectionName != editDialog->getName()) {
            removeRecentConnection(item);
        }

        item->setText(editDialog->getName());
        item->setData(UrlRole, shortDbUrl);
        item->setData(LoginRole, login);

        connectionTasks.remove(item);
        findUpgradeTasks();

        saveRecentConnection(item);
        updateState();
    }
}

void SharedConnectionsDialog::checkDbConnectionDuplicate(const QString &shortDbiUrl, const QString &userName,
    const QString &allowedDuplicateName)
{
    QString existingConnectionName;
    if (alreadyExists(shortDbiUrl, userName, existingConnectionName)
        && (allowedDuplicateName.isEmpty() || existingConnectionName != allowedDuplicateName))
    {
        QMessageBox::information(this, tr(CONNECTION_DUPLICATE_TITLE), tr(CONNECTION_DUPLICATE_TEXT).arg(existingConnectionName));
    }
}

void SharedConnectionsDialog::sl_addClicked() {
    QObjectScopedPointer<EditConnectionDialog> editDialog = new EditConnectionDialog(this);
    const int dialogResult = editDialog->exec();
    CHECK(!editDialog.isNull(), );

    if (QDialog::Accepted == dialogResult) {
        checkDbConnectionDuplicate(editDialog->getShortDbiUrl(), editDialog->getUserName());
        QListWidgetItem* item = insertConnection(editDialog->getName(), editDialog->getShortDbiUrl(), editDialog->getUserName());
        CHECK(NULL != item, );
        ui->lwConnections->setCurrentItem(item);
        saveRecentConnection(item);
        findUpgradeTasks();
        updateState();
    }
}

void SharedConnectionsDialog::sl_deleteClicked() {
    AppContext::getPasswordStorage()->setRemembered(getCurrentFullDbiUrl(), false);

    int index = ui->lwConnections->currentRow();
    QListWidgetItem* item = ui->lwConnections->takeItem(index);
    ui->lwConnections->setCurrentRow(qMin<int>(index, ui->lwConnections->count() - 1));

    cancelConnection(item);

    removeRecentConnection(item);
    connectionTasks.remove(item);
    upgradeTasks.remove(item);
    delete item;

    updateState();
}

void SharedConnectionsDialog::sl_connectionComplete() {
    AddDocumentTask* task = qobject_cast<AddDocumentTask*>(sender());
    if (!task->isFinished()) {
        return;
    }

    connectionTasks.remove(connectionTasks.key(task, NULL));
    updateState();
    emit si_connectionCompleted();
}

void SharedConnectionsDialog::sl_upgradeComplete(Task *upgradeTask) {
    SAFE_POINT(NULL != upgradeTask, L10N::nullPointerError("upgradeTask"), );

    upgradeTasks.remove(upgradeTasks.key(upgradeTask, NULL));
    updateState();

    if (upgradeTask->hasError()) {
        QMessageBox::critical(this,
                              tr(DATABASE_UPGRADE_ERROR_TITLE),
                              tr(DATABASE_UPGRADE_ERROR_TEXT) + upgradeTask->getError());
        coreLog.details(tr("Can't upgrade the shared database: ") + upgradeTask->getError());
    }
}

void SharedConnectionsDialog::init() {
    restoreRecentConnections();
    saveRecentConnections();
    findUpgradeTasks();
}

void SharedConnectionsDialog::connectSignals() {
    connect(ui->lwConnections,  SIGNAL(itemSelectionChanged()),     SLOT(sl_selectionChanged()));
    connect(ui->lwConnections,  SIGNAL(doubleClicked(QModelIndex)), SLOT(sl_itemDoubleClicked(QModelIndex)));

    connect(ui->pbConnect,      SIGNAL(clicked()),      SLOT(sl_connectClicked()));
    connect(ui->pbDisconnect,   SIGNAL(clicked()),      SLOT(sl_disconnectClicked()));
    connect(ui->pbEdit,         SIGNAL(clicked()),      SLOT(sl_editClicked()));
    connect(ui->pbAdd,          SIGNAL(clicked()),      SLOT(sl_addClicked()));
    connect(ui->pbDelete,       SIGNAL(clicked()),      SLOT(sl_deleteClicked()));
}

void SharedConnectionsDialog::updateState() {
    updateButtonsState();
    updateConnectionsState();
}

void SharedConnectionsDialog::updateButtonsState() {
    QListWidgetItem* currentItem = ui->lwConnections->currentItem();
    const bool isSomethingSelected = (NULL != currentItem);
    const bool isCurrentConnected = isConnected(currentItem);
    const bool isCurrentUpgradedNow = upgradeTasks.contains(currentItem);

    ui->pbDelete->setEnabled(isSomethingSelected);
    ui->pbEdit->setEnabled(isSomethingSelected && !isCurrentConnected);
    ui->pbConnect->setEnabled(isSomethingSelected && !isCurrentConnected && !isCurrentUpgradedNow);
    ui->pbDisconnect->setEnabled(isSomethingSelected && isCurrentConnected);
}

void SharedConnectionsDialog::updateConnectionsState() {
    for (int i = 0; i < ui->lwConnections->count(); i++) {
        QListWidgetItem* item = ui->lwConnections->item(i);
        updateItemIcon(item, isConnected(item));
        setUpgradedMark(item, upgradeTasks.contains(item));
    }
}

void SharedConnectionsDialog::updateItemIcon(QListWidgetItem* item, bool isConnected) {
    QPixmap px(":/core/images/db/database_lightning.png");
    if (!isConnected) {
        px.fill(Qt::transparent);
    }
    item->setIcon(QIcon(px));
}

bool SharedConnectionsDialog::askCredentials(QString &fullDbiUrl) {
    QString userName;
    const QString shortDbiUrl = U2DbiUtils::full2shortDbiUrl(fullDbiUrl, userName);

    QObjectScopedPointer<AuthenticationDialog> authenticationDialog = new AuthenticationDialog(tr("Connect to the database %1").arg(shortDbiUrl), this);
    authenticationDialog->setLogin(userName);
    const int dialogResult = authenticationDialog->exec();
    CHECK(!authenticationDialog.isNull(), false);

    if (QDialog::Accepted == dialogResult) {
        fullDbiUrl = U2DbiUtils::createFullDbiUrl(authenticationDialog->getLogin(), shortDbiUrl);
        AppContext::getPasswordStorage()->addEntry(fullDbiUrl, authenticationDialog->getPassword(), authenticationDialog->isRemembered());
        return true;
    } else {
        return false;
    }
}

void SharedConnectionsDialog::restoreRecentConnections() {
    const QStringList recentList = AppContext::getSettings()->getAllKeys(SETTINGS_RECENT);
    foreach (const QString& recent, recentList) {
        const QString fullDbiUrl = AppContext::getSettings()->getValue(SETTINGS_RECENT + recent).toString();
        insertConnection(recent, fullDbiUrl);
    }
}

void SharedConnectionsDialog::removeRecentConnection(const QListWidgetItem* item) const {
    AppContext::getSettings()->remove(SETTINGS_RECENT + item->text());
}

void SharedConnectionsDialog::saveRecentConnection(const QListWidgetItem* item) const {
    AppContext::getSettings()->setValue(SETTINGS_RECENT + item->text(), getFullDbiUrl(item));
}

void SharedConnectionsDialog::saveRecentConnections() const {
    AppContext::getSettings()->cleanSection(SETTINGS_RECENT);

    for (int i = 0; i < ui->lwConnections->count(); i++) {
        const QListWidgetItem* item = ui->lwConnections->item(i);
        saveRecentConnection(item);
    }
}

bool SharedConnectionsDialog::checkDatabaseAvailability(const U2DbiRef &ref, bool &initializationRequired) {
    CHECK(checkDbInitializationState(ref, initializationRequired), false);
    CHECK(checkDbIsTooNew(ref), false);
    CHECK(checkDbShouldBeUpgraded(ref), false);

    return true;
}

bool SharedConnectionsDialog::isConnected(QListWidgetItem* item) const {
    CHECK(NULL != item, false);
    CHECK(NULL != AppContext::getProject(), false);

    bool connectionIsInProcess = connectionTasks.contains(item);

    Document* connectionDoc = AppContext::getProject()->findDocumentByURL(GUrl(getFullDbiUrl(item), GUrl_Network));

    return ((NULL != connectionDoc) && (connectionDoc->isLoaded())) || connectionIsInProcess;
}

void SharedConnectionsDialog::setUpgradedMark(QListWidgetItem *item, bool isUpgraded) {
    QFont font = item->font();
    font.setBold(isUpgraded);
    item->setFont(font);
}

bool SharedConnectionsDialog::alreadyExists(const QString &dbiUrl, const QString &userName, QString &existingName) const {
    for (int i = 0; i < ui->lwConnections->count(); i++) {
        if (dbiUrl == ui->lwConnections->item(i)->data(UrlRole).toString()
         && userName == ui->lwConnections->item(i)->data(LoginRole).toString())
        {
            existingName = ui->lwConnections->item(i)->data(Qt::DisplayRole).toString();
            return true;
        }
    }
    return false;
}

QListWidgetItem *SharedConnectionsDialog::insertConnection(const QString &preferredName, const QString &fullDbiUrl) {
    QString userName;
    const QString shortDbiUrl = U2DbiUtils::full2shortDbiUrl(fullDbiUrl, userName);
    return insertConnection(preferredName, shortDbiUrl, userName);
}

QListWidgetItem* SharedConnectionsDialog::insertConnection(const QString& preferredName, const QString& dbiUrl, const QString &userName) {
    const QString name = rollName(preferredName);

    QListWidgetItem* item = new QListWidgetItem(name);
    item->setData(UrlRole, dbiUrl);
    item->setData(LoginRole, userName);
    ui->lwConnections->addItem(item);
    return item;
}

void SharedConnectionsDialog::cancelConnection(QListWidgetItem* item) {
    Task* connectionTask = connectionTasks.value(item, NULL);
    if (NULL != connectionTask) {
        connectionTask->cancel();
        connectionTasks.remove(item);
    }
}

QString SharedConnectionsDialog::rollName(const QString &preferredName) const {
    QString name = preferredName;
    int i = 0;
    while (!ui->lwConnections->findItems(name, Qt::MatchExactly).isEmpty()) {
        i++;
        name = preferredName + " (" + QString::number(i) + ")";
    }
    return name;
}

void SharedConnectionsDialog::countConnectionsToPublicDatabase(const QString &dbiUrl) {
    if (U2DbiUtils::PUBLIC_DATABASE_URL == dbiUrl) {
        GCOUNTER(cvar, tvar, "Connections to public database");
    }
}

QString SharedConnectionsDialog::getCurrentFullDbiUrl() const {
    return getFullDbiUrl(ui->lwConnections->currentItem());
}

QString SharedConnectionsDialog::getFullDbiUrl(const QListWidgetItem *item) const {
    return U2DbiUtils::createFullDbiUrl(item->data(LoginRole).toString(), item->data(UrlRole).toString());
}

void SharedConnectionsDialog::findUpgradeTasks() {
    upgradeTasks.clear();
    const QList<Task *> tasks = AppContext::getTaskScheduler()->getTopLevelTasks();
    foreach (Task *task, tasks) {
        MysqlUpgradeTask *upgradeTask = qobject_cast<MysqlUpgradeTask *>(task);
        if (NULL != upgradeTask) {
            const QString dbiUrl = U2DbiUtils::ref2Url(upgradeTask->getDbiRef());
            QListWidgetItem *item = findItemByDbiUrl(dbiUrl);
            if (NULL != item) {
                upgradeTasks.insert(item, upgradeTask);
            }
        }
    }
}

QListWidgetItem *SharedConnectionsDialog::findItemByDbiUrl(const QString &dbiUrl) const {
    for (int i = 0; i < ui->lwConnections->count(); i++) {
        QListWidgetItem *item = ui->lwConnections->item(i);
        if (dbiUrl == item->data(UrlRole)) {
            return item;
        }
    }
    return NULL;
}

bool SharedConnectionsDialog::checkDbInitializationState(const U2DbiRef &ref, bool &initializationRequired) {
    U2OpStatusImpl os;
    const bool dbInitialized = MysqlDbiUtils::isDbInitialized(ref, os);
    if (os.isCoR()) {
        QMessageBox::critical(this,
                              tr("Connection Error"),
                              tr("Unable to connect to the database:\n"
                                 "check connection settings"));
        coreLog.details(tr("Cannot connect to the shared database: ") + os.getError());
        return false;
    }

    if (!dbInitialized) {
        int userInput = QMessageBox::question(this, tr(NON_INITED_DB_MB_TITLE),
            tr(NON_INITED_DB_MB_TEXT), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        QMessageBox::StandardButton answer = static_cast<QMessageBox::StandardButton>(userInput);
        switch (answer) {
        case QMessageBox::No :
            initializationRequired = false;
            break;
        case QMessageBox::Yes :
            initializationRequired = true;
            break;
        default:
            FAIL("Unexpected user answer detected!", false);
        }
        return false;
    }
    return true;
}

bool SharedConnectionsDialog::checkDbIsTooNew(const U2DbiRef &ref) {
    U2OpStatusImpl os;
    QString minRequiredVersion;
    const bool isDbTooNew = U2DbiUtils::isDatabaseTooNew(ref, Version::appVersion(), minRequiredVersion, os);
    SAFE_POINT_OP(os, false);
    if (isDbTooNew) {
        QMessageBox::critical(this, tr(UNABLE_TO_CONNECT_TITLE), tr(UNABLE_TO_CONNECT_TEXT).arg(minRequiredVersion));
        return false;
    }
    return true;
}

bool SharedConnectionsDialog::checkDbShouldBeUpgraded(const U2DbiRef &ref) {
    U2OpStatusImpl os;
    const bool upgradeDatabase = U2DbiUtils::isDatabaseTooOld(ref, Version::minVersionForMySQL(), os);
    CHECK_OP(os, false);

    if (upgradeDatabase) {
        QObjectScopedPointer<QMessageBox> question = new QMessageBox(QMessageBox::Question, tr(DATABASE_UPGRADE_TITLE), tr(DATABASE_UPGRADE_TEXT), QMessageBox::Ok | QMessageBox::Cancel| QMessageBox::Help, this);
        question->button(QMessageBox::Ok)->setText(tr("Upgrade"));
        HelpButton(question.data(), question->button(QMessageBox::Help), "24749084");
        question->setDefaultButton(QMessageBox::Cancel);
        const int dialogResult = question->exec();
        CHECK(!question.isNull(), true);

        if (QMessageBox::Ok == dialogResult) {
            MysqlUpgradeTask *upgradeTask = new MysqlUpgradeTask(ref);
            upgradeTasks.insert(ui->lwConnections->currentItem(), upgradeTask);
            connect(new TaskSignalMapper(upgradeTask), SIGNAL(si_taskFinished(Task *)), SLOT(sl_upgradeComplete(Task *)));
            AppContext::getTaskScheduler()->registerTopLevelTask(upgradeTask);
        }
        return false;
    }

    return true;
}

}   // namespace U2
