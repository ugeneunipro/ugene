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

#include <QImageWriter>
#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/Theme.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>

#include "ExportImageDialog.h"
#include "imageExport/WidgetScreenshotExportTask.h"
#include "ui_ExportImageDialog.h"

static const QString IMAGE_DIR = "image";
static const char *DIALOG_ACCEPT_ERROR_TITLE = "Unable to save file";

namespace U2 {

ExportImageDialog::ExportImageDialog(QWidget *screenShotWidget,
                                     InvokedFrom invoSource,
                                     const QString &file,
                                     ImageScalingPolicy scalingPolicy,
                                     QWidget *parent)
    : QDialog(parent),
      scalingPolicy(scalingPolicy),
      filename(file),
      origFilename(file),
      source(invoSource)
{
    exportController = new WidgetScreenshotImageExportController(screenShotWidget);
    init();
}

ExportImageDialog::ExportImageDialog(ImageExportController *factory,
                                     InvokedFrom invoSource,
                                     const QString &file,
                                     ImageScalingPolicy scalingPolicy,
                                     QWidget *parent)
    : QDialog(parent),
      exportController(factory),
      scalingPolicy(scalingPolicy),
      filename(file),
      origFilename(file),
      source(invoSource)
{
    SAFE_POINT( exportController != NULL, tr("Image export task factory is NULL"), );
    init();
}

ExportImageDialog::~ExportImageDialog() {
    delete ui;
}

int ExportImageDialog::getWidth() const {
    return ui->widthSpinBox->value();
}

int ExportImageDialog::getHeight() const {
    return ui->heightSpinBox->value();
}

bool ExportImageDialog::hasQuality() const {
    return ui->qualitySpinBox->isEnabled();
}

int ExportImageDialog::getQuality() const {
    return ui->qualitySpinBox->value();
}

void ExportImageDialog::accept() {
    filename = saveController->getSaveFileName();
    if (filename.isEmpty()) {
        QMessageBox::warning(this, tr(DIALOG_ACCEPT_ERROR_TITLE), tr("The image file path is empty."));
        return;
    }

    U2OpStatusImpl os;
    GUrlUtils::prepareFileLocation(filename, os);
    if (!GUrlUtils::canWriteFile(filename)) {
        QMessageBox::warning(this, tr(DIALOG_ACCEPT_ERROR_TITLE), tr("The image file cannot be created. No write permissions."));
        return;
    }

    format = saveController->getFormatIdToSave();

    LastUsedDirHelper lod(IMAGE_DIR);
    lod.url = filename;
    ioLog.info(tr("Saving image to '%1'...").arg(filename));

    ImageExportTaskSettings settings(filename, format,
                                     QSize(getWidth(), getHeight()),
                                     (hasQuality() ? getQuality() : -1),
                                     ui->dpiSpinBox->value());
    Task* task = exportController->getTaskInstance(settings);
    AppContext::getTaskScheduler()->registerTopLevelTask(task);

    QDialog::accept();
}

void ExportImageDialog::sl_onFormatsBoxItemChanged(const QString &format) {
    setSizeControlsEnabled(!isVectorGraphicFormat(format));

    const bool areQualityWidgetsVisible = isLossyFormat( format );
    ui->qualityLabel->setVisible( areQualityWidgetsVisible );
    ui->qualityHorizontalSlider->setVisible( areQualityWidgetsVisible );
    ui->qualitySpinBox->setVisible( areQualityWidgetsVisible );
}

void ExportImageDialog::sl_showMessage(const QString &message) {
    ui->hintLabel->setText(message);
    if (!message.isEmpty()) {
        ui->hintLabel->show();
    } else {
        ui->hintLabel->hide();
    }
}

void ExportImageDialog::sl_disableExport(bool disable) {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(disable);
}

void ExportImageDialog::init() {
    ui = new Ui_ImageExportForm;
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Export"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    switch (source) {
    case WD:
        new HelpButton(this, ui->buttonBox, "28966993");
        break;
    case CircularView:
        new HelpButton(this, ui->buttonBox, "24748796");
        break;
    case MSA:
        new HelpButton(this, ui->buttonBox, "24748869");
        break;
    case SequenceView:
        new HelpButton(this, ui->buttonBox, "24748757");
        break;
    case AssemblyView:
        new HelpButton(this, ui->buttonBox, "24748906");
        break;
    case PHYTreeView:
        new HelpButton(this, ui->buttonBox, "24748934");
        break;
    case DotPlot:
        new HelpButton(this, ui->buttonBox, "24748827");
        break;
    case MolView:
        new HelpButton(this, ui->buttonBox, "24748810");
        break;
    default:
        FAIL("Can't find help Id",);
        break;
    }

