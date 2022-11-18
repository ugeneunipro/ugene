#include "EntropyCalculationWidget.h"

#include <U2Gui/ShowHideSubgroupWidget.h>

#include <U2View/AnnotatedDNAView.h>

namespace U2 {

EntropyCalculationWidget::EntropyCalculationWidget(AnnotatedDNAView* annotatedDnaView)
    : annotatedDnaView(annotatedDnaView) {
    setupUi(this);
    initLayout();
}

void EntropyCalculationWidget::initLayout() {
    additionalSettingsLayout->addWidget(new ShowHideSubgroupWidget(
        QObject::tr("Additional settings"), QObject::tr("Additional settings"), additionalSettingsWidget, false));
}

}  // namespace U2
