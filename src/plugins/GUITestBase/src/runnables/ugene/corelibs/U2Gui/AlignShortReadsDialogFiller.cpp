/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "AlignShortReadsDialogFiller.h"
#include <base_dialogs/MessageBoxFiller.h>
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QGroupBox>
#include <QPushButton>

#include <U2Core/U2SafePoints.h>

namespace U2 {

AlignShortReadsFiller::BwaSwParameters::BwaSwParameters(const QString& refDir, const QString& refFileName, const QString& readsDir, const QString& readsFileName)
    : Parameters(refDir, refFileName, readsDir, readsFileName, BwaSw),
      matchScore(1),
      mismatchPenalty(3),
      gapOpenPenalty(5),
      gapExtensionPenalty(2),
      bandWidth(50),
      maskLevel(0.5),
      threadsNumber(1),
      readsChunkSize(1000000),
      thresholdScore(30),
      zBest(1),
      seedsNumber(5),
      preferHardClippingInSam(false) {
}

#define GT_CLASS_NAME "GTUtilsDialog::AlignShortReadsFiller"

#define GT_METHOD_NAME "run"
void AlignShortReadsFiller::commonScenario() {
    SAFE_POINT_EXT(parameters, GT_CHECK(0, "Invalid input parameters: NULL pointer"), );

    QWidget* dialog = GTWidget::getActiveModalWidget();
    setCommonParameters(dialog);
    setAdditionalParameters(dialog);
    GTGlobals::sleep(500);
    GTUtilsDialog::clickButtonBox(dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setCommonParameters"
void AlignShortReadsFiller::setCommonParameters(QWidget* dialog) {
    GTComboBox::selectItemByText(GTWidget::findComboBox("methodNamesBox", dialog), parameters->getAlignmentMethod());

    GTUtilsDialog::waitForDialog(new GTFileDialogUtils(parameters->referenceFile));
    GTWidget::click(GTWidget::findWidget("addRefButton", dialog));

    if (!parameters->useDefaultResultPath) {
        GTUtilsDialog::waitForDialog(new GTFileDialogUtils(parameters->resultDir, parameters->resultFileName, GTFileDialogUtils::Save));
        GTWidget::click(GTWidget::findWidget("setResultFileNameButton", dialog));
    }

    foreach (const QString& readsFile, parameters->readsFiles) {
        GTUtilsDialog::waitForDialog(new GTFileDialogUtils(readsFile));
        GTWidget::click(GTWidget::findWidget("addShortreadsButton", dialog));
    }

    auto libraryComboBox = GTWidget::findComboBox("libraryComboBox", dialog);
    if (libraryComboBox->isEnabled()) {
        GTComboBox::selectItemByText(libraryComboBox, parameters->getLibrary());
    }

    auto samBox = GTWidget::findCheckBox("samBox", dialog);
    if (samBox->isEnabled()) {
        GTCheckBox::setChecked(samBox, parameters->samOutput);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setAdditionalParameters"
void AlignShortReadsFiller::setAdditionalParameters(QWidget* dialog) {
    auto bowtie2Parameters = dynamic_cast<Bowtie2Parameters*>(parameters);
    if (nullptr != bowtie2Parameters) {
        setBowtie2AdditionalParameters(bowtie2Parameters, dialog);
        return;
    }

    auto ugaParameters = dynamic_cast<UgeneGenomeAlignerParams*>(parameters);
    if (nullptr != ugaParameters) {
        setUgaAdditionalParameters(ugaParameters, dialog);
        return;
    }

    auto bwaSwParameters = dynamic_cast<BwaSwParameters*>(parameters);
    if (nullptr != bwaSwParameters) {
        setBwaSwAdditionalParameters(bwaSwParameters, dialog);
        return;
    }

    auto bwaParameters = dynamic_cast<BwaParameters*>(parameters);
    if (nullptr != bwaParameters) {
        setBwaAdditionalParameters(bwaParameters, dialog);
        return;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setBowtie2AdditionalParameters"
void AlignShortReadsFiller::setBowtie2AdditionalParameters(Bowtie2Parameters* bowtie2Parameters, QWidget* dialog) {
    // Parameters
    auto modeComboBox = GTWidget::findComboBox("modeComboBox", dialog);

    GTComboBox::selectItemByText(modeComboBox, bowtie2Parameters->getMode());

    auto mismatchesSpinBox = GTWidget::findSpinBox("mismatchesSpinBox", dialog);

    GTSpinBox::setValue(mismatchesSpinBox, bowtie2Parameters->numberOfMismatches);

    auto seedlenCheckBox = GTWidget::findCheckBox("seedlenCheckBox", dialog);

    GTCheckBox::setChecked(seedlenCheckBox, bowtie2Parameters->seedLengthCheckBox);

    auto seedlenSpinBox = GTWidget::findSpinBox("seedlenSpinBox", dialog);

    if (seedlenCheckBox->isChecked()) {
        GTSpinBox::setValue(seedlenSpinBox, bowtie2Parameters->seedLength);
    }

    auto dpadCheckBox = GTWidget::findCheckBox("dpadCheckBox", dialog);

    GTCheckBox::setChecked(dpadCheckBox, bowtie2Parameters->addColumnsToAllowGapsCheckBox);

    auto dpadSpinBox = GTWidget::findSpinBox("dpadSpinBox", dialog);

    if (dpadCheckBox->isChecked()) {
        GTSpinBox::setValue(dpadSpinBox, bowtie2Parameters->addColumnsToAllowGaps);
    }

    auto gbarCheckBox = GTWidget::findCheckBox("gbarCheckBox", dialog);

    GTCheckBox::setChecked(gbarCheckBox, bowtie2Parameters->disallowGapsCheckBox);

    auto gbarSpinBox = GTWidget::findSpinBox("gbarSpinBox", dialog);

    if (gbarCheckBox->isChecked()) {
        GTSpinBox::setValue(gbarSpinBox, bowtie2Parameters->disallowGaps);
    }

    auto seedCheckBox = GTWidget::findCheckBox("seedCheckBox", dialog);

    GTCheckBox::setChecked(seedCheckBox, bowtie2Parameters->seedCheckBox);

    auto seedSpinBox = GTWidget::findSpinBox("seedSpinBox", dialog);

    if (seedCheckBox->isChecked()) {
        GTSpinBox::setValue(seedSpinBox, bowtie2Parameters->seed);
    }

    if (!isOsWindows()) {
        auto threadsSpinBox = GTWidget::findSpinBox("threadsSpinBox", dialog);

        if (threadsSpinBox->minimum() <= bowtie2Parameters->threads && threadsSpinBox->maximum() >= bowtie2Parameters->threads) {
            GTSpinBox::setValue(threadsSpinBox, bowtie2Parameters->threads);
        }
    }

    // Flags
    auto nomixedCheckBox = GTWidget::findCheckBox("nomixedCheckBox", dialog);

    GTCheckBox::setChecked(nomixedCheckBox, bowtie2Parameters->noUnpairedAlignments);

    auto nodiscordantCheckBox = GTWidget::findCheckBox("nodiscordantCheckBox", dialog);

    GTCheckBox::setChecked(nodiscordantCheckBox, bowtie2Parameters->noDiscordantAlignments);

    auto nofwCheckBox = GTWidget::findCheckBox("nofwCheckBox", dialog);

    GTCheckBox::setChecked(nofwCheckBox, bowtie2Parameters->noForwardOrientation);

    auto norcCheckBox = GTWidget::findCheckBox("norcCheckBox", dialog);

    GTCheckBox::setChecked(norcCheckBox, bowtie2Parameters->noReverseComplementOrientation);

    auto nooverlapCheckBox = GTWidget::findCheckBox("nooverlapCheckBox", dialog);

    GTCheckBox::setChecked(nooverlapCheckBox, bowtie2Parameters->noOverlappingMates);

    auto nocontainCheckBox = GTWidget::findCheckBox("nocontainCheckBox", dialog);

    GTCheckBox::setChecked(nocontainCheckBox, bowtie2Parameters->noMatesContainingOneAnother);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setUgaAdditionalParameters"
void AlignShortReadsFiller::setUgaAdditionalParameters(UgeneGenomeAlignerParams* ugaParameters, QWidget* dialog) {
    if (ugaParameters->mismatchesAllowed) {
        GTGroupBox::setChecked("groupBox_mismatches", dialog);
    }
    GTCheckBox::setChecked(GTWidget::findCheckBox("firstMatchBox", dialog), ugaParameters->useBestMode);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setBwaAdditionalParameters"
void AlignShortReadsFiller::setBwaAdditionalParameters(AlignShortReadsFiller::BwaParameters* bwaParameters, QWidget* dialog) {
    GTComboBox::selectItemByText(GTWidget::findComboBox("indexAlgorithmComboBox", dialog), bwaParameters->getIndexAlgorithmString());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setAdditionalParameters"
void AlignShortReadsFiller::setBwaSwAdditionalParameters(AlignShortReadsFiller::BwaSwParameters* bwaSwParameters, QWidget* dialog) {
    GTSpinBox::setValue(GTWidget::findSpinBox("matchScoreSpinbox", dialog), bwaSwParameters->matchScore);
    GTSpinBox::setValue(GTWidget::findSpinBox("mismatchScoreSpinbox", dialog), bwaSwParameters->mismatchPenalty);
    GTSpinBox::setValue(GTWidget::findSpinBox("gapOpenSpinbox", dialog), bwaSwParameters->gapOpenPenalty);
    GTSpinBox::setValue(GTWidget::findSpinBox("gapExtSpinbox", dialog), bwaSwParameters->gapExtensionPenalty);
    GTSpinBox::setValue(GTWidget::findSpinBox("bandWidthSpinbox", dialog), bwaSwParameters->bandWidth);
    GTDoubleSpinbox::setValue(GTWidget::findDoubleSpinBox("maskLevelSpinbox", dialog), bwaSwParameters->maskLevel);
    GTSpinBox::setValue(GTWidget::findSpinBox("numThreadsSpinbox", dialog), bwaSwParameters->threadsNumber);
    GTSpinBox::setValue(GTWidget::findSpinBox("chunkSizeSpinbox", dialog), bwaSwParameters->readsChunkSize);
    GTSpinBox::setValue(GTWidget::findSpinBox("scoreThresholdSpinbox", dialog), bwaSwParameters->thresholdScore);
    GTSpinBox::setValue(GTWidget::findSpinBox("zBestSpinbox", dialog), bwaSwParameters->zBest);
    GTSpinBox::setValue(GTWidget::findSpinBox("revAlnThreshold", dialog), bwaSwParameters->seedsNumber);
    GTCheckBox::setChecked(GTWidget::findCheckBox("hardClippingCheckBox", dialog), bwaSwParameters->preferHardClippingInSam);
}
#undef GT_METHOD_NAME

AlignShortReadsFiller::UgeneGenomeAlignerParams::UgeneGenomeAlignerParams(const QString& refDir,
                                                                          const QString& refFileName,
                                                                          const QString& readsDir,
                                                                          const QString& readsFileName,
                                                                          bool allowMismatches)
    : Parameters(refDir, refFileName, readsDir, readsFileName, UgeneGenomeAligner),
      mismatchesAllowed(allowMismatches),
      useBestMode(true) {
}

AlignShortReadsFiller::UgeneGenomeAlignerParams::UgeneGenomeAlignerParams(const QString& referenceFile,
                                                                          const QStringList& readsFiles)
    : Parameters(referenceFile, readsFiles, UgeneGenomeAligner),
      mismatchesAllowed(false),
      useBestMode(true) {
}

AlignShortReadsFiller::Parameters::Parameters(const QString& refDir,
                                              const QString& refFileName,
                                              const QString& readsDir,
                                              const QString& readsFileName,
                                              AlignShortReadsFiller::Parameters::AlignmentMethod alignmentMethod)
    : alignmentMethod(alignmentMethod),
      referenceFile(refDir + "/" + refFileName),
      readsFiles(readsDir + "/" + readsFileName),
      library(SingleEnd),
      prebuiltIndex(false),
      samOutput(true),
      useDefaultResultPath(true) {
}

AlignShortReadsFiller::Parameters::Parameters(const QString& reference, const QStringList& reads, AlignmentMethod alignmentMethod)
    : alignmentMethod(alignmentMethod),
      referenceFile(reference),
      readsFiles(reads),
      library(SingleEnd),
      prebuiltIndex(false),
      samOutput(true),
      useDefaultResultPath(true) {
}

const QMap<AlignShortReadsFiller::Parameters::AlignmentMethod, QString> AlignShortReadsFiller::Parameters::alignmentMethodMap = AlignShortReadsFiller::Parameters::initAlignmentMethodMap();
const QMap<AlignShortReadsFiller::Parameters::Library, QString> AlignShortReadsFiller::Parameters::libraryMap = AlignShortReadsFiller::Parameters::initLibraryMap();

QMap<AlignShortReadsFiller::Parameters::AlignmentMethod, QString> AlignShortReadsFiller::Parameters::initAlignmentMethodMap() {
    QMap<AlignmentMethod, QString> result;
    result.insert(Bwa, "BWA");
    result.insert(BwaSw, "BWA-SW");
    result.insert(BwaMem, "BWA-MEM");
    result.insert(Bowtie, "Bowtie");
    result.insert(Bowtie2, "Bowtie2");
    result.insert(UgeneGenomeAligner, "UGENE Genome Aligner");
    return result;
}

QMap<AlignShortReadsFiller::Parameters::Library, QString> AlignShortReadsFiller::Parameters::initLibraryMap() {
    QMap<Library, QString> result;
    result.insert(SingleEnd, "Single-end");
    result.insert(PairedEnd, "Paired-end");
    return result;
}

AlignShortReadsFiller::BwaParameters::BwaParameters(const QString& referenceFile,
                                                    const QStringList& readsFiles)
    : Parameters(referenceFile, readsFiles, Bwa),
      indexAlgorithm(Autodetect) {
}

AlignShortReadsFiller::BwaParameters::BwaParameters(const QString& referenceFile, const QString& readsFile)
    : Parameters(referenceFile, {readsFile}, Bwa),
      indexAlgorithm(Autodetect) {
}

QString AlignShortReadsFiller::BwaParameters::getIndexAlgorithmString() const {
    return indexAlgorithmMap[indexAlgorithm];
}

const QMap<AlignShortReadsFiller::BwaParameters::IndexAlgorithm, QString> AlignShortReadsFiller::BwaParameters::indexAlgorithmMap =
    AlignShortReadsFiller::BwaParameters::initIndexAlgorithmMap();

QMap<AlignShortReadsFiller::BwaParameters::IndexAlgorithm, QString> AlignShortReadsFiller::BwaParameters::initIndexAlgorithmMap() {
    QMap<IndexAlgorithm, QString> result;

    result.insert(Autodetect, "autodetect");
    result.insert(Bwtsw, "bwtsw");
    result.insert(Div, "div");
    result.insert(Is, "is");

    return result;
}

#undef GT_CLASS_NAME

}  // namespace U2
