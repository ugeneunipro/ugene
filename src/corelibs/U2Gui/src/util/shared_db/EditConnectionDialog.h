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

#ifndef _U2_EDIT_CONNECTION_DIALOG_H_
#define _U2_EDIT_CONNECTION_DIALOG_H_

#include <QDialog>

#include <U2Core/U2DbiUtils.h>

class Ui_EditConnectionDialog;

namespace U2 {

class AuthenticationWidget;

class U2GUI_EXPORT EditConnectionDialog : public QDialog {
    Q_DISABLE_COPY(EditConnectionDialog)
    Q_OBJECT
public:
    EditConnectionDialog(QWidget *parent = 0, const QString &dbiUrl = "", const QString &userName = "", const QString &connectionName = "");
    ~EditConnectionDialog();

    void        setReadOnly(bool readOnly);
    QString     getName() const;
    QString     getUserName() const;
    QString     getShortDbiUrl() const;
    QString     getFullDbiUrl() const;

public slots:
    void accept();
    
private:
    void init(const QString &dbiUrl, const QString &connectionName, const QString &userName);
    void initTabOrder();
    void saveCredentials() const;
    void removeCredentials() const;
    bool checkFields();

    Ui_EditConnectionDialog *ui;

    static const QString DEFAULT_PORT;
};

}   // namespace U2

#endif // _U2_EDIT_CONNECTION_DIALOG_H_
