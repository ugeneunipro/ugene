#include "PairwiseAlignmentHirschbergGUIExtensionFactory.h"

#include <QLabel>
#include <QLayout>
#include <QList>
#include <QString>
#include <QVBoxLayout>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/U2SafePoints.h>

#include "PairwiseAlignmentHirschbergTask.h"

namespace U2 {

PairwiseAlignmentHirschbergMainWidget::PairwiseAlignmentHirschbergMainWidget(QWidget* parent, QVariantMap* s)
    : AlignmentAlgorithmMainWidget(parent, s) {
    setupUi(this);
    initParameters();
}

PairwiseAlignmentHirschbergMainWidget::~PairwiseAlignmentHirschbergMainWidget() {
    getAlignmentAlgorithmCustomSettings(true);
}

void PairwiseAlignmentHirschbergMainWidget::initParameters() {
    double defaultGapOpen;
    double defaultGapExtd;
    double defaultGapTerm;
    double defaultBonusScore;

    gapOpen->setMinimum(H_MIN_GAP_OPEN);
    gapOpen->setMaximum(H_MAX_GAP_OPEN);

    gapExtd->setMinimum(H_MIN_GAP_EXTD);
    gapExtd->setMaximum(H_MAX_GAP_EXTD);

    gapTerm->setMinimum(H_MIN_GAP_TERM);
    gapTerm->setMaximum(H_MAX_GAP_TERM);

    bonusScore->setMinimum(H_MIN_BONUS_SCORE);
    bonusScore->setMaximum(H_MAX_BONUS_SCORE);

    DNAAlphabetRegistry* alphabetReg = AppContext::getDNAAlphabetRegistry();
    SAFE_POINT(NULL != alphabetReg, "DNAAlphabetRegistry is NULL.", );
    QString alphabetId = externSettings->value(PairwiseAlignmentTaskSettings::ALPHABET, "").toString();
    const DNAAlphabet* alphabet = alphabetReg->findById(alphabetId);
    SAFE_POINT(NULL != alphabet, QString("Alphabet %1 not found").arg(alphabetId), );

    if (alphabet->isNucleic()) {
        defaultGapOpen = H_DEFAULT_GAP_OPEN_DNA;
        defaultGapExtd = H_DEFAULT_GAP_EXTD_DNA;
        defaultGapTerm = H_DEFAULT_GAP_TERM_DNA;
        defaultBonusScore = H_DEFAULT_BONUS_SCORE_DNA;
    } else {
        defaultGapOpen = H_DEFAULT_GAP_OPEN;
        defaultGapExtd = H_DEFAULT_GAP_EXTD;
        defaultGapTerm = H_DEFAULT_GAP_TERM;
        defaultBonusScore = H_DEFAULT_BONUS_SCORE;
    }

    if (externSettings->contains(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_OPEN) &&
        externSettings->value(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_OPEN, 0).toInt() >= H_MIN_GAP_OPEN &&
        externSettings->value(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_OPEN, 0).toInt() <= H_MAX_GAP_OPEN) {
        gapOpen->setValue(externSettings->value(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_OPEN, 0).toInt());
    } else {
        gapOpen->setValue(defaultGapOpen);
    }

    if (externSettings->contains(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_EXTD) &&
        externSettings->value(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_EXTD, 0).toInt() >= H_MIN_GAP_EXTD &&
        externSettings->value(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_EXTD, 0).toInt() <= H_MAX_GAP_EXTD) {
        gapExtd->setValue(externSettings->value(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_EXTD, 0).toInt());
    } else {
        gapExtd->setValue(defaultGapExtd);
    }

    if (externSettings->contains(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_TERM) &&
        externSettings->value(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_TERM, 0).toInt() >= H_MIN_GAP_TERM &&
        externSettings->value(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_TERM, 0).toInt() <= H_MAX_GAP_TERM) {
        gapTerm->setValue(externSettings->value(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_EXTD, 0).toInt());
    } else {
        gapTerm->setValue(defaultGapTerm);
    }

    if (externSettings->contains(PairwiseAlignmentHirschbergTaskSettings::PA_H_BONUS_SCORE) &&
        externSettings->value(PairwiseAlignmentHirschbergTaskSettings::PA_H_BONUS_SCORE, 0).toInt() >= H_MIN_BONUS_SCORE &&
        externSettings->value(PairwiseAlignmentHirschbergTaskSettings::PA_H_BONUS_SCORE, 0).toInt() <= H_MAX_BONUS_SCORE) {
        bonusScore->setValue(externSettings->value(PairwiseAlignmentHirschbergTaskSettings::PA_H_BONUS_SCORE, 0).toInt());
    } else {
        bonusScore->setValue(defaultBonusScore);
    }

    fillInnerSettings();
}

QMap<QString, QVariant> PairwiseAlignmentHirschbergMainWidget::getAlignmentAlgorithmCustomSettings(bool append) {
    fillInnerSettings();
    return AlignmentAlgorithmMainWidget::getAlignmentAlgorithmCustomSettings(append);
}

void PairwiseAlignmentHirschbergMainWidget::fillInnerSettings() {
    innerSettings.insert(PairwiseAlignmentTaskSettings::REALIZATION_NAME, "KAlign");
    innerSettings.insert(PairwiseAlignmentHirschbergTaskSettings::PA_H_REALIZATION_NAME, "KAlign");
    innerSettings.insert(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_OPEN, gapOpen->value());
    innerSettings.insert(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_EXTD, gapExtd->value());
    innerSettings.insert(PairwiseAlignmentHirschbergTaskSettings::PA_H_GAP_TERM, gapTerm->value());
    innerSettings.insert(PairwiseAlignmentHirschbergTaskSettings::PA_H_BONUS_SCORE, bonusScore->value());
}

PairwiseAlignmentHirschbergGUIExtensionFactory::PairwiseAlignmentHirschbergGUIExtensionFactory()
    : AlignmentAlgorithmGUIExtensionFactory() {
}

PairwiseAlignmentHirschbergGUIExtensionFactory::~PairwiseAlignmentHirschbergGUIExtensionFactory() {
}

AlignmentAlgorithmMainWidget* PairwiseAlignmentHirschbergGUIExtensionFactory::createMainWidget(QWidget* parent, QVariantMap* s) {
    if (mainWidgets.contains(parent)) {
        return mainWidgets.value(parent, NULL);
    }
    PairwiseAlignmentHirschbergMainWidget* newMainWidget = new PairwiseAlignmentHirschbergMainWidget(parent, s);
    connect(newMainWidget, SIGNAL(destroyed(QObject*)), SLOT(sl_widgetDestroyed(QObject*)));
    mainWidgets.insert(parent, newMainWidget);
    return newMainWidget;
}

}  // namespace U2