    ui->dpiWidget->setVisible(source == DotPlot);

    // set tip color
    QString style = "QLabel { color: " + Theme::errorColorLabelStr() + "; font: bold;}";
    ui->hintLabel->setStyleSheet(style);
    ui->hintLabel->hide();

    QString defaultFormat = "PNG";
    initSaveController(defaultFormat);

    if (scalingPolicy == NoScaling) {
        ui->imageSizeSettingsContainer->hide();
    }

    ui->widthSpinBox->setValue(exportController->getImageWidth());
    ui->heightSpinBox->setValue(exportController->getImageHeight());

    setSizeControlsEnabled(!isVectorGraphicFormat(saveController->getFormatIdToSave()));

    connect(ui->formatsBox, SIGNAL(currentIndexChanged(const QString&)), exportController, SLOT(sl_onFormatChanged(const QString&)));
    connect(ui->formatsBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(sl_onFormatsBoxItemChanged(const QString&)));

    connect(exportController, SIGNAL(si_disableExport(bool)), SLOT(sl_disableExport(bool)));
    connect(exportController, SIGNAL(si_showMessage(QString)), SLOT(sl_showMessage(QString)));

    if (exportController->isExportDisabled()) {
        sl_disableExport(true);
        sl_showMessage(exportController->getDisableMessage());
    }

    QWidget* settingsWidget = exportController->getSettingsWidget();
    if (settingsWidget == NULL) {
        ui->settingsGroupBox->hide();
    } else {
        ui->settingsLayout->addWidget(settingsWidget);
    }
    sl_onFormatsBoxItemChanged(defaultFormat);
}

void ExportImageDialog::initSaveController(const QString& defaultFormat) {
    LastUsedDirHelper dirHelper(IMAGE_DIR, GUrlUtils::getDefaultDataPath());

    SaveDocumentControllerConfig config;
    config.defaultDomain = IMAGE_DIR;
    config.defaultFileName = dirHelper.dir + "/" + GUrlUtils::fixFileName(origFilename);
    config.defaultFormatId = defaultFormat;
    config.fileDialogButton = ui->browseFileButton;
    config.fileNameEdit = ui->fileNameEdit;
    config.formatCombo = ui->formatsBox;
    config.parentWidget = this;
    config.saveTitle = tr("Save Image As");
    config.rollSuffix = "_copy";

    SaveDocumentController::SimpleFormatsInfo formatsInfo;
    QStringList formats = getFormats();
    foreach (const QString &format, formats) {
        formatsInfo.addFormat(format, QStringList() << format.toLower());
    }

    saveController = new SaveDocumentController(config, formatsInfo, this);
    saveController->setFormat(saveController->getFormatIdToSave());
}

void ExportImageDialog::setSizeControlsEnabled(bool enabled) {
    ui->widthLabel->setEnabled(enabled);
    ui->heightLabel->setEnabled(enabled);
    ui->widthSpinBox->setEnabled(enabled);
    ui->heightSpinBox->setEnabled(enabled);
}

QStringList ExportImageDialog::getFormats() {
    return getRasterFormats() + getSvgAndPdfFormats();
}

QStringList ExportImageDialog::getRasterFormats() {
    QStringList result;
    CHECK(exportController->isRasterFormatsEnabled(), result);
    QList<QByteArray> qtList = QImageWriter::supportedImageFormats();

    if (qtList.contains("png")) {
        result.append("PNG");
    }
    if (qtList.contains("bmp")) {
        result.append("BMP");
    }
    if (qtList.contains("gif")) {
        result.append("GIF");
    }
    if (qtList.contains("jpg") || qtList.contains("jpeg")) {
        result.append("JPG");
    }
    if (qtList.contains("tif") || qtList.contains("tiff")) {
        result.append("TIFF");
    }
    return result;
}

QStringList ExportImageDialog::getSvgAndPdfFormats() {
    QStringList result;
    if (exportController->isSvgSupported()) {
        result << ImageExportTaskSettings::SVG_FORMAT;
    }

    if (exportController->isPdfSupported()) {
        result << ImageExportTaskSettings::PS_FORMAT;
        result << ImageExportTaskSettings::PDF_FORMAT;
    }

    return result;
}

bool ExportImageDialog::isVectorGraphicFormat( const QString &formatName ) {
    return ( ImageExportTaskSettings::SVG_FORMAT == formatName ) || ( ImageExportTaskSettings::PS_FORMAT == formatName )
        || ( ImageExportTaskSettings::PDF_FORMAT == formatName );
}

bool ExportImageDialog::isLossyFormat(const QString &formatName) {
    QString lcFormat = formatName.toLower();
    return lcFormat == "jpeg" || lcFormat == "jpg";
}

} // namespace

