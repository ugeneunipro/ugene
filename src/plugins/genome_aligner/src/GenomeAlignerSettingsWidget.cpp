/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#include <U2Algorithm/OpenCLGpuRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/GUrl.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include "GenomeAlignerIndex.h"
#include "GenomeAlignerSettingsController.h"
#include "GenomeAlignerSettingsWidget.h"
#include "GenomeAlignerTask.h"

namespace U2 {

static const int MIN_READ_SIZE = 10;
static const int MIN_PART_SIZE = 1;
static const int DEFAULT_PART_SIZE = 10;

GenomeAlignerSettingsWidget::GenomeAlignerSettingsWidget(QWidget* parent) : DnaAssemblyAlgorithmMainWidget(parent) {
    setupUi(this);
    tabWidget->setCurrentIndex(0);
    layout()->setContentsMargins(0,0,0,0);

    connect(buildIndexFileButton, SIGNAL(clicked()), SLOT(sl_onSetIndexDirButtonClicked()));
    connect(partSlider, SIGNAL(valueChanged(int)), SLOT(sl_onPartSliderChanged(int)));
    connect(readSlider, SIGNAL(valueChanged(int)), SLOT(sl_onReadSliderChanged(int)));

    buildIndexFileButton->toggle();
#ifdef OPENCL_SUPPORT
    if (AppContext::getOpenCLGpuRegistry()->getEnabledGpu() == nullptr) {
#endif
        gpuBox->setEnabled(false);
#ifdef OPENCL_SUPPORT
    }
#endif

    systemSize = AppContext::getAppSettings()->getAppResourcePool()->getMaxMemorySizeInMB();
    partSlider->setEnabled(false);
    readSlider->setMinimum(MIN_READ_SIZE);
    readSlider->setMaximum(systemSize);
    readSlider->setValue(systemSize*2/3);

    QString indexDirPath = GenomeAlignerSettingsUtils::getIndexDir();
    QDir indexDir(indexDirPath);
    indexDir.mkpath(indexDirPath);

    indexDirEdit->setText(indexDirPath);

    partSizeLabel->setText(QByteArray::number(partSlider->value()) + " Mb");
    indexSizeLabel->setText(QByteArray::number(partSlider->value()*13) + " Mb");
    totalSizeLabel->setText(QByteArray::number(partSlider->value()*13 + readSlider->value()) + " Mb");
    systemSizeLabel->setText(QByteArray::number(systemSize) + " Mb");
}

QMap<QString,QVariant> GenomeAlignerSettingsWidget::getDnaAssemblyCustomSettings() const {
    QMap<QString,QVariant> settings;

    settings.insert(GenomeAlignerTask::OPTION_ALIGN_REVERSED, reverseBox->isChecked());
    settings.insert(GenomeAlignerTask::OPTION_OPENCL, gpuBox->isChecked());
    settings.insert(GenomeAlignerTask::OPTION_BEST, firstMatchBox->isChecked());
    settings.insert(GenomeAlignerTask::OPTION_READS_MEMORY_SIZE, readSlider->value());
    settings.insert(GenomeAlignerTask::OPTION_SEQ_PART_SIZE, partSlider->value());
    settings.insert(GenomeAlignerTask::OPTION_INDEX_DIR, indexDirEdit->text());
    if (omitQualitiesBox->isChecked()) {
        settings.insert(GenomeAlignerTask::OPTION_QUAL_THRESHOLD, qualThresholdBox->value() );
    }
    if (groupBox_mismatches->isChecked()) {
        settings.insert(GenomeAlignerTask::OPTION_MISMATCHES, mismatchesAllowedSpinBox->value());
        settings.insert(GenomeAlignerTask::OPTION_IF_ABS_MISMATCHES, absRadioButton->isChecked());
        settings.insert(GenomeAlignerTask::OPTION_PERCENTAGE_MISMATCHES, percentMismatchesAllowedSpinBox->value());
    } else {
        settings.insert(GenomeAlignerTask::OPTION_MISMATCHES, 0);
        settings.insert(GenomeAlignerTask::OPTION_IF_ABS_MISMATCHES, true);
        settings.insert(GenomeAlignerTask::OPTION_PERCENTAGE_MISMATCHES, 0);
    }

    return settings;
}

bool GenomeAlignerSettingsWidget::buildIndexUrl(const GUrl& url, bool prebuiltIndex, QString &error) const {
    if (prebuiltIndex) {
        GenomeAlignerIndex index;
        index.baseFileName = url.dirPath() + "/" + url.baseFileName();
        QByteArray e;
        bool res = index.deserialize(e);
        if (!res || url.lastFileSuffix() != GenomeAlignerIndex::HEADER_EXTENSION) {
            error = tr("This index file is corrupted. Please, load a valid index file.");
            return false;
        }

        partSlider->setMinimum(MIN_PART_SIZE);
        partSlider->setMaximum(index.seqPartSize);
        partSlider->setEnabled(true);
        partSlider->setValue(index.seqPartSize);
    } else {
        QString refUrl = url.getURLString();
        QFile file(refUrl);
        if (file.exists()) {
            int fileSize = 1 + (int)(file.size()/(1024*1024));
            int maxPartSize = qMin(fileSize*13, systemSize - MIN_READ_SIZE)/13;
            partSlider->setMinimum(MIN_PART_SIZE);
            partSlider->setMaximum(maxPartSize);
            partSlider->setEnabled(true);
            partSlider->setValue(qMin(maxPartSize, DEFAULT_PART_SIZE));
        }
    }

    return true;
}

bool GenomeAlignerSettingsWidget::isParametersOk(QString &error) const {
    bool gpuOk = (gpuBox->isChecked() == false) || ((gpuBox->isChecked() == true) && (partSlider->value() <= 10)); // 128MB is the minimum size for a buffer, according to CL_DEVICE_MAX_MEM_ALLOC_SIZE OpenCL documentation
    if ((systemSize < readSlider->value() + 13*partSlider->value()) || !gpuOk) {
        error = "There is no enough memory for the aligning on your computer. Try to reduce a memory size for short reads or for the reference fragment.";
        return false;
    }

    return true;
}

bool GenomeAlignerSettingsWidget::isIndexOk(const GUrl &refName, QString &error) const {
    GenomeAlignerIndex index;
    if (indexTab->isEnabled()) { //prebuiltIndex is not checked
        index.baseFileName = indexDirEdit->text() + "/" + refName.baseFileName();
    } else {
        index.baseFileName = refName.dirPath() + "/" + refName.baseFileName();
    }

    QByteArray e;
    bool res = index.deserialize(e);

    if (indexTab->isEnabled()) { //prebuiltIndex is not checked
        if (!res) {
            return true;
        }
        if (index.seqPartSize == partSlider->value()) {
            return true;
        }
        error = tr("The index folder has already contain the prebuilt index. But its reference fragmentation parameter is %1 and it doesn't equal to \
the parameter you have chosen (%2).\n\nPress \"Ok\" to delete this index file and create a new during the aligning.\nPress \"Cancel\" to change this parameter \
or the index folder.").arg(index.seqPartSize).arg(partSlider->value());
        return false;
    } else {
        if (!res || refName.lastFileSuffix() != GenomeAlignerIndex::HEADER_EXTENSION) {
            error = tr("This index file is corrupted. Please, load a valid index file.");
            return false;
        }
        return true;
    }
}

void GenomeAlignerSettingsWidget::sl_onSetIndexDirButtonClicked() {
    LastUsedDirHelper lod;

    lod.url = U2FileDialog::getExistingDirectory(this, tr("Set index files folder"), indexDirEdit->text());
    if (!lod.url.isEmpty()) {
        GUrl result = lod.url;
        indexDirEdit->setText(result.getURLString());
    }
}

void GenomeAlignerSettingsWidget::sl_onPartSliderChanged(int value) {
    partSizeLabel->setText(QByteArray::number(value) + " Mb");
    indexSizeLabel->setText(QByteArray::number(value*13) + " Mb");

    if (systemSize - 13*value >= MIN_READ_SIZE) {
        readSlider->setMaximum(systemSize - 13*value);
    } else {
        readSlider->setMaximum(MIN_READ_SIZE);
    }

    totalSizeLabel->setText(QByteArray::number(value*13 + readSlider->value()) + " Mb");
}

void GenomeAlignerSettingsWidget::sl_onReadSliderChanged(int value) {
    readSizeLabel->setText(QByteArray::number(value) + " Mb");
    totalSizeLabel->setText(QByteArray::number(partSlider->value()*13 + value) + " Mb");
}

} //namespace
