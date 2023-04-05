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

#include "SaveDocumentController.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QLineEdit>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/FileFilters.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ProjectUtils.h>
#include <U2Gui/U2FileDialog.h>

namespace U2 {

SaveDocumentControllerConfig::SaveDocumentControllerConfig()
    : fileNameEdit(nullptr),
      fileDialogButton(nullptr),
      formatCombo(nullptr),
      compressCheckbox(nullptr),
      parentWidget(nullptr),
      rollSuffix("_"),
      rollFileName(true),
      rollOutProjectUrls(false) {
}

void SaveDocumentController::SimpleFormatsInfo::addFormat(const QString& name, const QStringList& extensions) {
    addFormat(name, name, extensions);
}

void SaveDocumentController::SimpleFormatsInfo::addFormat(const QString& id, const QString& name, const QStringList& extensions) {
    extensionsByFormatId.insert(id, extensions);
    nameByFormatId.insert(id, name);
    formatIdByName.insert(name, id);
    for (const QString& extension : qAsConst(extensions)) {
        formatIdByExtension.insert(extension, id);
    }
}

QStringList SaveDocumentController::SimpleFormatsInfo::getNames() const {
    return nameByFormatId.values();
}

QString SaveDocumentController::SimpleFormatsInfo::getFormatNameById(const QString& id) const {
    return nameByFormatId.value(id);
}

QString SaveDocumentController::SimpleFormatsInfo::getFormatNameByExtension(const QString& extension) const {
    QString formatId = formatIdByExtension.value(extension);
    return nameByFormatId.value(formatId);
}

QString SaveDocumentController::SimpleFormatsInfo::getIdByName(const QString& name) const {
    return formatIdByName.value(name);
}

void SaveDocumentController::forceRoll(const QSet<QString>& excludeList) {
    setPath(getSaveFileName(), excludeList);
}

QStringList SaveDocumentController::SimpleFormatsInfo::getExtensionsByName(const QString& formatName) const {
    QString formatId = formatIdByName.value(formatName);
    return extensionsByFormatId.value(formatId);
}

QString SaveDocumentController::SimpleFormatsInfo::getFirstExtensionByName(const QString& formatName) const {
    QStringList formatExtensions = getExtensionsByName(formatName);
    CHECK(!formatExtensions.isEmpty(), "");
    return formatExtensions.first();
}

SaveDocumentController::SaveDocumentController(const SaveDocumentControllerConfig& config,
                                               const DocumentFormatConstraints& formatConstraints,
                                               QObject* parent)
    : QObject(parent),
      conf(config),
      overwritingConfirmed(false) {
    initSimpleFormatInfo(formatConstraints);
    init();
}

SaveDocumentController::SaveDocumentController(const SaveDocumentControllerConfig& config,
                                               const QList<DocumentFormatId>& formats,
                                               QObject* parent)
    : QObject(parent),
      conf(config),
      overwritingConfirmed(false) {
    initSimpleFormatInfo(formats);
    init();
}

SaveDocumentController::SaveDocumentController(const SaveDocumentControllerConfig& config,
                                               const SimpleFormatsInfo& formatsDesc,
                                               QObject* parent)
    : QObject(parent),
      conf(config),
      formatsInfo(formatsDesc),
      overwritingConfirmed(false) {
    init();
}

void SaveDocumentController::addFormat(const QString& id, const QString& name, const QStringList& extensions) {
    formatsInfo.addFormat(id, name, extensions);
    initFormatComboBox();
}

QString SaveDocumentController::getSaveFileName() const {
    return FileAndDirectoryUtils::getAbsolutePath(conf.fileNameEdit->text());
}

DocumentFormatId SaveDocumentController::getFormatIdToSave() const {
    SAFE_POINT(!currentFormatName.isEmpty(), "Current format is not set", DocumentFormatId());
    return formatsInfo.getIdByName(currentFormatName);
}

QString SaveDocumentController::getValidatedSaveFilePath(U2OpStatus& os) const {
    QString fileName = getSaveFileName();
    CHECK_EXT(!fileName.isEmpty(), os.setError(tr("Output file name is empty")), "");

    CHECK_EXT(FileAndDirectoryUtils::canWriteToPath(fileName), os.setError(L10N::errorOpeningFileWrite(fileName)), "");

    return fileName;
}

void SaveDocumentController::sl_fileNameChanged(const QString& newName) {
    GUrl url(newName);
    QString ext = GUrlUtils::getUncompressedExtension(url);
    if (!formatsInfo.getExtensionsByName(currentFormatName).contains(ext) &&
        !formatsInfo.getFormatNameByExtension(ext).isEmpty()) {
        overwritingConfirmed = true;
        setFormat(formatsInfo.getIdByName(formatsInfo.getFormatNameByExtension(ext)));
    }
}

void SaveDocumentController::sl_fileDialogButtonClicked() {
    QString defaultFilter = prepareDefaultFileFilter();
    const QString filter = prepareFileFilter();

    QString defaultUrl = getSaveFileName();
    LastUsedDirHelper lod(conf.defaultDomain, defaultUrl);
    if (defaultUrl.isEmpty()) {
        defaultUrl = lod;
    }

    cutGzExtension(defaultUrl);

    lod.url = U2FileDialog::getSaveFileName(conf.parentWidget, conf.saveTitle, defaultUrl, filter, defaultFilter);
    if (lod.url.isEmpty()) {
        return;
    }

    addFormatExtension(lod.url);
    addGzExtension(lod.url);
    overwritingConfirmed = true;
    setPath(lod.url);
}

void SaveDocumentController::sl_formatChanged(const QString& newFormat) {
    currentFormatName = newFormat;

    if (conf.compressCheckbox != nullptr) {
        DocumentFormatRegistry* fr = AppContext::getDocumentFormatRegistry();
        SAFE_POINT(fr != nullptr, L10N::nullPointerError("DocumentFormatRegistry"), );
        DocumentFormat* format = fr->getFormatById(formatsInfo.getIdByName(newFormat));
        if (format != nullptr) {  // custom format names without DocumentFormat class can be added into the formats combobox (e.g. ExportCoverageDialog)
            conf.compressCheckbox->setDisabled(format->checkFlags(DocumentFormatFlag_CannotBeCompressed));
        }
    }

    if (!conf.fileNameEdit->text().isEmpty()) {
        QString oldPath = conf.fileNameEdit->text();
        cutGzExtension(oldPath);

        const QFileInfo fileInfo(oldPath);
        const QString fileExt = formatsInfo.getFirstExtensionByName(newFormat);
        const QString fileDir = fileInfo.dir().path();

        QString newPath = QString("%1/%2.%3").arg(fileDir).arg(fileInfo.completeBaseName()).arg(fileExt);
        addGzExtension(newPath);
        setPath(newPath);
    }

    emit si_formatChanged(formatsInfo.getIdByName(newFormat));
}

void SaveDocumentController::sl_compressToggled(bool enable) {
    CHECK(conf.compressCheckbox != nullptr && conf.compressCheckbox->isEnabled(), );
    QString path = conf.fileNameEdit->text();
    if (enable) {
        addGzExtension(path);
    } else {
        cutGzExtension(path);
    }
    setPath(path);
}

void SaveDocumentController::init() {
    QString path = conf.defaultFileName;
    if (conf.defaultFileName.isEmpty()) {
        path = conf.fileNameEdit->text();
    }
    setPath(path);

    connect(conf.fileNameEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_fileNameChanged(const QString&)));
    connect(conf.fileNameEdit, SIGNAL(textEdited(const QString&)), SLOT(sl_fileNameChanged(const QString&)));

