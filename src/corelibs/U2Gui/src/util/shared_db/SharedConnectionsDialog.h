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

#ifndef _U2_SHARED_CONNECTIONS_DIALOG_H_
#define _U2_SHARED_CONNECTIONS_DIALOG_H_

#include <QDialog>
#include <QMap>

#include <U2Core/global.h>

class QListWidgetItem;
class QModelIndex;
class Ui_SharedConnectionsDialog;

namespace U2 {

class Task;
class U2DbiRef;

class U2GUI_EXPORT SharedConnectionsDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(SharedConnectionsDialog)
public:
    explicit SharedConnectionsDialog(QWidget *parent = 0);
    ~SharedConnectionsDialog();

signals:
    void si_connectionCompleted();
    
private slots:
    void sl_selectionChanged();
    void sl_itemDoubleClicked(const QModelIndex& index);
    void sl_connectClicked();
    void sl_disconnectClicked();
    void sl_editClicked();
    void sl_addClicked();
    void sl_deleteClicked();
    void sl_connectionComplete();
    void sl_upgradeComplete(Task *upgradeTask);

private:
    void init();
    void connectSignals();
    bool askCredentials(QString &dbiUrl);

    void updateState();
    void updateButtonsState();
    void updateConnectionsState();
    void updateItemIcon(QListWidgetItem* item, bool isConnected);

    void restoreRecentConnections();
    void removeRecentConnection(const QListWidgetItem *item) const;
    void saveRecentConnection(const QListWidgetItem *item) const;
    void saveRecentConnections() const;

    bool checkDatabaseAvailability(const U2DbiRef &ref, bool &initializationRequired);
    bool isConnected(QListWidgetItem *item) const;
    void setUpgradedMark(QListWidgetItem *item, bool isUpgraded);
    bool alreadyExists(const QString& dbiUrl, const QString &userName, QString &existingName) const;
    QListWidgetItem* insertConnection(const QString& preferredName, const QString& fullDbiUrl);
    QListWidgetItem* insertConnection(const QString& preferredName, const QString& dbiUrl, const QString &userName);
    void cancelConnection(QListWidgetItem* item);
    QString rollName(const QString& preferredName) const;
    void countConnectionsToPublicDatabase(const QString &dbiUrl);
    QString getCurrentFullDbiUrl() const;
    QString getFullDbiUrl(const QListWidgetItem *item) const;
    void findUpgradeTasks();
    QListWidgetItem *findItemByDbiUrl(const QString &dbiUrl) const;

    bool checkDbInitializationState(const U2DbiRef &ref, bool &initializationRequired);
    bool checkDbIsTooNew(const U2DbiRef &ref);
    bool checkDbShouldBeUpgraded(const U2DbiRef &ref);
    void checkDbConnectionDuplicate(const QString &shortDbiUrl, const QString &userName, const QString &allowedDuplicateName = QString());

    Ui_SharedConnectionsDialog *ui;
    QMap<QListWidgetItem*, Task*> connectionTasks;
    QMap<QListWidgetItem*, Task*> upgradeTasks;

    static const QString SETTINGS_RECENT; // TODO: move this identifier to another class, since it is to be used
                                          // not only within the dialog. After that search through the code,
                                          // where its literal value is used and replace it with the identifier.
    static const int UrlRole = Qt::UserRole;
    static const int LoginRole = UrlRole + 1;
};

}   // namespace U2

#endif // _U2_SHARED_CONNECTIONS_DIALOG_H_
