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

#include <QMainWindow>

#include <U2Core/AppContext.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>
#include <U2Core/QObjectScopedPointer.h>

#include "AuthenticationDialog.h"
#include "CredentialsAskerGui.h"

namespace U2 {

bool CredentialsAskerGui::askWithFixedLogin(const QString &resourceUrl) const {
    SAFE_POINT(AppContext::isGUIMode(), "Unexpected application run mode", false);

    QWidget *mainWindow = qobject_cast<QWidget *>(AppContext::getMainWindow()->getQMainWindow());

    QString userName;
    const QString shortDbiUrl = U2DbiUtils::full2shortDbiUrl(resourceUrl, userName);

    QObjectScopedPointer<AuthenticationDialog> authDialog = new AuthenticationDialog(QObject::tr("Connect to the ") + shortDbiUrl, mainWindow);
    authDialog->setLogin(userName);
    authDialog->disableLogin();

    const int dialogResult = authDialog->exec();
    CHECK(!authDialog.isNull(), false);

    if (QDialog::Accepted != dialogResult) {
        return false;
    }

    saveCredentials(resourceUrl, authDialog->getPassword(), authDialog->isRemembered());

    return true;
}

bool CredentialsAskerGui::askWithModifiableLogin(QString &resourceUrl) const {
    SAFE_POINT(AppContext::isGUIMode(), "Unexpected application run mode", false);

    QWidget *mainWindow = qobject_cast<QWidget *>(AppContext::getMainWindow()->getQMainWindow());

    QString userName;
    const QString shortDbiUrl = U2DbiUtils::full2shortDbiUrl(resourceUrl, userName);

    QObjectScopedPointer<AuthenticationDialog> authDialog = new AuthenticationDialog(QObject::tr("Connect to the ") + shortDbiUrl, mainWindow);
    authDialog->setLogin(userName);

    const int dialogResult = authDialog->exec();
    CHECK(!authDialog.isNull(), false);

    if (QDialog::Accepted != dialogResult) {
        return false;
    }

    resourceUrl = U2DbiUtils::createFullDbiUrl(authDialog->getLogin(), shortDbiUrl);
    saveCredentials(resourceUrl, authDialog->getPassword(), authDialog->isRemembered());

    return true;
}

} // namespace U2
