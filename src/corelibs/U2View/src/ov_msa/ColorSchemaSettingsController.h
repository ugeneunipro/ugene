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

#pragma once

#include <U2Algorithm/MsaColorScheme.h>

#include <U2Gui/AppSettingsGUI.h>

namespace U2 {

const QString ColorSchemaSettingsPageId = "ColorSchemaSettings";

class U2VIEW_EXPORT ColorSchemaSettingsPageController : public AppSettingsGUIPageController {
    Q_OBJECT
public:
    ColorSchemaSettingsPageController(MsaColorSchemeRegistry* mcsr, QObject* p = nullptr);

    AppSettingsGUIPageState* getSavedState() override;

    void saveState(AppSettingsGUIPageState* s) override;

    AppSettingsGUIPageWidget* createWidget(AppSettingsGUIPageState* data) override;

    const QString& getHelpPageId() const override {
        return helpPageId;
    };

signals:
    void si_customSettingsChanged();

private:
    static const QString helpPageId;
};

}  // namespace U2
