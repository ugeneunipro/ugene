#ifndef _U2_ALIGNMENT_ENTROPY_WIDGET_FACTORY_H_
#define _U2_ALIGNMENT_ENTROPY_WIDGET_FACTORY_H_

#include <U2Gui/OPWidgetFactory.h>

namespace U2 {

class GObjectView;

class U2VIEW_EXPORT EntropyCalculationWidgetFactory : public OPWidgetFactory {
    Q_OBJECT
public:
    EntropyCalculationWidgetFactory();

    QWidget* createWidget(GObjectView* objView, const QVariantMap& options) override;

    OPGroupParameters getOPGroupParameters() override;

    static const QString& getGroupId();

private:
    static const QString GROUP_ID;
    static const QString GROUP_ICON_STR;
    static const QString GROUP_DOC_PAGE;
};

}  // namespace U2

#endif
