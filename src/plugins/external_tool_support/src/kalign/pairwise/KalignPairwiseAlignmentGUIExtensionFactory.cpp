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
                         "Kalign3",
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
    DNAAlphabetRegistry* alphabetRegistry = AppContext::getDNAAlphabetRegistry();
    QString alphabetId = externSettings->value(PairwiseAlignmentTaskSettings::ALPHABET, "").toString();
    const DNAAlphabet* alphabet = alphabetRegistry->findById(alphabetId);
    if (alphabet == nullptr) {
        alphabet = alphabetRegistry->findById(BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    }

    Kalign3Settings defaultSettings = Kalign3Settings::getDefaultSettings(alphabet);
    gapOpen->setMinimum(0);
    gapOpen->setMaximum(65535);
    gapOpen->setValue(externSettings->value(KalignPairwiseAlignmentTaskSettings::GAP_OPEN_PENALTY_KEY, defaultSettings.gapOpenPenalty).toDouble());

    gapExtd->setMinimum(0);
    gapExtd->setMaximum(65535);
    gapExtd->setValue(externSettings->value(KalignPairwiseAlignmentTaskSettings::GAP_EXTENSION_PENALTY_KEY, defaultSettings.gapExtensionPenalty).toDouble());

    gapTerm->setMinimum(0);
    gapTerm->setMaximum(65535);
    gapTerm->setValue(externSettings->value(KalignPairwiseAlignmentTaskSettings::GAP_EXTENSION_PENALTY_KEY, defaultSettings.terminalGapExtensionPenalty).toDouble());

    fillInnerSettings();
}

QMap<QString, QVariant> KalignPairwiseAlignmentOptionsWidget::getAlignmentAlgorithmCustomSettings(bool append) {
    fillInnerSettings();
    return AlignmentAlgorithmMainWidget::getAlignmentAlgorithmCustomSettings(append);
}

void KalignPairwiseAlignmentOptionsWidget::fillInnerSettings() {
    innerSettings.insert(PairwiseAlignmentTaskSettings::REALIZATION_NAME, "Kalign3");
    innerSettings.insert(KalignPairwiseAlignmentTaskSettings::GAP_OPEN_PENALTY_KEY, gapOpen->value());
    innerSettings.insert(KalignPairwiseAlignmentTaskSettings::GAP_EXTENSION_PENALTY_KEY, gapExtd->value());
    innerSettings.insert(KalignPairwiseAlignmentTaskSettings::TERMINAL_GAP_EXTENSION_PENALTY_KEY, gapTerm->value());
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
