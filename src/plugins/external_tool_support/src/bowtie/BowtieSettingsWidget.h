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

#include "U2View/DnaAssemblyGUIExtension.h"

#include "ui_BowtieBuildSettings.h"
#include "ui_BowtieSettings.h"

namespace U2 {

class BowtieSettingsWidget : public DnaAssemblyAlgorithmMainWidget, Ui_BowtieSettings {
    Q_OBJECT
public:
    BowtieSettingsWidget(QWidget* parent);
    QMap<QString, QVariant> getDnaAssemblyCustomSettings() const override;
    bool isValidIndex(const QString& oneFileUrl) const override;
};

class BowtieBuildSettingsWidget : public DnaAssemblyAlgorithmBuildIndexWidget, Ui_BowtieBuildSettings {
    Q_OBJECT
public:
    BowtieBuildSettingsWidget(QWidget* parent);
    QMap<QString, QVariant> getBuildIndexCustomSettings() override;
    QString getIndexFileExtension() override;
    GUrl buildIndexUrl(const GUrl& url) override;
};

class BowtieGUIExtensionsFactory : public DnaAssemblyGUIExtensionsFactory {
    DnaAssemblyAlgorithmMainWidget* createMainWidget(QWidget* parent) override;
    DnaAssemblyAlgorithmBuildIndexWidget* createBuildIndexWidget(QWidget* parent) override;
    bool hasMainWidget() override;
    bool hasBuildIndexWidget() override;
};

}  // namespace U2