    if (conf.compressCheckbox != nullptr) {
        connect(conf.compressCheckbox, SIGNAL(toggled(bool)), SLOT(sl_compressToggled(bool)));
    }

    initFormatComboBox();
    CHECK(conf.fileDialogButton != nullptr, );
    connect(conf.fileDialogButton, SIGNAL(clicked()), SLOT(sl_fileDialogButtonClicked()));
}

void SaveDocumentController::initSimpleFormatInfo(DocumentFormatConstraints formatConstraints) {
    formatConstraints.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);

    DocumentFormatRegistry* fr = AppContext::getDocumentFormatRegistry();
    QList<DocumentFormatId> selectedFormats = fr->selectFormats(formatConstraints);
    initSimpleFormatInfo(selectedFormats);
}

void SaveDocumentController::initSimpleFormatInfo(const QList<DocumentFormatId>& formats) {
    DocumentFormatRegistry* fr = AppContext::getDocumentFormatRegistry();
    foreach (DocumentFormatId id, formats) {
        formatsInfo.addFormat(id,
                              fr->getFormatById(id)->getFormatName(),
                              fr->getFormatById(id)->getSupportedDocumentFileExtensions());
    }
}

void SaveDocumentController::initFormatComboBox() {
    currentFormatName = formatsInfo.getFormatNameById(conf.defaultFormatId);
    CHECK(conf.formatCombo != nullptr, );

    conf.formatCombo->blockSignals(true);
    conf.formatCombo->clear();

    QStringList items = formatsInfo.getNames();
    items.sort(Qt::CaseInsensitive);
    conf.formatCombo->addItems(items);

    if (currentFormatName.isEmpty()) {
        currentFormatName = conf.formatCombo->itemText(0);
    }
    conf.formatCombo->setCurrentText(currentFormatName);

    connect(conf.formatCombo, SIGNAL(currentIndexChanged(const QString&)), SLOT(sl_formatChanged(const QString&)), Qt::UniqueConnection);
    sl_formatChanged(conf.formatCombo->currentText());
    conf.formatCombo->blockSignals(false);
}

