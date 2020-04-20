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

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/L10n.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Formats/FastaFormat.h>

#include "SeqPasterWidgetController.h"
#include "ui_SeqPasterWidget.h"

namespace U2{

SeqPasterWidgetController::SeqPasterWidgetController( QWidget *p, const QByteArray& initText, bool needWarning ):
QWidget(p), preferred(0), additionalWarning(needWarning), allowFastaFormatMode(false)
{
    ui = new Ui_SeqPasterWidget;
    //TODO: add not null project checking
    ui->setupUi(this);
    ui->symbolToReplaceEdit->setMaximumSize(ui->symbolToReplaceEdit->height(), ui->symbolToReplaceEdit->height());

    if (!initText.isEmpty()) {
        ui->sequenceEdit->setPlainText(initText);
    }

    QList<const DNAAlphabet*> alps = AppContext::getDNAAlphabetRegistry()->getRegisteredAlphabets();
    foreach(const DNAAlphabet *a, alps){
        ui->alphabetBox->addItem(a->getName(), a->getId());
    }
    connect(ui->alphabetBox, SIGNAL(currentIndexChanged(const QString&)), SLOT(sl_currentIndexChanged(const QString&)));
}
QByteArray SeqPasterWidgetController::getNormSequence(const DNAAlphabet * alph, const QByteArray & seq, bool replace, QChar replaceChar) {
    assert(alph != NULL);
    QByteArray ret;
    if(alph->getId() == BaseDNAAlphabetIds::RAW()){
        foreach(QChar c, seq){
            QChar::Category cat = c.category();
            if((cat != QChar::Separator_Space) &&
                (cat != QChar::Other_Control)){
                    ret.append(c);
            }
        }
    }else{
        QByteArray alphabetSymbols = alph->getAlphabetChars(true);
        for(int i = 0; i < seq.size(); i++){
            QChar c = seq.at(i);
            if(alphabetSymbols.indexOf(c) > -1){
                ret.append(c);
            }else{
                if (replace){
                    ret.append(replaceChar);
                }
            }
        }
    }
    if(!alph->isCaseSensitive()){
        ret = ret.toUpper();
    }
    return ret;
}

typedef QPair<QString, QString> StrStrPair;

QString SeqPasterWidgetController::validate(){
    QString data = ui->sequenceEdit->document()->toPlainText();
    CHECK(!data.isEmpty(), tr("Input sequence is empty"));

    resultSequences.clear();

    if (allowFastaFormatMode && isFastaFormat(data.left(1000))) {
        U2OpStatusImpl os;
        QList<StrStrPair> sequences = FastaFormat::getSequencesAndNamesFromUserInput(ui->sequenceEdit->document()->toPlainText(), os);
        CHECK_OP(os, os.getError());
        foreach (const StrStrPair &sequence, sequences) {
            const QString errorMessage = addSequence(sequence.first, sequence.second);
            CHECK(errorMessage.isEmpty(), errorMessage);
        }
    } else {
        return addSequence("", data);
    }
    return "";
}

QList<DNASequence> SeqPasterWidgetController::getSequences() const {
    return resultSequences;
}

void SeqPasterWidgetController::sl_currentIndexChanged( const QString& newText){
    DNAAlphabetRegistry *r = AppContext::getDNAAlphabetRegistry();
    if((r->findById(BaseDNAAlphabetIds::RAW()))->getName() == newText){
        ui->skipRB->setDisabled(true);
        ui->replaceRB->setDisabled(true);
        ui->symbolToReplaceEdit->setDisabled(true);
    }else{
        ui->skipRB->setEnabled(true);
        ui->replaceRB->setEnabled(true);
        ui->symbolToReplaceEdit->setEnabled(true);
    }
}

QString SeqPasterWidgetController::addSequence(const QString &name, QString data) {
    QByteArray seq = data.remove(QRegExp("\\s")).toLatin1();

    const DNAAlphabet *alph = NULL;
    if (ui->groupBox->isChecked()) {
        alph = U2AlphabetUtils::getById((ui->alphabetBox->itemData(ui->alphabetBox->currentIndex())).toString());
    } else {
        alph = NULL != preferred ? preferred : U2AlphabetUtils::findBestAlphabet(seq);
    }
    CHECK(NULL != alph, tr("Alphabet not detected"));

    bool replace = ui->replaceRB->isChecked();
    if (replace) {
        CHECK(!ui->symbolToReplaceEdit->text().isEmpty(), tr("Replace symbol is empty"));
        char cc = ui->symbolToReplaceEdit->text().at(0).toLatin1();
        CHECK(alph->contains(cc), tr("Replace symbol is not belongs to selected alphabet"));
    }

    QChar replaceChar = replace ? ui->symbolToReplaceEdit->text().at(0) : QChar();
    QByteArray normSequence = getNormSequence(alph, seq, replace, replaceChar);
    bool sequencesAreEqual = 0 == qstricmp(seq.data(), normSequence.data());
    if (!sequencesAreEqual && additionalWarning) {
        QString message(tr("Symbols that don't match the alphabet have been "));
        if (replace) {
            message.append(tr("replaced"));
        } else {
            message.append(tr("removed"));
        }
        QMessageBox::critical(this, parentWidget()->windowTitle(), message);
    }

    CHECK(!normSequence.isEmpty(), tr("Input sequence is empty"));

    resultSequences << DNASequence(name, normSequence, alph);
    return "";
}

bool SeqPasterWidgetController::isFastaFormat(const QString &data) {
    DocumentFormatRegistry *docFormatRegistry = AppContext::getDocumentFormatRegistry();
    SAFE_POINT(NULL != docFormatRegistry, L10N::nullPointerError("document format registry"), false);
    DocumentFormat *fastaFormat = docFormatRegistry->getFormatById(BaseDocumentFormats::FASTA);
    SAFE_POINT(NULL != fastaFormat, L10N::nullPointerError("FASTA format"), false);
    return FormatDetection_Matched == fastaFormat->checkRawData(data.toLatin1()).score;
}

void SeqPasterWidgetController::disableCustomSettings(){
    ui->groupBox->setChecked(false);
    ui->groupBox->setDisabled(true);
    ui->groupBox->hide();
    //ui->groupBox->hide();
}

void SeqPasterWidgetController::setPreferredAlphabet( const DNAAlphabet *alp ){
    preferred = alp;
}

SeqPasterWidgetController::~SeqPasterWidgetController()
{
    delete ui;
}

void SeqPasterWidgetController::selectText(){
    ui->sequenceEdit->selectAll();
    ui->sequenceEdit->setFocus(Qt::OtherFocusReason);
}

void SeqPasterWidgetController::setEventFilter( QObject* evFilter ){
    if (evFilter == NULL){
        return;
    }
    ui->sequenceEdit->installEventFilter(evFilter);
}

void SeqPasterWidgetController::allowFastaFormat(bool allow) {
    allowFastaFormatMode = allow;
}


}//ns
