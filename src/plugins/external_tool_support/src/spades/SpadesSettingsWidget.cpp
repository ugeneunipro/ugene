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

#include <QFile>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AppResources.h>
#include <U2Core/L10n.h>

#include <U2Gui/U2WidgetStateStorage.h>

#include "SpadesSettingsWidget.h"
#include "SpadesTask.h"


namespace U2 {
// SpadesSettingsWidget

SpadesSettingsWidget::SpadesSettingsWidget(QWidget *parent)
    :GenomeAssemblyAlgorithmMainWidget(parent), U2SavableWidget(this)
{
    setupUi(this);
    layout()->setContentsMargins(0,0,0,0);

    numThreadsSpinbox->setMaximum(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());
    numThreadsSpinbox->setValue(AppContext::getAppSettings()->getAppResourcePool()->getIdealThreadCount());

    U2WidgetStateStorage::restoreWidgetState(*this);
}

QMap<QString,QVariant> SpadesSettingsWidget::getGenomeAssemblyCustomSettings() {
    QMap<QString, QVariant> settings;

    settings.insert(SpadesTask::OPTION_DATASET_TYPE, typeCombo->currentText());
    settings.insert(SpadesTask::OPTION_RUNNING_MODE, modeCombo->currentText());
    settings.insert(SpadesTask::OPTION_K_MER, kmerEdit->text());
    settings.insert(SpadesTask::OPTION_THREADS, numThreadsSpinbox->value());
    settings.insert(SpadesTask::OPTION_MEMLIMIT, memlimitSpin->value());

    U2WidgetStateStorage::saveWidgetState(*this);
    widgetStateSaved = true;

    return settings;
}

bool SpadesSettingsWidget::isParametersOk(QString &) {
    return true;
}

// SpadesGUIExtensionsFactory
GenomeAssemblyAlgorithmMainWidget *SpadesGUIExtensionsFactory::createMainWidget(QWidget *parent) {
    return new SpadesSettingsWidget(parent);
}


bool SpadesGUIExtensionsFactory::hasMainWidget() {
    return true;
}

} //namespace

