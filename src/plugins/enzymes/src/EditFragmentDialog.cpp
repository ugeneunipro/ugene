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

#include <QMessageBox>
#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/Task.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Algorithm/EnzymeModel.h>
#include "EnzymesIO.h"
#include "EditFragmentDialog.h"
#include <U2Gui/HelpButton.h>

namespace U2 {




EditFragmentDialog::EditFragmentDialog( DNAFragment& fragment, QWidget* p )
: QDialog(p), dnaFragment(fragment)
{

    setupUi(this);
    new HelpButton(this, buttonBox, "24748966");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    connect(lBluntButton, SIGNAL(toggled(bool)), SLOT(sl_onLeftBluntButtonToogled(bool)));
    connect(rBluntButton, SIGNAL(toggled(bool)), SLOT(sl_onRightBluntButtonToggled(bool)));

    static const int REGION_LEN = 10;

    U2OpStatusImpl os;
    seq = dnaFragment.getSequence(os);
    SAFE_POINT_OP(os, );

    transl = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(dnaFragment.getAlphabet());
    QByteArray data(seq.toLatin1());
    transl->translate(data.data(), data.length());
    trseq = QString(data);

    if (seq.length() > REGION_LEN*2) {
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
    } else {
        lComplRadioButton->setChecked(true);
        QByteArray buf = leftTerm.overhang;
        transl->translate(buf.data(), buf.length());
        lComplOverhangEdit->setText(buf);
    }

    if (rightTerm.isDirect) {
        rDirectRadioButton->setChecked(true);
        rDirectOverhangEdit->setText(rightTerm.overhang);
    } else {
        rComplRadioButton->setChecked(true);
        QByteArray buf = rightTerm.overhang;
        transl->translate(buf.data(), buf.size());
        rComplOverhangEdit->setText(buf);
    }

    leftTerm.type == OVERHANG_TYPE_BLUNT ? lBluntButton->toggle() : lStickyButton->toggle();
    rightTerm.type == OVERHANG_TYPE_BLUNT ? rBluntButton->toggle() : rStickyButton->toggle();

    updatePreview();

    connect(rStickyButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(rBluntButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(lStickyButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(lBluntButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(lDirectRadioButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(lComplRadioButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(rDirectRadioButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(rComplRadioButton, SIGNAL(clicked()), SLOT(sl_updatePreview()));
    connect(lResetButton, SIGNAL(clicked()), SLOT(sl_onLeftResetClicked()));
    connect(rResetButton, SIGNAL(clicked()), SLOT(sl_onRightResetClicked()));

    connect(lDirectOverhangEdit, SIGNAL(textChanged ( const QString& )), SLOT(sl_updatePreview()));
    connect(lComplOverhangEdit, SIGNAL(textChanged ( const QString& )), SLOT(sl_updatePreview()));
    connect(rDirectOverhangEdit, SIGNAL(textChanged ( const QString& )), SLOT(sl_updatePreview()));
    connect(rComplOverhangEdit, SIGNAL(textChanged ( const QString& )), SLOT(sl_updatePreview()));

}

void EditFragmentDialog::accept()
{
    QByteArray lTermType = lBluntButton->isChecked() ? OVERHANG_TYPE_BLUNT : OVERHANG_TYPE_STICKY;
    dnaFragment.setLeftTermType(lTermType);

    QByteArray rTermType = rBluntButton->isChecked() ? OVERHANG_TYPE_BLUNT : OVERHANG_TYPE_STICKY;
    dnaFragment.setRightTermType(rTermType);

    if (lCustomOverhangBox->isChecked() && lStickyButton->isChecked()) {
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
        if ( lComplRadioButton->isChecked() ) {
            transl->translate(bLeftOverhang.data(), bLeftOverhang.size());
        }
        dnaFragment.setLeftOverhang(bLeftOverhang);
        dnaFragment.setLeftOverhangStrand(lDirectRadioButton->isChecked());

    }

    if (rCustomOverhangBox->isChecked() && rStickyButton->isChecked()) {
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
        if ( rComplRadioButton->isChecked() ) {
            transl->translate(bRightOverhang.data(), bRightOverhang.size());
        }
        dnaFragment.setRightOverhang(bRightOverhang);
        dnaFragment.setRightOverhangStrand(rDirectRadioButton->isChecked());
    }

    QDialog::accept();

}

void EditFragmentDialog::updatePreview()
{
    QString preview;
    QString invertedStr = dnaFragment.isInverted() ? tr(" (INVERTED)") : QString();

    preview += tr("Fragment of %1%2<br>").arg(dnaFragment.getSequenceDocName()).arg(invertedStr);

    QString uLeftOverhang, bLeftOverhang, uRightOverhang, bRightOverhang;

    if (!lBluntButton->isChecked()) {
        uLeftOverhang = lDirectRadioButton->isChecked() ? lDirectOverhangEdit->text().toUpper() : QByteArray();
        bLeftOverhang = lComplRadioButton->isChecked() ? lComplOverhangEdit->text().toUpper() : QByteArray();
    }

    if (!rBluntButton->isChecked()){
        uRightOverhang = rDirectRadioButton->isChecked() ? rDirectOverhangEdit->text().toUpper() : QByteArray();
        bRightOverhang = rComplRadioButton->isChecked() ? rComplOverhangEdit->text().toUpper() : QByteArray();

    }
    preview+=("<table cellspacing=\"10\" >");
    preview += tr("<tr> <td align=\"center\"> 5' </td><td></td> <td align=\"center\"> 3' </td> </tr>");

    preview += QString("<tr> <td align=\"center\" >%1</td><td align=\"center\" >%2</td><td align=\"center\" >%3</td> </tr>").
        arg(uLeftOverhang).arg(seq).arg(uRightOverhang);
    preview += QString("<tr> <td align=\"center\" >%1</td><td align=\"center\" >%2</td><td align=\"center\" >%3</td> </tr>").
        arg(bLeftOverhang).arg(trseq).arg(bRightOverhang);
    preview += tr("<tr> <td align=\"center\"> 3' </td><td></td> <td align=\"center\"> 5' </td> </tr>");
    preview+=("</table>");
    previewEdit->setText(preview);
}

void EditFragmentDialog::sl_updatePreview() {
    updatePreview();
}

void EditFragmentDialog::sl_customOverhangSet( const QString& ) {
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

bool EditFragmentDialog::isValidOverhang( const QString& text ) {
    QByteArray seq(text.toLatin1());
    const DNAAlphabet *alph = U2AlphabetUtils::findBestAlphabet(seq);
    return alph != NULL && alph->isNucleic() ? true : false;
}

void EditFragmentDialog::resetLeftOverhang() {
    QByteArray enzymeId = dnaFragment.getLeftTerminus().enzymeId;
    if (enzymeId.isEmpty()) {
        return;
    }
    const QList<SEnzymeData>& enzymes = EnzymesIO::getDefaultEnzymesList();
    SEnzymeData enz = EnzymesIO::findEnzymeById(enzymeId, enzymes);

    int leftCutCompl = enz->seq.length() - enz->cutComplement;
    qint64 cutPos = dnaFragment.getFragmentRegions().first().startPos - qMax(enz->cutDirect, leftCutCompl);
    qint64 leftOverhangStart = cutPos + qMin(enz->cutDirect, leftCutCompl);
    U2OpStatusImpl os;
    QByteArray overhang = dnaFragment.getSourceSequenceRegion( U2Region( leftOverhangStart, dnaFragment.getFragmentRegions().first().startPos - leftOverhangStart), os );
    SAFE_POINT_OP(os, );
    bool isDirect = enz->cutDirect < leftCutCompl;

    if (isDirect) {
        lDirectRadioButton->setChecked(true);
        lDirectOverhangEdit->setText(overhang);
        lComplOverhangEdit->clear();
    } else {
        lComplRadioButton->setChecked(true);
        transl->translate(overhang.data(), overhang.length());
        lDirectOverhangEdit->clear();
        lComplOverhangEdit->setText(overhang);
    }
}

void EditFragmentDialog::resetRightOverhang()
{
    QByteArray enzymeId = dnaFragment.getRightTerminus().enzymeId;
    if (enzymeId.isEmpty()) {
        return;
    }

    const QList<SEnzymeData>& enzymes = EnzymesIO::getDefaultEnzymesList();
    SEnzymeData enz = EnzymesIO::findEnzymeById(enzymeId, enzymes);

    int rightCutCompl = enz->seq.length() - enz->cutComplement;
    int rightCutPos = dnaFragment.getFragmentRegions().last().endPos();
    int enzStart = rightCutPos - qMin(enz->cutDirect, rightCutCompl );
    int rightOverhangStart = enzStart + qMax(enz->cutDirect, rightCutCompl );
    U2OpStatusImpl os;
    QByteArray overhang = dnaFragment.getSourceSequenceRegion( U2Region(rightCutPos, rightOverhangStart - rightCutPos), os );
    SAFE_POINT_OP(os, );
    bool isDirect = enz->cutDirect > rightCutCompl;

    if (isDirect) {
        rDirectRadioButton->setChecked(true);
        rDirectOverhangEdit->setText(overhang);
        rComplOverhangEdit->clear();
    } else {
        rComplRadioButton->setChecked(true);
        transl->translate(overhang.data(), overhang.length());
        rDirectOverhangEdit->clear();
        rComplOverhangEdit->setText(overhang);
    }
}




} // namespace
