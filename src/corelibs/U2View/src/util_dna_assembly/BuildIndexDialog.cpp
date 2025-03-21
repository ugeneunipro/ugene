/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "BuildIndexDialog.h"

#include <QMessageBox>
#include <QPushButton>

#include <U2Algorithm/DnaAssemblyAlgRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/AppSettingsGUI.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include "DnaAssemblyGUIExtension.h"

namespace U2 {

QString BuildIndexDialog::genomePath;

BuildIndexDialog::BuildIndexDialog(const DnaAssemblyAlgRegistry* registry, QWidget* p)
    : QDialog(p), assemblyRegistry(registry), customGUI(nullptr) {
    setupUi(this);
    QMap<QString, QString> helpPagesMap;
    helpPagesMap.insert("BWA", "65930872");
    helpPagesMap.insert("BWA-MEM", "65930886");
    helpPagesMap.insert("BWA-SW", "65930879");
    helpPagesMap.insert("Bowtie", "65930855");
    helpPagesMap.insert("Bowtie2", "65930864");
    helpPagesMap.insert("UGENE Genome Aligner", "65930893");
    new ComboboxDependentHelpButton(this, buttonBox, methodNamesBox, helpPagesMap);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Start"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    QStringList names = registry->getRegisteredAlgorithmsWithIndexFileSupport();
    methodNamesBox->addItems(names);
    // TODO: change the way default method is set
    if (names.size() > 0) {
        methodNamesBox->setCurrentIndex(names.size() - 1);
    }
    sl_onAlgorithmChanged(methodNamesBox->currentText());
    connect(setIndexFileNameButton, SIGNAL(clicked()), SLOT(sl_onSetIndexFileNameButtonClicked()));
    connect(addRefButton, SIGNAL(clicked()), SLOT(sl_onAddRefButtonClicked()));
    connect(methodNamesBox, SIGNAL(currentIndexChanged(const QString&)), SLOT(sl_onAlgorithmChanged(const QString&)));

    if (!genomePath.isEmpty()) {
        refSeqEdit->setText(genomePath);
        buildIndexUrl(genomePath);
        SAFE_POINT(customGUI != nullptr, "Build Index dialog referenced null pointer", );
        customGUI->validateReferenceSequence(genomePath);
    }
}

void BuildIndexDialog::sl_onAddRefButtonClicked() {
    LastUsedDirHelper lod;
    QString filter;

    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        lod.url = U2FileDialog::getOpenFileName(this, tr("Open reference sequence"), lod.dir, filter, 0, QFileDialog::DontUseNativeDialog);
    } else {
        lod.url = U2FileDialog::getOpenFileName(this, tr("Open reference sequence"), lod.dir, filter);
    }
    if (lod.url.isEmpty()) {
        return;
    }

    if (customGUI != nullptr) {
        customGUI->validateReferenceSequence(GUrl(lod.url));
    }
    refSeqEdit->setText(lod.url);
    buildIndexUrl(lod.url);
}

void BuildIndexDialog::sl_onSetIndexFileNameButtonClicked() {
    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getSaveFileName(this, tr("Set index file name"), lod.dir);
    if (!lod.url.isEmpty()) {
        GUrl index = lod.url;
        if (index.lastFileSuffix().isEmpty() && customGUI != nullptr) {
            QString extension = customGUI->getIndexFileExtension();
            if (extension.isEmpty()) {
                index = QString("%1").arg(index.getURLString());
            } else {
                index = QString("%1.%2").arg(index.getURLString()).arg(extension);
            }
        }
        indexFileNameEdit->setText(index.getURLString());
    }
}

void BuildIndexDialog::sl_onAlgorithmChanged(const QString&) {
    updateState();
}

void BuildIndexDialog::updateState() {
    addGuiExtension();
}

