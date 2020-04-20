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

#ifndef _U2_BOWTIE2_SETTINGS_WIDGET_H
#define _U2_BOWTIE2_SETTINGS_WIDGET_H

#include "ui_Bowtie2Settings.h"
#include <U2View/DnaAssemblyGUIExtension.h>

namespace U2 {

class Bowtie2SettingsWidget : public DnaAssemblyAlgorithmMainWidget, Ui_Bowtie2Settings {
    Q_OBJECT
public:
    Bowtie2SettingsWidget(QWidget *parent);
    QMap<QString, QVariant> getDnaAssemblyCustomSettings() const;
    bool isValidIndex(const QString &oneFileUrl) const;
};

class Bowtie2BuildSettingsWidget : public DnaAssemblyAlgorithmBuildIndexWidget {
    Q_OBJECT
public:
    Bowtie2BuildSettingsWidget(QWidget *parent) : DnaAssemblyAlgorithmBuildIndexWidget(parent) {}
    virtual QMap<QString,QVariant> getBuildIndexCustomSettings() {return QMap<QString, QVariant>();}
    virtual QString getIndexFileExtension() { return QString(); }
    virtual GUrl buildIndexUrl(const GUrl& /*url*/) {return GUrl();}
};

class Bowtie2GUIExtensionsFactory : public DnaAssemblyGUIExtensionsFactory {
    DnaAssemblyAlgorithmMainWidget *createMainWidget(QWidget *parent);
    DnaAssemblyAlgorithmBuildIndexWidget *createBuildIndexWidget(QWidget *parent);
    bool hasMainWidget();
    bool hasBuildIndexWidget();
};

} // namespace U2

#endif // _U2_BOWTIE2_SETTINGS_WIDGET_H
