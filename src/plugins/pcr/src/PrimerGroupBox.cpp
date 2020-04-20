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

#include <QMainWindow>

#include <U2Algorithm/FindAlgorithmTask.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceUtils.h>
#include <U2Core/DNATranslationImpl.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/MainWindow.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "PrimerGroupBox.h"
#include "PrimerLibrarySelector.h"
#include "PrimerStatistics.h"


namespace U2 {

PrimerGroupBox::PrimerGroupBox(QWidget *parent)
    : QWidget(parent),
      findPrimerTask(NULL),
      annotatedDnaView(NULL)
{
    setupUi(this);

    connect(primerEdit, SIGNAL(textChanged(const QString &)), SLOT(sl_onPrimerChanged(const QString &)));
    connect(reverseComplementButton, SIGNAL(clicked()), SLOT(sl_translate()));
    connect(browseButton, SIGNAL(clicked()), SLOT(sl_browse()));
}

void PrimerGroupBox::setAnnotatedDnaView(AnnotatedDNAView *dnaView) {
    cancelFindPrimerTask();
    annotatedDnaView = dnaView;
    connect(annotatedDnaView, SIGNAL(si_focusChanged(ADVSequenceWidget*,ADVSequenceWidget*)), SLOT(sl_focusChanged()));
}

void PrimerGroupBox::sl_onPrimerChanged(const QString &primer) {
    if (PrimerStatistics::validate(primer) || primer.isEmpty() || annotatedDnaView == NULL) {
        updateStatistics(primer);
    } else {
        findPrimerAlternatives(primer);
    }

    emit si_primerChanged();
}

QByteArray PrimerGroupBox::getPrimer() const {
    return primerEdit->text().toLocal8Bit();
}

uint PrimerGroupBox::getMismatches() const {
    int value = mismatchesSpinBox->value();
    SAFE_POINT(value >= 0, "Negative mismatches count", 0);
    return uint(value);
}

void PrimerGroupBox::sl_translate() {
    const QByteArray translation = DNASequenceUtils::reverseComplement(primerEdit->text().toLocal8Bit());
    primerEdit->setInvalidatedText(translation);
}

void PrimerGroupBox::sl_browse() {
    QObjectScopedPointer<PrimerLibrarySelector> dlg = new PrimerLibrarySelector(AppContext::getMainWindow()->getQMainWindow());
    dlg->exec();
    CHECK(!dlg.isNull(), );
    CHECK(QDialog::Accepted == dlg->result(), );
    Primer result = dlg->getResult();
    primerEdit->setInvalidatedText(result.sequence);
}

void PrimerGroupBox::sl_findPrimerTaskStateChanged() {
    SAFE_POINT(findPrimerTask != NULL, "Caught the taskStateChanged of unknown task", );

    CHECK(findPrimerTask->isFinished() || findPrimerTask->isCanceled() || findPrimerTask->hasError(), );

    if (!findPrimerTask->getStateInfo().isCoR()) {
        QList<FindAlgorithmResult> results = findPrimerTask->popResults();
        if (results.size() == 1) {
            // in case of the sequence context was changed the task is canceled
            ADVSequenceObjectContext *sequenceContext = annotatedDnaView->getSequenceInFocus();
            SAFE_POINT(NULL != sequenceContext, L10N::nullPointerError("Sequence Context"), );
            U2SequenceObject *sequenceObject = sequenceContext->getSequenceObject();
            SAFE_POINT(NULL != sequenceObject, L10N::nullPointerError("Sequence Object"), );

            QByteArray primerOnSeq = sequenceObject->getSequenceData(results.first().region);
            if (results.first().strand == U2Strand::Complementary) {
                SAFE_POINT(findPrimerTask->getSettings().complementTT != NULL, L10N::nullPointerError("Complement Translation"), );
                findPrimerTask->getSettings().complementTT->translate(primerOnSeq.data(), primerOnSeq.length());
                TextUtils::reverse(primerOnSeq.data(), primerOnSeq.length());
            }
            updateStatistics(QString(primerOnSeq));
        }
    }
    findPrimerTask = NULL;
    disconnect(this, SLOT(sl_findPrimerTaskStateChanged()));
}

void PrimerGroupBox::sl_focusChanged() {
    cancelFindPrimerTask();
    if (annotatedDnaView->getSequenceInFocus() != NULL) {
        sl_onPrimerChanged(primerEdit->text());
    } else {
        annotatedDnaView = NULL;
    }
}

QString PrimerGroupBox::getTmString(const QString &sequence) {
    double tm = PrimerStatistics::getMeltingTemperature(sequence.toLocal8Bit());
    QString tmString = tm != Primer::INVALID_TM ? PrimerStatistics::getDoubleStringValue(tm) + QString::fromLatin1("\x00B0") + "C" : tr("N/A");
    return tr("Tm = ") + tmString;
}

void PrimerGroupBox::findPrimerAlternatives(const QString& primer) {
    cancelFindPrimerTask();
    updateStatistics(primer);

    FindAlgorithmTaskSettings settings;

    SAFE_POINT(annotatedDnaView != NULL, L10N::nullPointerError("Annotated DNA view"), );
    ADVSequenceObjectContext *sequenceContext = annotatedDnaView->getSequenceInFocus();
    SAFE_POINT(NULL != sequenceContext, L10N::nullPointerError("Sequence Context"), );
    U2SequenceObject *sequenceObject = sequenceContext->getSequenceObject();
    SAFE_POINT(NULL != sequenceObject, L10N::nullPointerError("Sequence Object"), );

    U2OpStatusImpl os;
    settings.sequence = sequenceObject->getWholeSequenceData(os);

    settings.searchIsCircular = sequenceObject->isCircular();
    settings.searchRegion.length = sequenceObject->getSequenceLength();
    settings.patternSettings = FindAlgorithmPatternSettings_Subst;
    settings.strand = FindAlgorithmStrand_Both;
    settings.useAmbiguousBases = true;
    settings.pattern = primer.toLatin1();

    const DNAAlphabet *alphabet = AppContext::getDNAAlphabetRegistry()->findById(BaseDNAAlphabetIds::NUCL_DNA_EXTENDED());
    SAFE_POINT(NULL != alphabet, L10N::nullPointerError("DNA Alphabet"), );
    DNATranslation *translator = AppContext::getDNATranslationRegistry()->lookupComplementTranslation(alphabet);
    SAFE_POINT(NULL != translator, L10N::nullPointerError("DNA Translator"), );
    settings.complementTT = translator;

    findPrimerTask = new FindAlgorithmTask(settings);
    connect(findPrimerTask, SIGNAL(si_stateChanged()), this, SLOT(sl_findPrimerTaskStateChanged()));

    AppContext::getTaskScheduler()->registerTopLevelTask(findPrimerTask);
}

void PrimerGroupBox::cancelFindPrimerTask() {
    if (findPrimerTask != NULL) {
        disconnect(this, SLOT(sl_findPrimerTaskStateChanged()));
        if(!findPrimerTask->isCanceled() && findPrimerTask->getState() != Task::State_Finished){
            findPrimerTask->cancel();
        }
        findPrimerTask = NULL;
    }
}

void PrimerGroupBox::updateStatistics(const QString &primer) {
    QString characteristics;

    if (!primer.isEmpty()) {
        characteristics += getTmString(primer) + ", ";
        characteristics += QString::number(primer.length()) + tr("-mer");
    }

    characteristicsLabel->setText(characteristics);
}

} // U2