void BuildIndexDialog::addGuiExtension() {
    // cleanup previous extension
    if (customGUI != nullptr) {
        layout()->removeWidget(customGUI);
        setMinimumHeight(minimumHeight() - customGUI->minimumHeight());
        delete customGUI;
        customGUI = nullptr;
    }

    // insert new extension widget
    DnaAssemblyAlgorithmEnv* env = assemblyRegistry->getAlgorithm(methodNamesBox->currentText());
    if (env == nullptr) {
        adjustSize();
        return;
    }
    DnaAssemblyGUIExtensionsFactory* gui = env->getGUIExtFactory();
    if (gui != nullptr && gui->hasBuildIndexWidget()) {
        customGUI = gui->createBuildIndexWidget(this);
        int insertPos = verticalLayout->count() - 1;
        verticalLayout->insertWidget(insertPos, customGUI);
        if (!refSeqEdit->text().isEmpty()) {
            buildIndexUrl(refSeqEdit->text());
            customGUI->validateReferenceSequence(GUrl(refSeqEdit->text()));
        }
        customGUI->show();
    }
    adjustSize();
}

void BuildIndexDialog::buildIndexUrl(const GUrl& refUrl) {
    QString extension("");
    GUrl url;
    if (customGUI != nullptr) {
        extension = customGUI->getIndexFileExtension();
        url = customGUI->buildIndexUrl(refUrl);
    }
    if (url.isEmpty()) {
        if (extension.isEmpty()) {
            url = GUrlUtils::rollFileName(refUrl.dirPath() + "/" + refUrl.baseFileName(), DocumentUtils::getNewDocFileNameExcludesHint());
        } else {
            url = GUrlUtils::rollFileName(refUrl.dirPath() + "/" + refUrl.baseFileName() + "." + extension, DocumentUtils::getNewDocFileNameExcludesHint());
        }
    }

    indexFileNameEdit->setText(url.getURLString());
}

void BuildIndexDialog::accept() {
    if ((getAlgorithmName() == "Bowtie") || (getAlgorithmName() == "Bowtie2") || (getAlgorithmName() == "BWA") || (getAlgorithmName() == "BWA-MEM") || (getAlgorithmName() == "BWA-SW")) {
        QString externalToolId;

        if (getAlgorithmName() == "Bowtie2") {
            externalToolId = "USUPP_BOWTIE2_BUILD";
        }
        if (getAlgorithmName() == "Bowtie") {
            externalToolId = "USUPP_BOWTIE_BUILD";
        }
        if ((getAlgorithmName() == "BWA") || (getAlgorithmName() == "BWA-MEM") || (getAlgorithmName() == "BWA-SW")) {
            externalToolId = "USUPP_BWA";
        }
        if (AppContext::getExternalToolRegistry()->getById(externalToolId)->getPath().isEmpty()) {
            QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(this);
            msgBox->setWindowTitle(tr("DNA Assembly"));
            msgBox->setInformativeText(tr("Do you want to select it now?"));
            msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox->setDefaultButton(QMessageBox::Yes);
            msgBox->setText(tr(QString("Path for <i>" + AppContext::getExternalToolRegistry()->getToolNameById(externalToolId) + "</i> tool is not selected.").toLatin1().data()));
            const int ret = msgBox->exec();
            CHECK(!msgBox.isNull(), );

            switch (ret) {
                case QMessageBox::Yes:
                    AppContext::getAppSettingsGUI()->showSettingsDialog(APP_SETTINGS_EXTERNAL_TOOLS);
                    break;
                case QMessageBox::No:
                    return;
                    break;
                default:
                    assert(false);
                    break;
            }
            if (AppContext::getExternalToolRegistry()->getById(externalToolId)->getPath().isEmpty()) {
                return;
            }
        }
    }
    if (refSeqEdit->text().isEmpty()) {
        QMessageBox::information(this, tr("Build Index"), tr("Reference sequence url is not set!"));
    } else if (indexFileNameEdit->text().isEmpty()) {
        QMessageBox::information(this, tr("Build Index"), tr("Index file name is not set!"));
    } else {
        genomePath.clear();
        genomePath = refSeqEdit->text();

        QDialog::accept();
    }
}

const GUrl BuildIndexDialog::getRefSeqUrl() {
    return refSeqEdit->text();
}

const QString BuildIndexDialog::getAlgorithmName() {
    return methodNamesBox->currentText();
}

const QString BuildIndexDialog::getIndexFileName() {
    return indexFileNameEdit->text();
}

QMap<QString, QVariant> BuildIndexDialog::getCustomSettings() {
    if (customGUI != nullptr) {
        return customGUI->getBuildIndexCustomSettings();
    } else {
        return QMap<QString, QVariant>();
    }
}

}  // namespace U2
