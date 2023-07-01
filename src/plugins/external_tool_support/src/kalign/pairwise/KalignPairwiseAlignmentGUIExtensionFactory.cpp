#include "KalignPairwiseAlignmentGUIExtensionFactory.h"

#include <QLabel>
#include <QString>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2SafePoints.h>

#include "../KalignSupportTask.h"
#include "KalignPairwiseAlignmentTask.h"

namespace U2 {

Kalign3PairwiseAlignmentAlgorithm::Kalign3PairwiseAlignmentAlgorithm()
    : AlignmentAlgorithm(PairwiseAlignment,
                         "KAlign3",
                         KalignPairwiseAlignmentGUIExtensionFactory::tr("Kalign3"),
                         new KalignPairwiseAlignmentTaskFactory(),
                         new KalignPairwiseAlignmentGUIExtensionFactory(),
                         "Kalign3") {
}

bool Kalign3PairwiseAlignmentAlgorithm::checkAlphabet(const DNAAlphabet* al) const {
    return Kalign3SupportTask::isAlphabetSupported(al->getId());
}

KalignPairwiseAlignmentOptionsWidget::KalignPairwiseAlignmentOptionsWidget(QWidget* parent, QVariantMap* s)
    : AlignmentAlgorithmMainWidget(parent, s) {
    setupUi(this);
    initParameters();
}

KalignPairwiseAlignmentOptionsWidget::~KalignPairwiseAlignmentOptionsWidget() {
    getAlignmentAlgorithmCustomSettings(true);
}

void KalignPairwiseAlignmentOptionsWidget::initParameters() {
    double defaultGapOpen;
    double defaultGapExtd;
    double defaultGapTerm;

    gapOpen->setMinimum(H_MIN_GAP_OPEN);
    gapOpen->setMaximum(H_MAX_GAP_OPEN);

    gapExtd->setMinimum(H_MIN_GAP_EXTD);
    gapExtd->setMaximum(H_MAX_GAP_EXTD);

    gapTerm->setMinimum(H_MIN_GAP_TERM);
    gapTerm->setMaximum(H_MAX_GAP_TERM);

    DNAAlphabetRegistry* alphabetReg = AppContext::getDNAAlphabetRegistry();
    SAFE_POINT(NULL != alphabetReg, "DNAAlphabetRegistry is NULL.", );
    QString alphabetId = externSettings->value(PairwiseAlignmentTaskSettings::ALPHABET, "").toString();
    const DNAAlphabet* alphabet = alphabetReg->findById(alphabetId);
    SAFE_POINT(NULL != alphabet, QString("Alphabet %1 not found").arg(alphabetId), );

    if (alphabet->isNucleic()) {
        defaultGapOpen = H_DEFAULT_GAP_OPEN_DNA;
        defaultGapExtd = H_DEFAULT_GAP_EXTD_DNA;
        defaultGapTerm = H_DEFAULT_GAP_TERM_DNA;
    } else {
        defaultGapOpen = H_DEFAULT_GAP_OPEN;
        defaultGapExtd = H_DEFAULT_GAP_EXTD;
        defaultGapTerm = H_DEFAULT_GAP_TERM;
    }

    if (externSettings->contains(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_OPEN) &&
        externSettings->value(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_OPEN, 0).toInt() >= H_MIN_GAP_OPEN &&
        externSettings->value(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_OPEN, 0).toInt() <= H_MAX_GAP_OPEN) {
        gapOpen->setValue(externSettings->value(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_OPEN, 0).toInt());
    } else {
        gapOpen->setValue(defaultGapOpen);
    }

    if (externSettings->contains(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_EXTD) &&
        externSettings->value(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_EXTD, 0).toInt() >= H_MIN_GAP_EXTD &&
        externSettings->value(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_EXTD, 0).toInt() <= H_MAX_GAP_EXTD) {
        gapExtd->setValue(externSettings->value(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_EXTD, 0).toInt());
    } else {
        gapExtd->setValue(defaultGapExtd);
    }

    if (externSettings->contains(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_TERM) &&
        externSettings->value(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_TERM, 0).toInt() >= H_MIN_GAP_TERM &&
        externSettings->value(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_TERM, 0).toInt() <= H_MAX_GAP_TERM) {
        gapTerm->setValue(externSettings->value(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_EXTD, 0).toInt());
    } else {
        gapTerm->setValue(defaultGapTerm);
    }

    fillInnerSettings();
}

QMap<QString, QVariant> KalignPairwiseAlignmentOptionsWidget::getAlignmentAlgorithmCustomSettings(bool append) {
    fillInnerSettings();
    return AlignmentAlgorithmMainWidget::getAlignmentAlgorithmCustomSettings(append);
}

void KalignPairwiseAlignmentOptionsWidget::fillInnerSettings() {
    innerSettings.insert(PairwiseAlignmentTaskSettings::REALIZATION_NAME, "KAlign3");
    innerSettings.insert(KalignPairwiseAlignmentTaskSettings::PA_H_REALIZATION_NAME, "KAlign3");
    innerSettings.insert(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_OPEN, gapOpen->value());
    innerSettings.insert(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_EXTD, gapExtd->value());
    innerSettings.insert(KalignPairwiseAlignmentTaskSettings::PA_H_GAP_TERM, gapTerm->value());
}

KalignPairwiseAlignmentGUIExtensionFactory::KalignPairwiseAlignmentGUIExtensionFactory()
    : AlignmentAlgorithmGUIExtensionFactory() {
}

AlignmentAlgorithmMainWidget* KalignPairwiseAlignmentGUIExtensionFactory::createMainWidget(QWidget* parent, QVariantMap* s) {
    if (mainWidgets.contains(parent)) {
        return mainWidgets.value(parent, nullptr);
    }
    auto optionsWidget = new KalignPairwiseAlignmentOptionsWidget(parent, s);
    connect(optionsWidget, SIGNAL(destroyed(QObject*)), SLOT(sl_widgetDestroyed(QObject*)));
    mainWidgets.insert(parent, optionsWidget);
    return optionsWidget;
}

}  // namespace U2
