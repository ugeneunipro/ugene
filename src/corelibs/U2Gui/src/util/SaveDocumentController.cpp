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

#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QLineEdit>
#include <QToolButton>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/FormatUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/ProjectUtils.h>
#include <U2Gui/U2FileDialog.h>

#include "SaveDocumentController.h"

namespace U2 {

SaveDocumentControllerConfig::SaveDocumentControllerConfig()
    : fileNameEdit(NULL),
      fileDialogButton(NULL),
      formatCombo(NULL),
      compressCheckbox(NULL),
      parentWidget(NULL),
      rollSuffix("_"),
      rollFileName(true),
      rollOutProjectUrls(false) {

}

void SaveDocumentController::SimpleFormatsInfo::addFormat(const QString& name, const QStringList& extensions) {
    addFormat(name, name, extensions);
}

void SaveDocumentController::SimpleFormatsInfo::addFormat(const QString& id, const QString& name, const QStringList& _extensions) {
    extensions.insert(id, _extensions);
    names.insert(id, name);
}

QStringList SaveDocumentController::SimpleFormatsInfo::getNames() const {
    return names.values();
}

QString SaveDocumentController::SimpleFormatsInfo::getFormatNameById(const QString& id) const {
    return names.value(id);
}

QString SaveDocumentController::SimpleFormatsInfo::getFormatNameByExtension(const QString &ext) const {
    foreach (const QStringList& exts, extensions.values()) {
        if (exts.contains(ext)) {
            return names.value(extensions.key(exts));
        }
    }
    return QString();
}

QString SaveDocumentController::SimpleFormatsInfo::getIdByName(const QString &name) const {
    return names.key(name);
}

void SaveDocumentController::forceRoll(const QSet<QString>& excludeList) {
    setPath(getSaveFileName(), excludeList);
}

QStringList SaveDocumentController::SimpleFormatsInfo::getExtensionsByName(const QString &formatName) const {
    CHECK(names.values().contains(formatName), QStringList());
    return extensions.value(names.key(formatName));
}

QString SaveDocumentController::SimpleFormatsInfo::getFirstExtensionByName(const QString &formatName) const {
    const QStringList extensions = getExtensionsByName(formatName);
    CHECK(!extensions.isEmpty(), QString());
    return extensions.first();
}

SaveDocumentController::SaveDocumentController(const SaveDocumentControllerConfig &config,
                                               const DocumentFormatConstraints &formatCnstr,
                                               QObject *parent)
    : QObject(parent),
      conf(config),
      overwritingConfirmed(false) {
    initSimpleFormatInfo(formatCnstr);
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

SaveDocumentController::SaveDocumentController(const SaveDocumentControllerConfig &config,
                                               const SimpleFormatsInfo &formatsDesc,
                                               QObject *parent)
    : QObject(parent),
      conf(config),
      formatsInfo(formatsDesc),
      overwritingConfirmed(false) {
    init();
}

void SaveDocumentController::addFormat(const QString &id, const QString &name, const QStringList &extenstions) {
    formatsInfo.addFormat(id, name, extenstions);
    initFormatComboBox();
}

QString SaveDocumentController::getSaveFileName() const {
    return FileAndDirectoryUtils::getAbsolutePath(conf.fileNameEdit->text());
}

DocumentFormatId SaveDocumentController::getFormatIdToSave() const {
    SAFE_POINT(!currentFormat.isEmpty(), "Current format is not set", DocumentFormatId::null);
    return formatsInfo.getIdByName(currentFormat);
}

void SaveDocumentController::sl_fileNameChanged(const QString &newName) {
    GUrl url(newName);
    QString ext = GUrlUtils::getUncompressedExtension(url);
    if (!formatsInfo.getExtensionsByName(currentFormat).contains(ext) &&
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

    lod.url = U2FileDialog::getSaveFileName(conf.parentWidget, conf.saveTitle, defaultUrl, filter, &defaultFilter);
    if (lod.url.isEmpty()) {
        return;
    }

    addFormatExtension(lod.url);
    addGzExtension(lod.url);
    overwritingConfirmed = true;
    setPath(lod.url);
}

void SaveDocumentController::sl_formatChanged(const QString &newFormat) {
    currentFormat = newFormat;

    if (conf.compressCheckbox != NULL) {
        DocumentFormatRegistry* fr = AppContext::getDocumentFormatRegistry();
        SAFE_POINT(fr != NULL, L10N::nullPointerError("DocumentFormatRegistry"), );
        DocumentFormat* format = fr->getFormatById(formatsInfo.getIdByName(newFormat));
        if (format != NULL) { // custom format names without DocumentFormat class can be added into the formats combobox (e.g. ExportCoverageDialog)
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
    CHECK(NULL != conf.compressCheckbox && conf.compressCheckbox->isEnabled(), );
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

    if (NULL != conf.compressCheckbox) {
        connect(conf.compressCheckbox, SIGNAL(toggled(bool)), SLOT(sl_compressToggled(bool)));
    }

    initFormatComboBox();
    CHECK(conf.fileDialogButton != NULL, );
    connect(conf.fileDialogButton, SIGNAL(clicked()), SLOT(sl_fileDialogButtonClicked()));
}

void SaveDocumentController::initSimpleFormatInfo(DocumentFormatConstraints formatConstraints) {
    formatConstraints.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);

    DocumentFormatRegistry* fr = AppContext::getDocumentFormatRegistry();
    QList<DocumentFormatId> selectedFormats = fr->selectFormats(formatConstraints);
    initSimpleFormatInfo(selectedFormats);
}

void SaveDocumentController::initSimpleFormatInfo(const QList<DocumentFormatId> &formats) {
    DocumentFormatRegistry* fr = AppContext::getDocumentFormatRegistry();
    foreach(DocumentFormatId id, formats) {
        formatsInfo.addFormat(id,
                              fr->getFormatById(id)->getFormatName(),
                              fr->getFormatById(id)->getSupportedDocumentFileExtensions());
    }
}

void SaveDocumentController::initFormatComboBox() {
    currentFormat = formatsInfo.getFormatNameById(conf.defaultFormatId);
    CHECK(conf.formatCombo != NULL, );

    conf.formatCombo->blockSignals(true);
    conf.formatCombo->clear();

    QStringList items = formatsInfo.getNames();
    items.sort(Qt::CaseInsensitive);
    conf.formatCombo->addItems(items);

    if (currentFormat.isEmpty()) {
        currentFormat = conf.formatCombo->itemText(0);
    }
    conf.formatCombo->setCurrentText(currentFormat);

    connect(conf.formatCombo, SIGNAL(currentIndexChanged(const QString&)), SLOT(sl_formatChanged(const QString&)), Qt::UniqueConnection);
    sl_formatChanged(conf.formatCombo->currentText());
    conf.formatCombo->blockSignals(false);
}

bool SaveDocumentController::cutGzExtension(QString &path) const {
    CHECK(NULL != conf.compressCheckbox, false);
    CHECK(path.endsWith(".gz"), false);
    path.chop(QString(".gz").length());
    return true;
}

void SaveDocumentController::addGzExtension(QString &path) const {
    CHECK(NULL != conf.compressCheckbox && conf.compressCheckbox->isChecked() && conf.compressCheckbox->isEnabled(), );
    CHECK(!path.endsWith(".gz"), );
    path += ".gz";
}

void SaveDocumentController::addFormatExtension(QString &path) const {
    bool gzWasCut = cutGzExtension(path);
    QFileInfo fileInfo(path);
    const QStringList extensions = formatsInfo.getExtensionsByName(currentFormat);
    if (!extensions.isEmpty() && !extensions.contains(fileInfo.suffix())) {
        path += "." + extensions.first();
    }

    if (gzWasCut) {
        addGzExtension(path);
    }
}

QString SaveDocumentController::prepareDefaultFileFilter() const {
    QStringList extraExtensions;
    if (NULL != conf.compressCheckbox && conf.compressCheckbox->isEnabled()) {
        extraExtensions << ".gz";
    }

    return FormatUtils::prepareFileFilter(currentFormat, formatsInfo.getExtensionsByName(currentFormat), false, extraExtensions);
}

QString SaveDocumentController::prepareFileFilter() const {
    QMap<QString, QStringList> formatsWithExtensions;
    foreach (const QString &formatName, formatsInfo.getNames()) {
        formatsWithExtensions.insert(formatName, formatsInfo.getExtensionsByName(formatName));
    }

    QStringList extraExtensions;
    if (NULL != conf.compressCheckbox && conf.compressCheckbox->isEnabled()) {
        extraExtensions << ".gz";
    }

    return FormatUtils::prepareFileFilter(formatsWithExtensions, false, extraExtensions);
}

void SaveDocumentController::setPath(const QString &path, const QSet<QString>& excludeList) {
    QSet<QString> currentExcludeList = excludeList;
    if (conf.rollOutProjectUrls) {
        currentExcludeList += DocumentUtils::getNewDocFileNameExcludesHint();
    }

    const QString newPath = (conf.rollFileName && !overwritingConfirmed) ? GUrlUtils::rollFileName(path, conf.rollSuffix, currentExcludeList) : path;
    conf.fileNameEdit->setText(QDir::toNativeSeparators(newPath));
    overwritingConfirmed = false;
    emit si_pathChanged(newPath);
}

void SaveDocumentController::setFormat(const QString &formatId) {
    SAFE_POINT(!formatsInfo.getFormatNameById(formatId).isEmpty(), QString("Format '%1' not found in the model"), );
    if (NULL != conf.formatCombo) {
        conf.formatCombo->setCurrentText(formatsInfo.getFormatNameById(formatId));
    } else {
        sl_formatChanged(formatsInfo.getFormatNameById(formatId));
    }
    emit si_formatChanged(formatId);
}

}
