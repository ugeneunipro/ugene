/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include "EditFragmentDialog.h"

#include <QDir>
#include <QMessageBox>

#include <U2Algorithm/EnzymeModel.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/Task.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/HelpButton.h>

#include "EnzymesIO.h"

namespace U2 {

EditFragmentDialog::EditFragmentDialog(DNAFragment& fragment, QWidget* p)
    : QDialog(p),
      dnaFragment(fragment),
      comboBoxItems({{tr("Blunt"), OverhangType::Blunt}, {tr("Sticky"), OverhangType::Sticky}}) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930769");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    QList<QComboBox*> cbList = {cbLeftEndType, cbRightEndType};
    const auto& cbItemKeys = comboBoxItems.keys();
    for (auto comboBox : qAsConst(cbList)) {
        for (const auto& key : qAsConst(cbItemKeys)) {
            comboBox->addItem(key, QVariant((int)comboBoxItems[key]));
        }
    }

    static const int REGION_LEN = 10;

    U2OpStatusImpl os;
    seq = dnaFragment.getSequence(os);
    SAFE_POINT_OP(os, );

    transl = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(dnaFragment.getAlphabet());
    QByteArray data(seq.toLatin1());
    transl->translate(data.data(), data.length());
    trseq = QString(data);

    if (seq.length() > REGION_LEN * 2) {
        QString leftSeqPart = seq.mid(0, REGION_LEN);
        QString rightSeqPart = seq.mid(seq.length() - REGION_LEN - 1);
        seq = QString("%1 ... %2").arg(leftSeqPart).arg(rightSeqPart);

        leftSeqPart = trseq.mid(0, REGION_LEN);
        rightSeqPart = trseq.mid(trseq.length() - REGION_LEN - 1);
        trseq = QString("%1 ... %2").arg(leftSeqPart).arg(rightSeqPart);
    }

    const DNAFragmentTerm& leftTerm = dnaFragment.getLeftTerminus();
    const DNAFragmentTerm& rightTerm = dnaFragment.getRightTerminus();

    if (leftTerm.isDirect) {
        lDirectRadioButton->setChecked(true);
        lDirectOverhangEdit->setText(leftTerm.overhang);
        lDirectOverhangEdit->setEnabled(true);
        lComplOverhangEdit->setEnabled(false);
    } else {
        lComplRadioButton->setChecked(true);
        QByteArray buf = leftTerm.overhang;
        transl->translate(buf.data(), buf.length());
        lComplOverhangEdit->setText(buf);
        lDirectOverhangEdit->setEnabled(false);
        lComplOverhangEdit->setEnabled(true);
    }

    if (rightTerm.isDirect) {
        rDirectRadioButton->setChecked(true);
        rDirectOverhangEdit->setText(rightTerm.overhang);
        rDirectOverhangEdit->setEnabled(true);
        rComplOverhangEdit->setEnabled(false);
    } else {
        rComplRadioButton->setChecked(true);
        QByteArray buf = rightTerm.overhang;
        transl->translate(buf.data(), buf.size());
        rComplOverhangEdit->setText(buf);
        rDirectOverhangEdit->setEnabled(false);
        rComplOverhangEdit->setEnabled(true);
    }

    connect(cbLeftEndType, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        lStickyWidget->setEnabled((OverhangType)index == OverhangType::Sticky);
        sl_updatePreview();
    });
    connect(cbRightEndType, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        rStickyWidget->setEnabled((OverhangType)index == OverhangType::Sticky);
        sl_updatePreview();
    });
    connect(lDirectRadioButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(lComplRadioButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(rDirectRadioButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(rComplRadioButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(lResetButton, SIGNAL(clicked()), SLOT(sl_onLeftResetClicked()));
    connect(rResetButton, SIGNAL(clicked()), SLOT(sl_onRightResetClicked()));

    connect(lDirectOverhangEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_updatePreview()));
    connect(lComplOverhangEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_updatePreview()));
    connect(rDirectOverhangEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_updatePreview()));
    connect(rComplOverhangEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_updatePreview()));

    bool isLeftSticky = leftTerm.type == OVERHANG_TYPE_STICKY;
    cbLeftEndType->setCurrentIndex(isLeftSticky ? (int)OverhangType::Sticky : (int)OverhangType::Blunt);

    bool isRightSticky = rightTerm.type == OVERHANG_TYPE_STICKY;
    cbRightEndType->setCurrentIndex(isRightSticky ? (int)OverhangType::Sticky : (int)OverhangType::Blunt);

    updatePreview();
}

