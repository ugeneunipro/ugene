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

#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QGroupBox>
#include <QPushButton>

#include "AlignShortReadsDialogFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTComboBox.h>
#include <primitives/GTDoubleSpinBox.h>
#include <primitives/GTGroupBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>
#include <base_dialogs/MessageBoxFiller.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

AlignShortReadsFiller::BwaSwParameters::BwaSwParameters(const QString &refDir, const QString &refFileName, const QString &readsDir, const QString &readsFileName) :
    Parameters(refDir, refFileName, readsDir, readsFileName, BwaSw),
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
    preferHardClippingInSam(false)
{

}

#define GT_CLASS_NAME "GTUtilsDialog::AlignShortReadsFiller"

#define GT_METHOD_NAME "run"
void AlignShortReadsFiller::commonScenario() {
    SAFE_POINT_EXT(parameters, GT_CHECK(0, "Invalid input parameters: NULL pointer"), );

    QWidget* dialog = QApplication::activeModalWidget();
    GT_CHECK(dialog, "activeModalWidget is NULL");

    setCommonParameters(dialog);
    CHECK_OP(os, );

    setAdditionalParameters(dialog);
    CHECK_OP(os, );

    GTGlobals::sleep(500);

    GTUtilsDialog::clickButtonBox(os, dialog, QDialogButtonBox::Ok);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setCommonParameters"
void AlignShortReadsFiller::setCommonParameters(QWidget* dialog) {
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "methodNamesBox", dialog), parameters->getAlignmentMethod());

    GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, parameters->referenceFile));
    GTWidget::click(os, GTWidget::findWidget(os, "addRefButton", dialog));

    if (!parameters->useDefaultResultPath) {
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils (os, parameters->resultDir, parameters->resultFileName, GTFileDialogUtils::Save));
        GTWidget::click(os, GTWidget::findWidget(os, "setResultFileNameButton", dialog));
    }

    foreach (const QString &readsFile, parameters->readsFiles) {
        GTUtilsDialog::waitForDialog(os, new GTFileDialogUtils(os, readsFile));
        GTWidget::click(os, GTWidget::findWidget(os, "addShortreadsButton", dialog));
    }

    QComboBox* libraryComboBox = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "libraryComboBox", dialog));
    GT_CHECK(libraryComboBox, "libraryComboBox is NULL");
    if (libraryComboBox->isEnabled()) {
        GTComboBox::setIndexWithText(os, libraryComboBox, parameters->getLibrary());
    }

    QCheckBox* samBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "samBox", dialog));
    GT_CHECK(samBox, "samBox is NULL");
    if (samBox->isEnabled()) {
        GTCheckBox::setChecked(os, samBox, parameters->samOutput);
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setAdditionalParameters"
void AlignShortReadsFiller::setAdditionalParameters(QWidget* dialog) {
    Bowtie2Parameters* bowtie2Parameters = dynamic_cast<Bowtie2Parameters*>(parameters);
    if (NULL != bowtie2Parameters) {
        setBowtie2AdditionalParameters(bowtie2Parameters, dialog);
        return;
    }

    UgeneGenomeAlignerParams *ugaParameters = dynamic_cast<UgeneGenomeAlignerParams *>(parameters);
    if (NULL != ugaParameters) {
        setUgaAdditionalParameters(ugaParameters, dialog);
        return;
    }

    BwaSwParameters *bwaSwParameters = dynamic_cast<BwaSwParameters *>(parameters);
    if (NULL != bwaSwParameters) {
        setBwaSwAdditionalParameters(bwaSwParameters, dialog);
        return;
    }

    BwaParameters *bwaParameters = dynamic_cast<BwaParameters *>(parameters);
    if (NULL != bwaParameters) {
        setBwaAdditionalParameters(bwaParameters, dialog);
        return;
    }
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setBowtie2AdditionalParameters"
void AlignShortReadsFiller::setBowtie2AdditionalParameters(Bowtie2Parameters* bowtie2Parameters, QWidget* dialog) {
    // Parameters
    QComboBox* modeComboBox = qobject_cast<QComboBox*>(GTWidget::findWidget(os, "modeComboBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(modeComboBox, "modeComboBox is NULL");
    GTComboBox::setIndexWithText(os, modeComboBox, bowtie2Parameters->getMode());
    CHECK_OP(os, );

    QSpinBox* mismatchesSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "mismatchesSpinBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(mismatchesSpinBox, "mismatchesSpinBox is NULL");
    GTSpinBox::setValue(os, mismatchesSpinBox, bowtie2Parameters->numberOfMismatches);
    CHECK_OP(os, );

    QCheckBox* seedlenCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "seedlenCheckBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(seedlenCheckBox, "seedlenCheckBox is NULL");
    GTCheckBox::setChecked(os, seedlenCheckBox, bowtie2Parameters->seedLengthCheckBox);
    CHECK_OP(os, );

    QSpinBox* seedlenSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "seedlenSpinBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(seedlenSpinBox, "seedlenSpinBox is NULL");
    if(seedlenCheckBox->isChecked()){
        GTSpinBox::setValue(os, seedlenSpinBox, bowtie2Parameters->seedLength);
    }
    CHECK_OP(os, );

    QCheckBox* dpadCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "dpadCheckBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(dpadCheckBox, "dpadCheckBox is NULL");
    GTCheckBox::setChecked(os, dpadCheckBox, bowtie2Parameters->addColumnsToAllowGapsCheckBox);
    CHECK_OP(os, );

    QSpinBox* dpadSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "dpadSpinBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(dpadSpinBox, "dpadSpinBox is NULL");
    if(dpadCheckBox->isChecked()){
        GTSpinBox::setValue(os, dpadSpinBox, bowtie2Parameters->addColumnsToAllowGaps);
    }
    CHECK_OP(os, );

    QCheckBox* gbarCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "gbarCheckBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(gbarCheckBox, "gbarCheckBox is NULL");
        GTCheckBox::setChecked(os, gbarCheckBox, bowtie2Parameters->disallowGapsCheckBox);
    CHECK_OP(os, );

    QSpinBox* gbarSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "gbarSpinBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(gbarSpinBox, "gbarSpinBox is NULL");
    if(gbarCheckBox->isChecked()){
        GTSpinBox::setValue(os, gbarSpinBox, bowtie2Parameters->disallowGaps);
    }
    CHECK_OP(os, );

    QCheckBox* seedCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "seedCheckBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(seedCheckBox, "seedCheckBox is NULL");
    GTCheckBox::setChecked(os, seedCheckBox, bowtie2Parameters->seedCheckBox);
    CHECK_OP(os, );

    QSpinBox* seedSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "seedSpinBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(seedSpinBox, "seedSpinBox is NULL");
    if(seedCheckBox->isChecked()){
        GTSpinBox::setValue(os, seedSpinBox, bowtie2Parameters->seed);
    }
    CHECK_OP(os, );

#ifndef Q_OS_WIN
    QSpinBox* threadsSpinBox = qobject_cast<QSpinBox*>(GTWidget::findWidget(os, "threadsSpinBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(threadsSpinBox, "threadsSpinBox is NULL");
    if ( threadsSpinBox->minimum( ) <= bowtie2Parameters->threads
         && threadsSpinBox->maximum( ) >= bowtie2Parameters->threads )
    {
        GTSpinBox::setValue(os, threadsSpinBox, bowtie2Parameters->threads);
        CHECK_OP(os, );
    }
#endif

    // Flags
    QCheckBox* nomixedCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "nomixedCheckBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(nomixedCheckBox, "nomixedCheckBox is NULL");
    GTCheckBox::setChecked(os, nomixedCheckBox, bowtie2Parameters->noUnpairedAlignments);
    CHECK_OP(os, );

    QCheckBox* nodiscordantCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "nodiscordantCheckBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(nodiscordantCheckBox, "nodiscordantCheckBox is NULL");
    GTCheckBox::setChecked(os, nodiscordantCheckBox, bowtie2Parameters->noDiscordantAlignments);
    CHECK_OP(os, );

    QCheckBox* nofwCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "nofwCheckBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(nofwCheckBox, "nofwCheckBox is NULL");
    GTCheckBox::setChecked(os, nofwCheckBox, bowtie2Parameters->noForwardOrientation);
    CHECK_OP(os, );

    QCheckBox* norcCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "norcCheckBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(norcCheckBox, "norcCheckBox is NULL");
    GTCheckBox::setChecked(os, norcCheckBox, bowtie2Parameters->noReverseComplementOrientation);
    CHECK_OP(os, );

    QCheckBox* nooverlapCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "nooverlapCheckBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(nooverlapCheckBox, "nooverlapCheckBox is NULL");
    GTCheckBox::setChecked(os, nooverlapCheckBox, bowtie2Parameters->noOverlappingMates);
    CHECK_OP(os, );

    QCheckBox* nocontainCheckBox = qobject_cast<QCheckBox*>(GTWidget::findWidget(os, "nocontainCheckBox", dialog));
    CHECK_OP(os, );
    GT_CHECK(nocontainCheckBox, "nocontainCheckBox is NULL");
    GTCheckBox::setChecked(os, nocontainCheckBox, bowtie2Parameters->noMatesContainingOneAnother);
    CHECK_OP(os, );
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setUgaAdditionalParameters"
void AlignShortReadsFiller::setUgaAdditionalParameters(UgeneGenomeAlignerParams *ugaParameters, QWidget* dialog) {
    if(ugaParameters->mismatchesAllowed){
        GTGroupBox::setChecked(os, "groupBox_mismatches", dialog);
    }
    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "firstMatchBox", dialog), ugaParameters->useBestMode);
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setBwaAdditionalParameters"
void AlignShortReadsFiller::setBwaAdditionalParameters(AlignShortReadsFiller::BwaParameters *bwaParameters, QWidget *dialog) {
    GTComboBox::setIndexWithText(os, GTWidget::findExactWidget<QComboBox *>(os, "indexAlgorithmComboBox", dialog), bwaParameters->getIndexAlgorithmString());
}
#undef GT_METHOD_NAME

#define GT_METHOD_NAME "setAdditionalParameters"
void AlignShortReadsFiller::setBwaSwAdditionalParameters(AlignShortReadsFiller::BwaSwParameters *bwaSwParameters, QWidget *dialog) {
    GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "matchScoreSpinbox", dialog), bwaSwParameters->matchScore);
    GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "mismatchScoreSpinbox", dialog), bwaSwParameters->mismatchPenalty);
    GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "gapOpenSpinbox", dialog), bwaSwParameters->gapOpenPenalty);
    GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "gapExtSpinbox", dialog), bwaSwParameters->gapExtensionPenalty);
    GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "bandWidthSpinbox", dialog), bwaSwParameters->bandWidth);
    GTDoubleSpinbox::setValue(os, GTWidget::findExactWidget<QDoubleSpinBox *>(os, "maskLevelSpinbox", dialog), bwaSwParameters->maskLevel);
    GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "numThreadsSpinbox", dialog), bwaSwParameters->threadsNumber);
    GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "chunkSizeSpinbox", dialog), bwaSwParameters->readsChunkSize);
    GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "scoreThresholdSpinbox", dialog), bwaSwParameters->thresholdScore);
    GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "zBestSpinbox", dialog), bwaSwParameters->zBest);
    GTSpinBox::setValue(os, GTWidget::findExactWidget<QSpinBox *>(os, "revAlnThreshold", dialog), bwaSwParameters->seedsNumber);
    GTCheckBox::setChecked(os, GTWidget::findExactWidget<QCheckBox *>(os, "hardClippingCheckBox", dialog), bwaSwParameters->preferHardClippingInSam);
}
#undef GT_METHOD_NAME

