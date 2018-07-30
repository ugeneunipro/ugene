/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QToolButton>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentImport.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/ImportWidget.h>
#include <U2Gui/ObjectViewModel.h>

#include <U2View/AssemblyBrowserFactory.h>
#include <U2View/MaEditorFactory.h>

#include "DocumentProviderSelectorController.h"

namespace U2 {

const QString DocumentProviderSelectorController::DOCUMENT_PROVIDER_SELECTOR_CONTROLLER_ROOT = "/document_provider_selector_controller_settings/";
const QString DocumentProviderSelectorController::SELECTION = "selected";

int DocumentProviderSelectorController::selectResult(const GUrl& url, QList<FormatDetectionResult> &results) {
    SAFE_POINT(!results.isEmpty(), "Results list is empty!", -1);
    if (results.size() == 1) {
        return 0;
    }

    QObjectScopedPointer<DocumentProviderSelectorController> d = new DocumentProviderSelectorController(url, results, QApplication::activeModalWidget());

    const int rc = d->exec();
    CHECK(!d.isNull(), -1);

    if (rc == QDialog::Rejected) {
        return -1;
    }

    return d->getSelectedFormatIdx();
}

DocumentProviderSelectorController::DocumentProviderSelectorController(const GUrl& url, QList<FormatDetectionResult> &results, QWidget *parent) :
    QDialog(parent),
    formatDetectionResults(results),
    selectedRadioButton(0)
{
    setupUi(this);

    setObjectName("Select Document Format");
    new HelpButton(this, buttonBox, "21433285");
    gbFormats->setTitle(QString("Options for %1").arg(url.fileName()));
    buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);
    buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(true);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);

    QButtonGroup* bg = new QButtonGroup();
    connect(bg, SIGNAL(buttonClicked(int)), SLOT(sl_enableConvertInfo(int)));

    int size = formatDetectionResults.size();
    for (int i = 0; i < size; i++) {
        fillTitle(results[i]);
    }

    Settings* set = AppContext::getSettings();
    selectedFormat = set->getValue(DOCUMENT_PROVIDER_SELECTOR_CONTROLLER_ROOT + title + "/" + SELECTION).toString();

    for (int i = 0; i < size; i++) {
        addFormatRadioButton(url, results, bg, i);
    }

    formatsRadioButtons[selectedRadioButton]->setChecked(true);
    sl_enableConvertInfo(selectedRadioButton);

    CHECK(!formatsRadioButtons.isEmpty(), );
    formatsRadioButtons[0]->setFocus();
}

int DocumentProviderSelectorController::getSelectedFormatIdx() const {
    for (int i = 0; i < formatsRadioButtons.size(); i++) {
        if (formatsRadioButtons[i]->isChecked()) {
            return i;
        }
    }
    return 0;
}

QString DocumentProviderSelectorController::getButtonName(const GObjectType &objectType) {
    GObjectViewFactoryRegistry *objectViewFactoriesRegistry = AppContext::getObjectViewFactoryRegistry();
    SAFE_POINT(NULL != objectViewFactoriesRegistry, L10N::nullPointerError("Object View Factories Registry"), "");

    QString typeName;
    QString id;
    if (GObjectTypes::ASSEMBLY == objectType) {
        typeName = tr("Short reads assembly");
        id = AssemblyBrowserFactory::ID;
    } else if (GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT == objectType) {
        typeName = tr("Multiple sequence alignment");
        id = MsaEditorFactory::ID;
    } else {
        FAIL("An unexpected type", "");
    }

    GObjectViewFactory *factory = objectViewFactoriesRegistry->getFactoryById(id);
    SAFE_POINT(NULL != factory, L10N::nullPointerError("GObject View Factory"), "");

    QString res = tr("%1 in the %2").arg(typeName).arg(factory->getName());
    return res;
}

