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

#include <math.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>

#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/GenbankLocationParser.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/U2LongLongValidator.h>

#include "GenbankLocationValidator.h"
#include "RangeSelector.h"
#include "ui_RangeSelectionDialog.h"

namespace U2 {

void RangeSelector::init() {
    int w = qMax(((int)log10((double)rangeEnd)) * 10, 70);
    startEdit = new QLineEdit(this);
    startEdit->setValidator(new U2LongLongValidator(1, len, startEdit));
    if (dialog == nullptr) {
        startEdit->setFixedWidth(w);
    } else {
        startEdit->setMinimumWidth(w);
    }

    startEdit->setAlignment(Qt::AlignRight);
    startEdit->setText(QString::number(rangeStart));
    connect(startEdit, SIGNAL(returnPressed()), SLOT(sl_onReturnPressed()));

    endEdit = new QLineEdit(this);
    endEdit->setValidator(new U2LongLongValidator(1, len, endEdit));
    if (dialog == nullptr) {
        endEdit->setFixedWidth(w);
    } else {
        endEdit->setMinimumWidth(w);
    }

    endEdit->setAlignment(Qt::AlignRight);
    endEdit->setText(QString::number(rangeEnd));
    connect(endEdit, SIGNAL(returnPressed()), SLOT(sl_onReturnPressed()));

    minButton = new QToolButton(this);
    minButton->setText(tr("Min"));
    connect(minButton, SIGNAL(clicked(bool)), SLOT(sl_onMinButtonClicked(bool)));

    maxButton = new QToolButton(this);
    maxButton->setText(tr("Max"));
    connect(maxButton, SIGNAL(clicked(bool)), SLOT(sl_onMaxButtonClicked(bool)));

    if (dialog != nullptr) {
        minButton->setShortcut(QKeySequence(Qt::ALT | Qt::Key_Z));
        maxButton->setShortcut(QKeySequence(Qt::ALT | Qt::Key_X));
    }

    auto l = new QHBoxLayout(this);
    if (dialog == nullptr) {
        l->setContentsMargins(5, 0, 5, 0);
        l->setSizeConstraint(QLayout::SetFixedSize);
    } else {
        l->setContentsMargins(0, 0, 0, 0);
    }

    rangeLabel = new QLabel(tr("Range:"), this);
    rangeLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    startEdit->setObjectName("start_edit_line");
    endEdit->setObjectName("end_edit_line");
    minButton->setObjectName("min_val_button");
    maxButton->setObjectName("max_val_button");
    this->setObjectName("range_selector");

    l->addWidget(rangeLabel);
    l->addWidget(minButton);
    l->addWidget(startEdit);
    l->addWidget(new QLabel(tr("-"), this));
    l->addWidget(endEdit);
    l->addWidget(maxButton);

    setLayout(l);
}

RangeSelector::RangeSelector(QDialog* dialog, qint64 rangeStart, qint64 rangeEnd, qint64 len, bool autoClose)
    : QWidget(dialog), rangeStart(rangeStart), rangeEnd(rangeEnd), len(len), startEdit(nullptr), endEdit(nullptr),
      minButton(nullptr), maxButton(nullptr), rangeLabel(nullptr), dialog(dialog), autoClose(autoClose) {
    init();

    auto okButton = new QPushButton(this);
    okButton->setText(tr("OK"));
    okButton->setDefault(true);
    okButton->setObjectName("ok_button");
    connect(okButton, SIGNAL(clicked(bool)), SLOT(sl_onGoButtonClicked(bool)));

    auto cancelButton = new QPushButton(this);
    cancelButton->setText(tr("Cancel"));
    cancelButton->setObjectName("cancel_button");
    connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));

    auto l3 = new QHBoxLayout();
    l3->setContentsMargins(0, 0, 0, 0);
    l3->addStretch();
    l3->addWidget(okButton);
    l3->addWidget(cancelButton);

    assert(dialog != nullptr);
    auto l2 = new QVBoxLayout();
    l2->addWidget(this);
    l2->addStretch();
    l2->addLayout(l3);

    dialog->setObjectName("range_selection_dialog");
    dialog->setLayout(l2);
    dialog->resize(l2->minimumSize());
}

