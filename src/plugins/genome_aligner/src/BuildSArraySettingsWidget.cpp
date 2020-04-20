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

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/GUrl.h>
#include <U2Core/UserApplicationsSettings.h>
#include <U2Gui/DialogUtils.h>

#include "BuildSArraySettingsWidget.h"
#include "GenomeAlignerTask.h"

static const int MIN_PART_SIZE = 1;
static const int DEFAULT_PART_SIZE = 10;

namespace U2 {

BuildSArraySettingsWidget::BuildSArraySettingsWidget(QWidget* parent) : DnaAssemblyAlgorithmBuildIndexWidget(parent) {
	setupUi(this);
	layout()->setContentsMargins(0,0,0,0);

    connect(partSlider, SIGNAL(valueChanged(int)), SLOT(sl_onPartSliderChanged(int)));

    systemSize = AppContext::getAppSettings()->getAppResourcePool()->getMaxMemorySizeInMB();
    partSlider->setEnabled(false);

    partSizeLabel->setText(QByteArray::number(partSlider->value()) + " Mb");
    totalSizeLabel->setText(QByteArray::number(partSlider->value()*13) + " Mb");
    systemSizeLabel->setText(QByteArray::number(systemSize) + " Mb");
}

QMap<QString,QVariant> BuildSArraySettingsWidget::getBuildIndexCustomSettings() {
	QMap<QString,QVariant> settings;

    settings.insert(GenomeAlignerTask::OPTION_SEQ_PART_SIZE, partSlider->value());

	return settings;
}

QString BuildSArraySettingsWidget::getIndexFileExtension() {
	return "";
}

void BuildSArraySettingsWidget::sl_onPartSliderChanged(int value) {
    partSizeLabel->setText(QByteArray::number(value) + " Mb");
    totalSizeLabel->setText(QByteArray::number(value*13) + " Mb");
}

GUrl BuildSArraySettingsWidget::buildIndexUrl(const GUrl& url) {
    QString refUrl = url.getURLString();
    QFile file(refUrl);
    if (file.exists()) {
        int fileSize = 1 + (int)(file.size()/(1024*1024));
        int maxPartSize = qMin(fileSize*13, systemSize)/13;
        partSlider->setMinimum(MIN_PART_SIZE);
        partSlider->setMaximum(maxPartSize);
        partSlider->setEnabled(true);
        partSlider->setValue(qMin(maxPartSize, DEFAULT_PART_SIZE));
    }
    return GUrl();
}

} //namespace
