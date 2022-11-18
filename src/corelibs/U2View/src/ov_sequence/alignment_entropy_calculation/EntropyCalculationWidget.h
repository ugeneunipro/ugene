#ifndef _U2_ENTROPY_CALCULATION_WIDGET_H_
#define _U2_ENTROPY_CALCULATION_WIDGET_H_

#include "ui_EntropyCalculationWidget.h"

namespace U2 {
class AnnotatedDNAView;

class EntropyCalculationWidget : public QWidget, private Ui_EntropyCalculationWidget {
    Q_OBJECT

public:
    EntropyCalculationWidget(AnnotatedDNAView*);

private:
    AnnotatedDNAView* annotatedDnaView;

    void initLayout();
};

}  // namespace U2

#endif // _U2_ENTROPY_CALCULATION_WIDGET_H_