void RangeSelector::sl_onGoButtonClicked(bool checked) {
    Q_UNUSED(checked);
    exec();
}

void RangeSelector::sl_onReturnPressed() {
    exec();
}

void RangeSelector::exec() {
    bool ok = false;
    qint64 v1 = startEdit->text().toLongLong(&ok);
    if (!ok || v1 < 1 || v1 > len) {
        return;
    }
    qint64 v2 = endEdit->text().toLongLong(&ok);
    if (!ok || v2 < v1 || v2 > len) {
        return;
    }

    emit si_rangeChanged(v1, v2);

    if (dialog != nullptr && autoClose) {
        dialog->accept();
    }
}

void RangeSelector::sl_onMinButtonClicked(bool checked) {
    Q_UNUSED(checked);
    startEdit->setText(QString::number(1));
}

void RangeSelector::sl_onMaxButtonClicked(bool checked) {
    Q_UNUSED(checked);
    endEdit->setText(QString::number(len));
}

qint64 RangeSelector::getStart() const {
    bool ok = false;
    qint64 v = startEdit->text().toLongLong(&ok);
    assert(ok);
    return v;
}

qint64 RangeSelector::getEnd() const {
    bool ok = false;
    qint64 v = endEdit->text().toLongLong(&ok);
    assert(ok);
    return v;
}

MultipleRangeSelector::MultipleRangeSelector(QWidget* _parent, const QVector<U2Region>& _regions, qint64 _seqLen, bool _isCircular)
    : QDialog(_parent), seqLen(_seqLen), selectedRanges(_regions), isCircular(_isCircular) {
    ui = new Ui_RangeSelectionDialog;
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "65929414");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Go"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    ui->startEdit->setValidator(new U2LongLongValidator(1, seqLen, ui->startEdit));
    ui->endEdit->setValidator(new U2LongLongValidator(1, seqLen, ui->endEdit));

    int w = qMax(((int)log10((double)seqLen)) * 10, 70);

    ui->startEdit->setMinimumWidth(w);
    ui->endEdit->setMinimumWidth(w);

    if (selectedRanges.isEmpty()) {
        ui->startEdit->setText(QString::number(1));
        ui->endEdit->setText(QString::number(seqLen));
    } else {
        U2Region firstRegion = selectedRanges.first();
        ui->startEdit->setText(QString::number(firstRegion.startPos + 1));
        ui->endEdit->setText(QString::number(firstRegion.endPos()));
    }

    QString loc;
    if (selectedRanges.isEmpty()) {
        loc = QString("1..%1").arg(seqLen);
    } else {
        loc = U1AnnotationUtils::buildLocationString(selectedRanges);
    }
    ui->multipleRegionEdit->setText(loc);

    ui->minButton->setShortcut(QKeySequence(Qt::ALT | Qt::Key_Z));
    ui->maxButton->setShortcut(QKeySequence(Qt::ALT | Qt::Key_X));
    connect(ui->miltipleButton, SIGNAL(toggled(bool)), SLOT(sl_multipleButtonToggled(bool)));

    connect(ui->startEdit, SIGNAL(returnPressed()), SLOT(sl_returnPressed()));
    connect(ui->endEdit, SIGNAL(returnPressed()), SLOT(sl_returnPressed()));
    connect(ui->startEdit, SIGNAL(textEdited(QString)), SLOT(sl_textEdited(const QString&)));
    connect(ui->endEdit, SIGNAL(textEdited(QString)), SLOT(sl_textEdited(const QString&)));
    connect(ui->startEdit, SIGNAL(textChanged(QString)), SLOT(sl_textEdited(const QString&)));
    connect(ui->endEdit, SIGNAL(textChanged(QString)), SLOT(sl_textEdited(const QString&)));

    connect(ui->multipleRegionEdit, SIGNAL(returnPressed()), SLOT(sl_returnPressed()));
    connect(ui->minButton, SIGNAL(clicked()), SLOT(sl_minButton()));
    connect(ui->maxButton, SIGNAL(clicked()), SLOT(sl_maxButton()));

    connect(ui->buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), SLOT(sl_buttonClicked(QAbstractButton*)));
    ui->singleButton->toggle();
    sl_buttonClicked(ui->singleButton);
}

