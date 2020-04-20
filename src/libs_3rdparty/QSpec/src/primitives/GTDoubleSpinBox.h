#ifndef _HI_GT_DOUBLE_SPINBOX_H
#define _HI_GT_DOUBLE_SPINBOX_H

#include "primitives/GTSpinBox.h"
#include "GTGlobals.h"

#include <QDoubleSpinBox>

namespace HI {
/*!
 * \brief The class for working with QDoubleSpinBox primitive
 */
class HI_EXPORT GTDoubleSpinbox
{
public:
    static int getValue(GUITestOpStatus &os, QDoubleSpinBox *spinBox);
    static int getValue(GUITestOpStatus &os, const QString &spinBoxName, QWidget *parent = NULL);

    static void setValue(GUITestOpStatus& os, QDoubleSpinBox *spinBox, double v, GTGlobals::UseMethod useMethod = GTGlobals::UseMouse);
    static void setValue(GUITestOpStatus &os, const QString &spinBoxName, int v, GTGlobals::UseMethod useMethod = GTGlobals::UseMouse, QWidget *parent = NULL);
    static void setValue(GUITestOpStatus &os, const QString &spinBoxName, int v, QWidget *parent = NULL);

    static void checkLimits(GUITestOpStatus &os, QDoubleSpinBox *spinBox, double min, double max);
    static void checkLimits(GUITestOpStatus &os, const QString &spinBoxName, int min, int max, QWidget *parent = NULL);
};
}
#endif // GTDOUBLESPINBOX_H