void EditFragmentDialog::accept() {
    bool leftSticky = cbLeftEndType->currentIndex() == (int)OverhangType::Sticky;
    QByteArray lTermType = leftSticky ? OVERHANG_TYPE_STICKY : OVERHANG_TYPE_BLUNT;
    dnaFragment.setLeftTermType(lTermType);

    bool rightSticky = cbRightEndType->currentIndex() == (int)OverhangType::Sticky;
    QByteArray rTermType = rightSticky ? OVERHANG_TYPE_STICKY : OVERHANG_TYPE_BLUNT;
    dnaFragment.setRightTermType(rTermType);

    if (leftSticky) {
        QLineEdit* lCustomOverhangEdit = lDirectRadioButton->isChecked() ? lDirectOverhangEdit : lComplOverhangEdit;
        QString leftOverhang = lCustomOverhangEdit->text();

        if (leftOverhang.isEmpty()) {
            QMessageBox::warning(this, windowTitle(), tr("Left overhang is empty. Please enter the overhang or set blunt left end."));
            return;
        }
        if (!isValidOverhang(leftOverhang)) {
            QMessageBox::warning(this, windowTitle(), tr("Invalid left overhang: unsupported alphabet!"));
            lCustomOverhangEdit->setFocus();
            lCustomOverhangEdit->setSelection(0, leftOverhang.length());
            return;
        }

        QByteArray bLeftOverhang = leftOverhang.toUpper().toLatin1();
        if (lComplRadioButton->isChecked()) {
            transl->translate(bLeftOverhang.data(), bLeftOverhang.size());
        }
        dnaFragment.setLeftOverhang(bLeftOverhang);
        dnaFragment.setLeftOverhangStrand(lDirectRadioButton->isChecked());
    }

    if (rightSticky) {
        QLineEdit* rCustomOverhangEdit = rDirectRadioButton->isChecked() ? rDirectOverhangEdit : rComplOverhangEdit;
        QString rightOverhang = rCustomOverhangEdit->text();

        if (rightOverhang.isEmpty()) {
            QMessageBox::warning(this, windowTitle(), tr("Right overhang is empty. Please enter the overhang or set blunt right end."));
            return;
        }
        if (!isValidOverhang(rightOverhang)) {
            QMessageBox::warning(this, windowTitle(), tr("Invalid right overhang: unsupported alphabet!"));
            rCustomOverhangEdit->setFocus();
            rCustomOverhangEdit->setSelection(0, rightOverhang.length());
            return;
        }

        QByteArray bRightOverhang = rightOverhang.toUpper().toLatin1();
        if (rComplRadioButton->isChecked()) {
            transl->translate(bRightOverhang.data(), bRightOverhang.size());
        }
        dnaFragment.setRightOverhang(bRightOverhang);
        dnaFragment.setRightOverhangStrand(rDirectRadioButton->isChecked());
    }

    QDialog::accept();
}