MultipleRangeSelector::~MultipleRangeSelector() {
    delete ui->multipleRegionEdit->validator();
    delete ui;
}

void MultipleRangeSelector::accept() {
    if (ui->singleButton->isChecked()) {
        bool ok = false;
        qint64 v1 = ui->startEdit->text().toLongLong(&ok);
        if (!ok || v1 < 1 || v1 > seqLen) {
            return;
        }
        qint64 v2 = ui->endEdit->text().toLongLong(&ok);
        if (!ok || (v2 < v1 && !isCircular) || v2 > seqLen) {
            return;
        }
    } else {
        QByteArray locEditText = ui->multipleRegionEdit->text().toLatin1();
        U2Location currentLocation;
        if (isCircular) {
            Genbank::LocationParser::parseLocation(locEditText.constData(), ui->multipleRegionEdit->text().length(), currentLocation, seqLen);
        } else {
            Genbank::LocationParser::parseLocation(locEditText.constData(), ui->multipleRegionEdit->text().length(), currentLocation, -1);
        }
        if (currentLocation->isEmpty()) {
            return;
        }
    }
    QDialog::accept();
}

void MultipleRangeSelector::sl_textEdited(const QString&) {
    qint64 min = ui->startEdit->text().toLongLong();
    qint64 max = ui->endEdit->text().toLongLong();
    QPalette p = normalPalette;
    if (min > max && !isCircular) {
        p.setColor(QPalette::Base, QColor(255, 200, 200));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    } else {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    ui->startEdit->setPalette(p);
    ui->endEdit->setPalette(p);
}

void MultipleRangeSelector::sl_buttonClicked(QAbstractButton* b) {
    bool singleMode = b == ui->singleButton;

    ui->labelS1->setEnabled(singleMode);
    ui->labelS2->setEnabled(singleMode);
    ui->startEdit->setEnabled(singleMode);
    ui->endEdit->setEnabled(singleMode);
    ui->minButton->setEnabled(singleMode);
    ui->maxButton->setEnabled(singleMode);

    ui->labelM1->setEnabled(!singleMode);
    ui->multipleRegionEdit->setEnabled(!singleMode);
}

QVector<U2Region> MultipleRangeSelector::getSelectedRegions() {
    QVector<U2Region> currentRegions;

    if (ui->singleButton->isChecked()) {
        bool ok = false;
        qint64 st = ui->startEdit->text().toLongLong(&ok);
        CHECK(ok, currentRegions);

        qint64 en = ui->endEdit->text().toLongLong(&ok);
        CHECK(ok, currentRegions);

        if (isCircular && st > en) {
            currentRegions.append(U2Region(0, en));
            currentRegions.append(U2Region(st - 1, seqLen - st + 1));
        } else {
            currentRegions.append(U2Region(st - 1, en - st + 1));
        }
    } else {
        QByteArray locEditText = ui->multipleRegionEdit->text().toLatin1();
        U2Location currentLocation;
        if (isCircular) {
            Genbank::LocationParser::parseLocation(locEditText.constData(), ui->multipleRegionEdit->text().length(), currentLocation, seqLen);
        } else {
            Genbank::LocationParser::parseLocation(locEditText.constData(), ui->multipleRegionEdit->text().length(), currentLocation, -1);
        }
        currentRegions = currentLocation->regions;
    }

    return currentRegions;
}

void MultipleRangeSelector::sl_minButton() {
    ui->startEdit->setText(QString::number(1));
}

void MultipleRangeSelector::sl_maxButton() {
    ui->endEdit->setText(QString::number(seqLen));
}

void MultipleRangeSelector::sl_returnPressed() {
    accept();
}

void MultipleRangeSelector::sl_multipleButtonToggled(bool toggleState) {
    if (toggleState) {
        QValidator* v = new GenbankLocationValidator(ui->buttonBox->button(QDialogButtonBox::Ok), seqLen, isCircular, ui->multipleRegionEdit);
        ui->multipleRegionEdit->setValidator(v);
        int size = ui->multipleRegionEdit->text().size();
        QString toValidate = ui->multipleRegionEdit->text();
        v->validate(toValidate, size);
    } else {
        delete ui->multipleRegionEdit->validator();
        ui->multipleRegionEdit->setValidator(nullptr);
    }
}

}  // namespace U2