bool SaveDocumentController::cutGzExtension(QString& path) const {
    CHECK(conf.compressCheckbox != nullptr, false);
    CHECK(path.endsWith(".gz"), false);
    path.chop(QString(".gz").length());
    return true;
}

void SaveDocumentController::addGzExtension(QString& path) const {
    CHECK(conf.compressCheckbox != nullptr && conf.compressCheckbox->isChecked() && conf.compressCheckbox->isEnabled(), );
    CHECK(!path.endsWith(".gz"), );
    path += ".gz";
}

void SaveDocumentController::addFormatExtension(QString& path) const {
    bool gzWasCut = cutGzExtension(path);
    QFileInfo fileInfo(path);
    const QStringList extensions = formatsInfo.getExtensionsByName(currentFormatName);
    if (!extensions.isEmpty() && !extensions.contains(fileInfo.suffix())) {
        path += "." + extensions.first();
    }

    if (gzWasCut) {
        addGzExtension(path);
    }
}

QString SaveDocumentController::prepareDefaultFileFilter() const {
    bool useGzipVariant = conf.compressCheckbox && conf.compressCheckbox->isEnabled();
    return FileFilters::createSingleFileFilter(currentFormatName,
                                               formatsInfo.getExtensionsByName(currentFormatName),
                                               useGzipVariant);
}

QString SaveDocumentController::prepareFileFilter() const {
    QMap<QString, QStringList> extensionByFormatName;
    foreach (const QString& formatName, formatsInfo.getNames()) {
        extensionByFormatName.insert(formatName, formatsInfo.getExtensionsByName(formatName));
    }
    bool useGzipVariant = conf.compressCheckbox && conf.compressCheckbox->isEnabled();
    return FileFilters::createFileFilter(extensionByFormatName, useGzipVariant);
}

void SaveDocumentController::setPath(const QString& path, const QSet<QString>& excludeList) {
    QSet<QString> currentExcludeList = excludeList;
    if (conf.rollOutProjectUrls) {
        currentExcludeList += DocumentUtils::getNewDocFileNameExcludesHint();
    }

    const QString newPath = (conf.rollFileName && !overwritingConfirmed) ? GUrlUtils::rollFileName(path, conf.rollSuffix, currentExcludeList) : path;
    conf.fileNameEdit->setText(QDir::toNativeSeparators(newPath));
    overwritingConfirmed = false;
    emit si_pathChanged(newPath);
}

void SaveDocumentController::setFormat(const QString& formatId) {
    SAFE_POINT(!formatsInfo.getFormatNameById(formatId).isEmpty(), QString("Format '%1' not found in the model"), );
    if (conf.formatCombo != nullptr) {
        conf.formatCombo->setCurrentText(formatsInfo.getFormatNameById(formatId));
    } else {
        sl_formatChanged(formatsInfo.getFormatNameById(formatId));
    }
    emit si_formatChanged(formatId);
}

}  // namespace U2
