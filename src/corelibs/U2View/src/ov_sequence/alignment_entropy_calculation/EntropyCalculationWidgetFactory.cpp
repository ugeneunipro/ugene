#include "EntropyCalculationWidgetFactory.h"

#include <QPixmap>

#include <U2Core/U2SafePoints.h>

#include <U2View/AnnotatedDNAView.h>

#include "EntropyCalculationWidget.h"

namespace U2 {

const QString EntropyCalculationWidgetFactory::GROUP_ID = "OP_ENTROPY_CALCULATION";
const QString EntropyCalculationWidgetFactory::GROUP_ICON_STR = "";
const QString EntropyCalculationWidgetFactory::GROUP_DOC_PAGE = "";

EntropyCalculationWidgetFactory::EntropyCalculationWidgetFactory() {
    objectViewOfWidget = ObjViewType_SequenceView;
}

QWidget* EntropyCalculationWidgetFactory::createWidget(GObjectView* objView, const QVariantMap& /*options*/) {
    SAFE_POINT(objView != nullptr,
               QString("Internal error: unable to create widget for group '%1', object view is NULL.").arg(GROUP_ID),
               nullptr);

    AnnotatedDNAView* annotatedDnaView = qobject_cast<AnnotatedDNAView*>(objView);
    SAFE_POINT(annotatedDnaView != nullptr,
               QString("Internal error: unable to cast object view to AnnotatedDNAView for group '%1'.").arg(GROUP_ID),
               nullptr);

    EntropyCalculationWidget* widget = new EntropyCalculationWidget(annotatedDnaView);
    widget->setObjectName("EntropyCalculationWidget");
    return widget;
}

OPGroupParameters EntropyCalculationWidgetFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), QObject::tr("Alignment Entropy Calculation"), GROUP_DOC_PAGE);
}

const QString& EntropyCalculationWidgetFactory::getGroupId() {
    return GROUP_ID;
}

}  // namespace U2
