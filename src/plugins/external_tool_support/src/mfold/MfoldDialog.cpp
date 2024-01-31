/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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
#include "MfoldDialog.h"

#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QScopedPointer>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/RegionSelector.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

namespace U2 {
const QString tooLongRegionStr = QT_TR_NOOP("Region cannot be larger than 3000 nucleotides");

MfoldDialog::MfoldDialog(const ADVSequenceObjectContext& ctx)
    : QDialog(ctx.getAnnotatedDNAView()->getWidget()), seqLen(ctx.getSequenceLength()),
      isCircular(ctx.getSequenceObject()->isCircular()) {
    ui.setupUi(this);
    initRegionSelector(ctx.getSequenceSelection());
    initOutputTab();
    ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Run"));
    new HelpButton(this, ui.buttonBox, "96666017");
}

void MfoldDialog::initRegionSelector(DNASequenceSelection* seqSelection) {
    regionSelector = new RegionSelector(this, seqLen, true, seqSelection, isCircular);
    ui.mfoldTabLayout->insertWidget(2, regionSelector);

    auto lineEdits = regionSelector->getLineEdits();
    startEdit = lineEdits.first;
    endEdit = lineEdits.second;
    validateRegionAndShowError();
    connect(startEdit, &QLineEdit::textChanged, this, &MfoldDialog::validateRegionAndShowError);
    connect(endEdit, &QLineEdit::textChanged, this, &MfoldDialog::validateRegionAndShowError);
}

void MfoldDialog::initOutputTab() {
    connect(ui.notSaveBtnLabel, &ClickableLabel::clicked, this, &MfoldDialog::checkNotSave);
    connect(ui.saveBtnLabel, &ClickableLabel::clicked, this, &MfoldDialog::checkSave);
    ui.outPathLineEdit->setText(FileAndDirectoryUtils::getAbsolutePath(
        AppContext::getAppSettings()->getUserAppsSettings()->getDefaultDataDirPath()));
    connect(ui.browsePushButton, &QAbstractButton::clicked, this, [this]() {
        QString dir = U2FileDialog::getExistingDirectory(this, tr("Select a folder"), ui.outPathLineEdit->text());
        if (!dir.isEmpty()) {
            ui.outPathLineEdit->setText(dir);
            checkSave();
        }
    });
    // todo use PathLineEdit?
}

void MfoldDialog::validateRegionAndShowError() {
    QString err;
    bool startOk, endOk;
    auto startVal = startEdit->text().toULongLong(&startOk);
    auto endVal = endEdit->text().toULongLong(&endOk);

    // Returns region len taking into account that the sequence can be circular. Doesn't validate region.
    auto getRegionLen = [this](qulonglong start, qulonglong end) {
        return end >= start ? end - start + 1 : seqLen - start + end + 1;
    };

    if (startEdit->text().isEmpty()) {
        err = tr("Start position not specified");
    } else if (endEdit->text().isEmpty()) {
        err = tr("End position not specified");
    } else if (!startOk) {
        err = tr("Start position was overflowed");
    } else if (!endOk) {
        err = tr("End position was overflowed");
    } else if (!isCircular && startVal > endVal) {
        err = tr("Start position cannot be greater than end position");
    } else if (getRegionLen(startVal, endVal) > 3000) {
        err = tooLongRegionStr;
    } else {
        err = "";
    }
    ui.wrongRegionLabel->setText(err);
}

void MfoldDialog::checkNotSave() {
    ui.notSaveRadioButton->setChecked(true);
    ui.saveRadioButton->setChecked(false);
}
void MfoldDialog::checkSave() {
    ui.notSaveRadioButton->setChecked(false);
    ui.saveRadioButton->setChecked(true);
}

MfoldSettings MfoldDialog::getSettings() const {
    MfoldSettings settings;
    settings.labFr = ui.labFrSpinBox->value();
    settings.maxBp = ui.maxBpSpinBox->value();
    settings.maxFold = ui.maxSpinBox->value();
    settings.mgConc = ui.mgDoubleSpinBox->value();
    settings.naConc = ui.naDoubleSpinBox->value();
    settings.outPath = ui.saveRadioButton->isChecked() ? ui.outPathLineEdit->text() : "";
    settings.percent = ui.pSpinBox->value();
    settings.region = regionSelector->getRegion();
    settings.rotAng = ui.rotAngDoubleSpinBox->value();
    settings.temperature = ui.tSpinBox->value();
    settings.window = ui.wSpinBox->value();
    return settings;
}

void MfoldDialog::accept() {
    bool isRegionOk;
    auto region = regionSelector->getRegion(&isRegionOk);
    if (!isRegionOk) {
        regionSelector->showErrorMessage();
        regionSelector->setFocus(Qt::OtherFocusReason);
        return;
    }
    if (region.length > 3000) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::NoIcon,
                                                                   L10N::errorTitle(),
                                                                   tr("Invalid sequence region!"),
                                                                   QMessageBox::Ok,
                                                                   this);
        msgBox->setInformativeText(tooLongRegionStr);
        msgBox->exec();
        regionSelector->setFocus(Qt::OtherFocusReason);
        return;
    }
    auto isDirValid = [](const QString& dirPath) {
        auto absPath = FileAndDirectoryUtils::getAbsolutePath(dirPath);
        return !absPath.isEmpty() && FileAndDirectoryUtils::canWriteToPath(absPath);
    };
    if (ui.saveRadioButton->isChecked() && !isDirValid(ui.outPathLineEdit->text())) {
        QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::NoIcon,
                                                                   L10N::errorTitle(),
                                                                   tr("Invalid out path!"),
                                                                   QMessageBox::Ok,
                                                                   this);
        msgBox->setInformativeText(tr("Output dir is read-only"));
        msgBox->exec();
        ui.outPathLineEdit->setFocus(Qt::OtherFocusReason);
        return;
    }
    QDialog::accept();
}
}  // namespace U2