void EditFragmentDialog::updatePreview() {
    QString preview;
    QString invertedStr = dnaFragment.isInverted() ? tr(" (INVERTED)") : QString();

    preview += tr("Fragment of %1%2<br>").arg(dnaFragment.getSequenceDocName()).arg(invertedStr);

    QString uLeftOverhang, bLeftOverhang, uRightOverhang, bRightOverhang;

    if (cbLeftEndType->currentIndex() == (int)OverhangType::Sticky) {
        uLeftOverhang = lDirectRadioButton->isChecked() ? lDirectOverhangEdit->text().toUpper() : QByteArray();
        bLeftOverhang = lComplRadioButton->isChecked() ? lComplOverhangEdit->text().toUpper() : QByteArray();
    }

    if (cbRightEndType->currentIndex() == (int)OverhangType::Sticky) {
        uRightOverhang = rDirectRadioButton->isChecked() ? rDirectOverhangEdit->text().toUpper() : QByteArray();
        bRightOverhang = rComplRadioButton->isChecked() ? rComplOverhangEdit->text().toUpper() : QByteArray();
    }
    preview += ("<table cellspacing=\"10\" >");
    preview += tr("<tr> <td align=\"center\"> 5' </td><td></td> <td align=\"center\"> 3' </td> </tr>");

    preview += QString("<tr> <td align=\"center\" >%1</td><td align=\"center\" >%2</td><td align=\"center\" >%3</td> </tr>").arg(uLeftOverhang).arg(seq).arg(uRightOverhang);
    preview += QString("<tr> <td align=\"center\" >%1</td><td align=\"center\" >%2</td><td align=\"center\" >%3</td> </tr>").arg(bLeftOverhang).arg(trseq).arg(bRightOverhang);
    preview += tr("<tr> <td align=\"center\"> 3' </td><td></td> <td align=\"center\"> 5' </td> </tr>");
    preview += ("</table>");
    previewEdit->setText(preview);
}

void EditFragmentDialog::sl_updatePreview() {
    updatePreview();
}

void EditFragmentDialog::sl_customOverhangSet(const QString&) {
    updatePreview();
}

void EditFragmentDialog::sl_onLeftResetClicked() {
    resetLeftOverhang();
    updatePreview();
}

void EditFragmentDialog::sl_onRightResetClicked() {
    resetRightOverhang();
    updatePreview();
}

bool EditFragmentDialog::isValidOverhang(const QString& text) {
    QByteArray seq(text.toLatin1());
    const DNAAlphabet* alph = U2AlphabetUtils::findBestAlphabet(seq);
    return alph != nullptr && alph->isNucleic() ? true : false;
}

void EditFragmentDialog::resetLeftOverhang() {
    const auto& leftTerminus = dnaFragment.getLeftTerminus();
    if (leftTerminus.type == OVERHANG_TYPE_BLUNT) {
        cbLeftEndType->setCurrentIndex((int)OverhangType::Blunt);
        lStickyWidget->setDisabled(true);
    } else {
        cbLeftEndType->setCurrentIndex((int)OverhangType::Sticky);
        if (leftTerminus.isDirect) {
            lDirectRadioButton->setChecked(true);
            lDirectOverhangEdit->setEnabled(true);
            lDirectOverhangEdit->setText(leftTerminus.overhang);
            lComplOverhangEdit->setEnabled(false);
            lComplOverhangEdit->clear();
        } else {
            lComplRadioButton->setChecked(true);
            lDirectOverhangEdit->setEnabled(false);
            lDirectOverhangEdit->clear();
            lComplOverhangEdit->setEnabled(true);
            QByteArray overhang = leftTerminus.overhang;
            transl->translate(overhang.data(), overhang.length());
            lComplOverhangEdit->setText(overhang);
        }
    }
}

void EditFragmentDialog::resetRightOverhang() {
    const auto& rightTerminus = dnaFragment.getRightTerminus();
    if (rightTerminus.type == OVERHANG_TYPE_BLUNT) {
        cbRightEndType->setCurrentIndex((int)OverhangType::Blunt);
        rStickyWidget->setDisabled(true);
    } else {
        cbRightEndType->setCurrentIndex((int)OverhangType::Sticky);
        if (rightTerminus.isDirect) {
            rDirectRadioButton->setChecked(true);
            rDirectOverhangEdit->setEnabled(true);
            rDirectOverhangEdit->setText(rightTerminus.overhang);
            rComplOverhangEdit->setEnabled(false);
            rComplOverhangEdit->clear();
        } else {
            rComplRadioButton->setChecked(true);
            rDirectOverhangEdit->setEnabled(false);
            rDirectOverhangEdit->clear();
            rComplOverhangEdit->setEnabled(true);
            QByteArray overhang = rightTerminus.overhang;
            transl->translate(overhang.data(), overhang.length());
            rComplOverhangEdit->setText(overhang);
        }
    }
}

}  // namespace U2