AlignShortReadsFiller::UgeneGenomeAlignerParams::UgeneGenomeAlignerParams(const QString &refDir,
                                                                          const QString &refFileName,
                                                                          const QString &readsDir,
                                                                          const QString &readsFileName,
                                                                          bool allowMismatches) :
    Parameters(refDir, refFileName, readsDir, readsFileName, UgeneGenomeAligner),
    mismatchesAllowed(allowMismatches),
    useBestMode(true)
{

}

AlignShortReadsFiller::UgeneGenomeAlignerParams::UgeneGenomeAlignerParams(const QString &referenceFile,
                                                                          const QStringList &readsFiles) :
    Parameters(referenceFile, readsFiles, UgeneGenomeAligner),
    mismatchesAllowed(false),
    useBestMode(true)
{

}

AlignShortReadsFiller::Parameters::Parameters(const QString &refDir,
                                              const QString &refFileName,
                                              const QString &readsDir,
                                              const QString &readsFileName,
                                              AlignShortReadsFiller::Parameters::AlignmentMethod alignmentMethod) :
    alignmentMethod(alignmentMethod),
    referenceFile(refDir + "/" + refFileName),
    readsFiles(readsDir + "/" + readsFileName),
    library(SingleEnd),
    prebuiltIndex(false),
    samOutput(true),
    useDefaultResultPath(true)
{

}

AlignShortReadsFiller::Parameters::Parameters(const QString &reference, const QStringList &reads, AlignmentMethod alignmentMethod) :
    alignmentMethod(alignmentMethod),
    referenceFile(reference),
    readsFiles(reads),
    library(SingleEnd),
    prebuiltIndex(false),
    samOutput(true),
    useDefaultResultPath(true)
{

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

AlignShortReadsFiller::BwaParameters::BwaParameters(const QString &referenceFile,
                                                    const QStringList &readsFiles) :
    Parameters(referenceFile, readsFiles, Bwa),
    indexAlgorithm(Autodetect)
{

}

AlignShortReadsFiller::BwaParameters::BwaParameters(const QString &referenceFile, const QString &readsFile) :
    Parameters(referenceFile, QStringList() << readsFile, Bwa),
    indexAlgorithm(Autodetect)
{

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

}
