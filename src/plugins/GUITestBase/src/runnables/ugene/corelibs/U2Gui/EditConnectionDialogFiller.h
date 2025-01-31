/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class EditConnectionDialogFiller : public Filler {
public:
    class Parameters {
    public:
        Parameters()
            : connectionName(""),
              host(""),
              port(""),
              database(""),
              login(""),
              password(""),
              rememberMe(false),
              accept(true),
              checkDefaults(false) {
        }

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

    enum ConnectionType { FROM_SETTINGS,
                          MANUAL };
    EditConnectionDialogFiller(const Parameters& parameters, ConnectionType type);
    EditConnectionDialogFiller(CustomScenario* scenario);
    void commonScenario() override;

private:
    Parameters parameters;
};

class AuthenticationDialogFiller : public Filler {
public:
    AuthenticationDialogFiller(const QString& login, const QString& password);
    void commonScenario() override;

private:
    QString login;
    QString password;
};

}  // namespace U2
