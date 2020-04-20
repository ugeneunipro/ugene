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

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/MainWindow.h>
#include <U2Gui/U2FileDialog.h>

#include "IlluminaClipStep.h"
#include "trimmomatic/util/LineEditHighlighter.h"

namespace U2 {
namespace LocalWorkflow {

const QString IlluminaClipStepFactory::ID = "ILLUMINACLIP";

IlluminaClipStep::IlluminaClipStep()
    : TrimmomaticStep(IlluminaClipStepFactory::ID)
{
    name = "ILLUMINACLIP";
    description = tr("<html><head></head><body>"
                     "<h4>ILLUMINACLIP</h4>"
                     "<p>This step is used to find and remove Illumina adapters.</p>"
                     "<p>Trimmomatic first compares short sections of an adapter and a read. If they match enough, "
                     "the entire alignment between the read and adapter is scored. For paired-end reads, the \"palindrome\" "
                     "approach is also used to improve the result. See Trimmomatic manual for details.</p>"
                     "<p>Input the following values:</p>"
                     "<ul>"
                     "<li><b>Adapter sequences</b>: a FASTA file with the adapter sequences. Files for TruSeq2 "
                     "(GAII machines), TruSeq3 (HiSeq and MiSeq machines) and Nextera kits for SE and PE reads are "
                     "now available by default. The naming of the various sequences within the specified file "
                     "determines how they are used.</li>"
                     "<li><b>Seed mismatches</b>: the maximum mismatch count in short sections which will still allow "
                     "a full match to be performed.</li>"
                     "<li><b>Simple clip threshold</b>: a threshold for simple alignment mode. Values between 7 and 15 "
                     "are recommended. A perfect match of a 12 base sequence will score just over 7, while 25 bases "
                     "are needed to score 15.</li>"
                     "<li><b>Palindrome clip threshold</b>: a threshold for palindrome alignment mode. For palindromic "
                     "matches, a longer alignment is possible. Therefore the threshold can be in the range of 30. "
                     "Even though this threshold is very high (requiring a match of almost 50 bases) Trimmomatic is "
                     "still able to identify very, very short adapter fragments.</li>"
                     "</ul>"
                     "<p>There are also two optional parameters for palindrome mode: <b>Min adapter length</b> and "
                     "<b>Keep both reads</b>."
                     "</body></html>");
}

TrimmomaticStepSettingsWidget *IlluminaClipStep::createWidget() const {
    return new IlluminaClipSettingsWidget();
}

QString IlluminaClipStep::serializeState(const QVariantMap &widgetState) const {
    QString serializedState;
    serializedState += "\'" + widgetState.value(IlluminaClipSettingsWidget::FASTA_WITH_ADAPTERS_ETC, "").toString() + "\'";

    serializedState += ":";

    if (widgetState.contains(IlluminaClipSettingsWidget::SEED_MISMATCHES)) {
        serializedState += QString::number(widgetState.value(IlluminaClipSettingsWidget::SEED_MISMATCHES).toInt());
    }

    serializedState += ":";

    if (widgetState.contains(IlluminaClipSettingsWidget::PALINDROME_CLIP_THRESHOLD)) {
        serializedState += QString::number(widgetState.value(IlluminaClipSettingsWidget::PALINDROME_CLIP_THRESHOLD).toInt());
    }

    serializedState += ":";

    if (widgetState.contains(IlluminaClipSettingsWidget::SIMPLE_CLIP_THRESHOLD)) {
        serializedState += QString::number(widgetState.value(IlluminaClipSettingsWidget::SIMPLE_CLIP_THRESHOLD).toInt());
    }

    if (widgetState.value(IlluminaClipAdditionalSettingsDialog::ADDITIONAL_SETTINGS_ENABLED, false).toBool()) {
        serializedState += ":";

        if (widgetState.contains(IlluminaClipAdditionalSettingsDialog::MIN_ADAPTER_LENGTH)) {
            serializedState += QString::number(widgetState.value(IlluminaClipAdditionalSettingsDialog::MIN_ADAPTER_LENGTH).toInt());
        }

        serializedState += ":";

        if (widgetState.contains(IlluminaClipAdditionalSettingsDialog::KEEP_BOTH_READS)) {
            serializedState += widgetState.value(IlluminaClipAdditionalSettingsDialog::KEEP_BOTH_READS).toBool() ? "true" : "false";
        }
    }

    return serializedState;
}

QVariantMap IlluminaClipStep::parseState(const QString &command) const {
    QVariantMap state;
    QRegExp regExp(id + ":" + "\\\'([^\\\']*)\\'" + ":" + "(\\d*)" + ":" + "(\\d*)" + ":" + "(\\d*)" +
                   "(:" + "(\\d*)" + ":" + "((true|false){0,1})" + ")?", Qt::CaseInsensitive);

    const bool matched = regExp.exactMatch(command);
    CHECK(matched, state);

    const QString fastaWithAdaptersEtc = regExp.cap(1);
    if (!fastaWithAdaptersEtc.isEmpty()) {
        state[IlluminaClipSettingsWidget::FASTA_WITH_ADAPTERS_ETC] = fastaWithAdaptersEtc;
    }

    const QString seedMismatches = regExp.cap(2);
    if (!seedMismatches.isEmpty()) {
        state[IlluminaClipSettingsWidget::SEED_MISMATCHES] = seedMismatches.toInt();
    }

    const QString palindromeClipThreshold = regExp.cap(3);
    if (!palindromeClipThreshold.isEmpty()) {
        state[IlluminaClipSettingsWidget::PALINDROME_CLIP_THRESHOLD] = palindromeClipThreshold.toInt();
    }

    const QString simpleClipThreshold = regExp.cap(4);
    if (!simpleClipThreshold.isEmpty()) {
        state[IlluminaClipSettingsWidget::SIMPLE_CLIP_THRESHOLD] = simpleClipThreshold.toInt();
    }

    if (!regExp.cap(5).isEmpty()) {
        state[IlluminaClipAdditionalSettingsDialog::ADDITIONAL_SETTINGS_ENABLED] = true;

        const QString minAdapterLength = regExp.cap(6);
        if (!minAdapterLength.isEmpty()) {
            state[IlluminaClipAdditionalSettingsDialog::MIN_ADAPTER_LENGTH] = minAdapterLength.toInt();
        }

        const QString keepBothReads = regExp.cap(7);
        if (!keepBothReads.isEmpty()) {
            state[IlluminaClipAdditionalSettingsDialog::KEEP_BOTH_READS] = (keepBothReads.compare("true", Qt::CaseInsensitive) == 0);
        }
    }

    return state;
}

const QString IlluminaClipSettingsWidget::FASTA_WITH_ADAPTERS_ETC = "fastaWithAdaptersEtc";
const QString IlluminaClipSettingsWidget::SEED_MISMATCHES = "seedMismatches";
const QString IlluminaClipSettingsWidget::PALINDROME_CLIP_THRESHOLD = "palindromeClipThreshold";
const QString IlluminaClipSettingsWidget::SIMPLE_CLIP_THRESHOLD = "simpleClipThreshold";

const QString IlluminaClipSettingsWidget::DEFAULT_SE_ADAPTERS = "TruSeq3-SE.fa";
const QString IlluminaClipSettingsWidget::DEFAULT_PE_ADAPTERS = "TruSeq3-PE-2.fa";

IlluminaClipSettingsWidget::IlluminaClipSettingsWidget() {
    setupUi(this);

    fileName->setText(QDir::toNativeSeparators(QDir("data:").path() + "/adapters/illumina/" + DEFAULT_SE_ADAPTERS));    // The default adapters should be set depending on another attribute value
    new LineEditHighlighter(fileName);

    connect(fileName, SIGNAL(textChanged(QString)), SIGNAL(si_valueChanged()));
    connect(mismatches, SIGNAL(valueChanged(int)), SIGNAL(si_valueChanged()));
    connect(palindromeThreshold, SIGNAL(valueChanged(int)), SIGNAL(si_valueChanged()));
    connect(simpleThreshold, SIGNAL(valueChanged(int)), SIGNAL(si_valueChanged()));
    connect(tbBrowse, SIGNAL(clicked(bool)), SLOT(sl_browseButtonClicked()));
    connect(pushButton, SIGNAL(clicked(bool)), SLOT(sl_optionalButtonClicked()));
}

IlluminaClipSettingsWidget::~IlluminaClipSettingsWidget() {
    emit si_widgetIsAboutToBeDestroyed(getState());
}

bool IlluminaClipSettingsWidget::validate() const {
    return !fileName->text().isEmpty();
}

QVariantMap IlluminaClipSettingsWidget::getState() const {
    QVariantMap state;

    const QString fastaWithAdaptersEtc = fileName->text();
    if (!fastaWithAdaptersEtc.isEmpty()) {
        state[FASTA_WITH_ADAPTERS_ETC] = fastaWithAdaptersEtc;
    }

    state[SEED_MISMATCHES] = mismatches->value();
    state[PALINDROME_CLIP_THRESHOLD] = palindromeThreshold->value();
    state[SIMPLE_CLIP_THRESHOLD] = simpleThreshold->value();

    return state.unite(additionalOptions);
}

void IlluminaClipSettingsWidget::setState(const QVariantMap &state) {
    bool contains = state.contains(FASTA_WITH_ADAPTERS_ETC);
    if (contains) {
        fileName->setText(state[FASTA_WITH_ADAPTERS_ETC].toString());
    }

    contains = state.contains(SEED_MISMATCHES);
    bool valid = false;
    const int seedMismatches = state[SEED_MISMATCHES].toInt(&valid);
    if (contains && valid) {
        mismatches->setValue(seedMismatches);
    }

    contains = state.contains(PALINDROME_CLIP_THRESHOLD);
    const int palindromeClipThreshold = state[PALINDROME_CLIP_THRESHOLD].toInt(&valid);
    if (contains && valid) {
        palindromeThreshold->setValue(palindromeClipThreshold);
    }

    contains = state.contains(SIMPLE_CLIP_THRESHOLD);
    const int simpleClipThreshold = state[SIMPLE_CLIP_THRESHOLD].toInt(&valid);
    if (contains && valid) {
        simpleThreshold->setValue(simpleClipThreshold);
    }

    additionalOptions = IlluminaClipAdditionalSettingsDialog::extractState(state);
}

void IlluminaClipSettingsWidget::sl_browseButtonClicked() {
    QString defaultDir = QDir::searchPaths(PATH_PREFIX_DATA).first() + "/adapters/illumina";
    LastUsedDirHelper dirHelper("trimmomatic/adapters", defaultDir);

    const QString filter = DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::FASTA, true, QStringList());
    QString defaultFilter = DialogUtils::prepareDocumentsFileFilter(BaseDocumentFormats::FASTA, false);
    const QString adaptersFilePath = U2FileDialog::getOpenFileName(this, tr("Open FASTA with adapters"), dirHelper.dir, filter, &defaultFilter);
    if (!adaptersFilePath.isEmpty()) {
        dirHelper.url = adaptersFilePath;
        fileName->setText(adaptersFilePath);
    }
}

void IlluminaClipSettingsWidget::sl_optionalButtonClicked() {
    QObjectScopedPointer<IlluminaClipAdditionalSettingsDialog> additionalOptionsDialog(new IlluminaClipAdditionalSettingsDialog(additionalOptions, AppContext::getMainWindow()->getQMainWindow()));
    const int executionResult = additionalOptionsDialog->exec();
    if (static_cast<QDialog::DialogCode>(executionResult) == QDialog::Accepted) {
        CHECK(!additionalOptionsDialog.isNull(), );
        additionalOptions = additionalOptionsDialog->getState();
    }
}

const QString IlluminaClipAdditionalSettingsDialog::ADDITIONAL_SETTINGS_ENABLED = "additionalSettingsEnabled";
const QString IlluminaClipAdditionalSettingsDialog::MIN_ADAPTER_LENGTH = "minAdapterLength";
const QString IlluminaClipAdditionalSettingsDialog::KEEP_BOTH_READS = "keepBothReads";

IlluminaClipAdditionalSettingsDialog::IlluminaClipAdditionalSettingsDialog(const QVariantMap &widgetState, QWidget* parent) : QDialog(parent) {
    setupUi(this);

    new HelpButton(this, buttonBox, "28967214");

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Apply"));

    keepBothCombo->addItem(tr("True"), true);
    keepBothCombo->addItem(tr("False"), false);

    groupBox->setChecked(widgetState.value(ADDITIONAL_SETTINGS_ENABLED, false).toBool());
    minLengthSpin->setValue(widgetState.value(MIN_ADAPTER_LENGTH, 8).toInt());
    keepBothCombo->setCurrentIndex(keepBothCombo->findData(widgetState.value(KEEP_BOTH_READS, false).toBool()));
}

QVariantMap IlluminaClipAdditionalSettingsDialog::extractState(const QVariantMap &fromState) {
    QVariantMap state;
    state[ADDITIONAL_SETTINGS_ENABLED] = fromState.value(ADDITIONAL_SETTINGS_ENABLED, false);
    state[MIN_ADAPTER_LENGTH] = fromState.value(MIN_ADAPTER_LENGTH, 8);
    state[KEEP_BOTH_READS] = fromState.value(KEEP_BOTH_READS, false);
    return state;
}

QVariantMap IlluminaClipAdditionalSettingsDialog::getState() const {
    QVariantMap state;
    state[ADDITIONAL_SETTINGS_ENABLED] = groupBox->isChecked();
    state[MIN_ADAPTER_LENGTH] = minLengthSpin->value();
    state[KEEP_BOTH_READS] = keepBothCombo->currentData();
    return state;
}

IlluminaClipStepFactory::IlluminaClipStepFactory()
    : TrimmomaticStepFactory(ID)
{

}

IlluminaClipStep *IlluminaClipStepFactory::createStep() const {
    return new IlluminaClipStep();
}

}   // namespace LocalWorkflow
}   // namespace U2
