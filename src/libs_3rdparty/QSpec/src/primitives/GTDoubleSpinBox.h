#pragma once
#include <QDoubleSpinBox>

#include "GTGlobals.h"
#include "primitives/GTSpinBox.h"

namespace HI {
/*!
 * \brief The class for working with QDoubleSpinBox primitive
 */
class HI_EXPORT GTDoubleSpinbox {
public:
    static int getValue(QDoubleSpinBox* spinBox);
    static int getValue(const QString& spinBoxName, QWidget* parent = NULL);

    static void setValue(QDoubleSpinBox* spinBox, double v, GTGlobals::UseMethod useMethod = GTGlobals::UseMouse);
    static void setValue(const QString& spinBoxName, double v, GTGlobals::UseMethod useMethod = GTGlobals::UseMouse, QWidget* parent = NULL);
    static void setValue(const QString& spinBoxName, double v, QWidget* parent = NULL);

    static void checkLimits(QDoubleSpinBox* spinBox, double min, double max);
    static void checkLimits(const QString& spinBoxName, int min, int max, QWidget* parent = NULL);
};
}  // namespace HI
