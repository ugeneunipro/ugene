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

#include "utils/GTUtilsDialog.h"
#include <base_dialogs/GTFileDialog.h>

namespace U2 {
using namespace HI;

class EditConnectionDialogFiller : public Filler {
public:
    class Parameters {
        public:
        Parameters():
            connectionName(""),
            host(""),
            port(""),
            database(""),
            login(""),
            password(""),
            rememberMe(false),
            accept(true),
            checkDefaults(false) {}

        QString connectionName;
        QString host;
        QString port;
        QString database;
        QString login;
        QString password;
        bool rememberMe;
        bool accept;
        bool checkDefaults;
    };


    enum ConnectionType {FROM_SETTINGS, MANUAL};
    EditConnectionDialogFiller(HI::GUITestOpStatus &os, const Parameters &parameters, ConnectionType type);
    EditConnectionDialogFiller(HI::GUITestOpStatus &os, CustomScenario *scenario);
    void commonScenario();

private:
    Parameters parameters;
};

class AuthenticationDialogFiller : public Filler {
public:
    AuthenticationDialogFiller(HI::GUITestOpStatus &os, const QString &login, const QString &password);
    void commonScenario();

private:
    QString login;
    QString password;
};

} // U2
