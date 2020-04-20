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

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AppResources.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Gui/AppSettingsGUI.h>
#include "Bowtie2SettingsWidget.h"
#include "Bowtie2Task.h"
#include "Bowtie2Support.h"

namespace U2 {

// Bowtie2SettingsWidget

Bowtie2SettingsWidget::Bowtie2SettingsWidget(QWidget *parent):
    DnaAssemblyAlgorithmMainWidget(parent)
{
    setupUi(this);
    layout()->setContentsMargins(0,0,0,0);

    threadsSpinBox->setMaximum(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
    threadsSpinBox->setValue(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
#ifdef Q_OS_WIN
    threadsSpinBox->setValue(1);
    threadsLabel->setVisible(false);
    threadsSpinBox->setVisible(false);
#endif
    indexSuffixes << Bowtie2Task::indexSuffixes;
    indexSuffixes << Bowtie2Task::largeIndexSuffixes;

    requiredExtToolIds << Bowtie2Support::ET_BOWTIE2_ALIGN_ID << Bowtie2Support::ET_BOWTIE2_BUILD_ID;
}

QMap<QString,QVariant> Bowtie2SettingsWidget::getDnaAssemblyCustomSettings() const {
    QMap<QString, QVariant> settings;

    switch(modeComboBox->currentIndex()) {
    case 0:
        settings.insert(Bowtie2Task::OPTION_MODE, "--end-to-end");
        break;
    case 1:
        settings.insert(Bowtie2Task::OPTION_MODE, "--local");
    }

    settings.insert(Bowtie2Task::OPTION_MISMATCHES, mismatchesSpinBox->value());

    if(seedlenCheckBox->isChecked()) {
        settings.insert(Bowtie2Task::OPTION_SEED_LEN, seedlenSpinBox->value());
    }

    if (dpadCheckBox->isChecked()) {
        settings.insert(Bowtie2Task::OPTION_DPAD, dpadSpinBox->value());
    }

    if (gbarCheckBox->isChecked()) {
        settings.insert(Bowtie2Task::OPTION_GBAR, gbarSpinBox->value());
    }

    if(seedCheckBox->isChecked()) {
        settings.insert(Bowtie2Task::OPTION_SEED, seedSpinBox->value());
    }

    settings.insert(Bowtie2Task::OPTION_THREADS, threadsSpinBox->value());

    settings.insert(Bowtie2Task::OPTION_NOMIXED, nomixedCheckBox->isChecked());
    settings.insert(Bowtie2Task::OPTION_NODISCORDANT, nodiscordantCheckBox->isChecked());
    settings.insert(Bowtie2Task::OPTION_NOFW, nofwCheckBox->isChecked());
    settings.insert(Bowtie2Task::OPTION_NORC, norcCheckBox->isChecked());
    settings.insert(Bowtie2Task::OPTION_NOOVERLAP, nooverlapCheckBox->isChecked());
    settings.insert(Bowtie2Task::OPTION_NOCONTAIN, nocontainCheckBox->isChecked());

    return settings;
}

bool Bowtie2SettingsWidget::isValidIndex(const QString &oneIndexFileUrl) const {
    QStringList suffixes;
    suffixes << Bowtie2Task::indexSuffixes;
    suffixes << Bowtie2Task::largeIndexSuffixes;

    QString baseUrl = DnaAssemblyToReferenceTask::getBaseUrl(oneIndexFileUrl, suffixes);
    bool index = DnaAssemblyToReferenceTask::isPrebuiltIndex(baseUrl, Bowtie2Task::indexSuffixes);
    bool largeIndex = DnaAssemblyToReferenceTask::isPrebuiltIndex(baseUrl, Bowtie2Task::largeIndexSuffixes);
    return index || largeIndex;
}

// Bowtie2GUIExtensionsFactory

DnaAssemblyAlgorithmMainWidget *Bowtie2GUIExtensionsFactory::createMainWidget(QWidget *parent) {
    return new Bowtie2SettingsWidget(parent);
}

DnaAssemblyAlgorithmBuildIndexWidget *Bowtie2GUIExtensionsFactory::createBuildIndexWidget(QWidget *parent) {
    return new Bowtie2BuildSettingsWidget(parent);
}

bool Bowtie2GUIExtensionsFactory::hasMainWidget() {
    return true;
}

bool Bowtie2GUIExtensionsFactory::hasBuildIndexWidget() {
    return true;
}

} //namespace