ImportWidget* DocumentProviderSelectorController::getRadioButtonWgt(const FormatDetectionResult& result, QString& radioButtonName, const GUrl& url, int it) {
    ImportWidget* wgt = NULL;
    if (result.format != NULL) {
        GObjectType supportedType = result.format->getSupportedObjectTypes().toList().first();
        radioButtonName = result.format->getRadioButtonText();
        if (radioButtonName.isEmpty() && !supportedType.isEmpty()) {
            radioButtonName = getButtonName(supportedType);
        }
    } else if (result.importer != NULL) {
        GObjectType supportedType = result.importer->getSupportedObjectTypes().toList().first();
        QString formatId = result.importer->getId();
        radioButtonName = result.importer->getRadioButtonText();
        if (radioButtonName.isEmpty() && !supportedType.isEmpty()) {
            radioButtonName = getButtonName(supportedType);
        }
        Settings* set = AppContext::getSettings();
        QVariantMap settings;
        QVariant defaultFormatId = set->getValue(DOCUMENT_PROVIDER_SELECTOR_CONTROLLER_ROOT + title + "/" + formatId);
        settings[ImportHint_FormatId] = defaultFormatId;
        wgt = result.importer->createImportWidget(url, settings);
        if (selectedFormat == formatId) {
            selectedRadioButton = it;
        }
    } else {
        assert(0);
    }

    return wgt;
}

void DocumentProviderSelectorController::addFormatRadioButton(const GUrl& url, QList<FormatDetectionResult> &results, QButtonGroup* bg, int it) {
    const FormatDetectionResult &result = results[it];
    QString text;
    ImportWidget* wgt = getRadioButtonWgt(result, text, url, it);

    QRadioButton *rbFormat = new QRadioButton(text);
    QString name = QString::number(it) + "_radio";
    rbFormat->setObjectName(name);
    formatsRadioButtons << rbFormat;
    bg->addButton(rbFormat, it);
    formatsLayout->addWidget(rbFormat);

    radioButtonConnectedWidget << wgt;
    if (wgt != NULL) {
        formatsLayout->addWidget(wgt);
    }
}

void DocumentProviderSelectorController::accept() {
    Settings* set = AppContext::getSettings();
    QString selectedRadioButton;
    int size = formatInfo.size();
    for (int i = 0; i < size; i++) {
        DocumentFormatId formatId;
        if (radioButtonConnectedWidget[i] != NULL) {
            QVariantMap settings = radioButtonConnectedWidget[i]->getSettings();
            formatId = settings[ImportHint_FormatId].toString();
            formatDetectionResults[i].rawDataCheckResult.properties.unite(settings);
        }
        bool isChecked = formatsRadioButtons[i]->isChecked();
        if (isChecked) {
            selectedRadioButton = formatInfo[i];
        }
        set->setValue(DOCUMENT_PROVIDER_SELECTOR_CONTROLLER_ROOT + title + "/" + formatInfo[i], formatId);
    }
    set->setValue(DOCUMENT_PROVIDER_SELECTOR_CONTROLLER_ROOT + title + "/" + SELECTION, selectedRadioButton);

    QDialog::accept();
}

void DocumentProviderSelectorController::sl_enableConvertInfo(int) {
    int size = formatsRadioButtons.size();
    for (int i = 0; i < size; i++) {
        bool state = formatsRadioButtons[i]->isChecked();
        if (radioButtonConnectedWidget[i] != NULL) {
            radioButtonConnectedWidget[i]->setEnabled(state);
        }
    }
}

void DocumentProviderSelectorController::fillTitle(const FormatDetectionResult &result) {
    if (result.format != NULL) {
        QString formatId = result.format->getFormatId();
        title += QString("%1__").arg(formatId);
        formatInfo << formatId;
    } else if (result.importer != NULL) {
        QString formatId = result.importer->getId();
        title += QString("%1__").arg(formatId);
        formatInfo << formatId;
    }
}

}   // namespace U2
