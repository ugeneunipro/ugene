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

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>

#include <U2Core/AppContext.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include "InputWidgetsControllers.h"

namespace U2 {

/*InputWidgetController*/
InputWidgetController::InputWidgetController(QWidget* baseWidget, const QString& settingsPath, const QString& cmdLinePreffix, const QVariant& defaultValue) 
  : cmdLinePrefix(cmdLinePreffix),
    settingsPath(settingsPath),
    curValue(defaultValue),
    defaultValue(defaultValue),
    baseWidget(baseWidget)
{
}

void InputWidgetController::restoreDefault() {
    AppContext::getSettings()->remove(settingsPath);
    curValue = defaultValue;
    setWidgetValue(defaultValue);
}

void InputWidgetController::restoreFromSettings() {
    curValue = AppContext::getSettings()->getValue(settingsPath, defaultValue);
    setWidgetValue(curValue);
}

void InputWidgetController::storeParameter() {
    curValue = getWidgetValue();
    if(!curValue.isNull()) {
        AppContext::getSettings()->setValue(settingsPath, curValue);
    }
}

void InputWidgetController::addParameterToCmdLineSettings(QStringList& settings) {
    CHECK(baseWidget->isEnabled(), );
    CHECK(!cmdLinePrefix.isEmpty(), );
    curValue = getWidgetValue();
    if(!curValue.isNull() && curValue != defaultValue) {
        settings << cmdLinePrefix;
        settings << curValue.toString();
    }
}

void InputWidgetController::setWidgetEnabled(bool isEnabled) {
    if(NULL != baseWidget) {
        baseWidget->setEnabled(isEnabled);
    }
}

/*SpinBoxController*/
SpinBoxController::SpinBoxController(QSpinBox* inputWidget, const QString& settingsPath, const QString& cmdLinePreffix, const QVariant& defaultValue) 
    : InputWidgetController(inputWidget, settingsPath, cmdLinePreffix, defaultValue),
      inputWidget( inputWidget),
      minimumValue(0)
{
    minimumValue = inputWidget->minimum();
}

void SpinBoxController::setWidgetValue(const QVariant& newValue) {
    int newSpinValue = 0;
    if(!newValue.isNull()) {
        newSpinValue = newValue.toInt();
    } 
    else {
        inputWidget->setValue(curValue.toInt());
        return;
    }

    bool withoutSpecialValue = inputWidget->specialValueText().isEmpty();
    if(withoutSpecialValue || newSpinValue > minimumValue) {
        curValue = newValue;
    }
    inputWidget->setValue(newSpinValue);
}

QVariant SpinBoxController::getWidgetValue() {
    if(inputWidget->specialValueText().isEmpty() || (inputWidget->value() >= minimumValue && inputWidget->isEnabled())) {
        return inputWidget->value();
    }
    else {
        return QVariant();
    }
}

void SpinBoxController::setWidgetEnabled(bool isEnabled) {
    if(isEnabled == inputWidget->isEnabled()) {
        return;
    }
    bool withoutSpecialValue = inputWidget->specialValueText().isEmpty();
    if(!isEnabled && inputWidget->value() > inputWidget->minimum()) {
        curValue = inputWidget->value();
    }
    if(!withoutSpecialValue) {
        inputWidget->setValue(isEnabled ? curValue.toInt() : inputWidget->minimum());
    }
    inputWidget->setEnabled(isEnabled);
}

/*DoubleSpinBoxController*/
DoubleSpinBoxController::DoubleSpinBoxController(QDoubleSpinBox* inputWidget, const QString& settingsPath, const QString& cmdLinePreffix, const QVariant& defaultValue) 
    : InputWidgetController(inputWidget, settingsPath, cmdLinePreffix, defaultValue),
      inputWidget(inputWidget),
      minimumValue(0.0)
{
    minimumValue = inputWidget->value();
}

void DoubleSpinBoxController::setWidgetValue(const QVariant& newValue) {
    double newSpinValue = 0.0;
    if(!newValue.isNull()) {
        newSpinValue = newValue.toDouble();
    } 
    else {
        inputWidget->setValue(curValue.toDouble());
        return;
    }

    bool withoutSpecialValue = inputWidget->specialValueText().isEmpty();
    if(withoutSpecialValue || newSpinValue > inputWidget->minimum()) {
        curValue = newValue;
    }
    inputWidget->setValue(newSpinValue);
}

QVariant DoubleSpinBoxController::getWidgetValue() {
    if(inputWidget->specialValueText().isEmpty() || (inputWidget->value() >= 0 && inputWidget->isEnabled())) {
        return inputWidget->value();
    }
    else {
        return QVariant();
    }
}

void DoubleSpinBoxController::setWidgetEnabled(bool isEnabled) {
    if(isEnabled == inputWidget->isEnabled()) {
        return;
    }
    bool withoutSpecialValue = inputWidget->specialValueText().isEmpty();
    if(!isEnabled && inputWidget->value() > inputWidget->minimum()) {
        curValue = inputWidget->value();
    }
    if(!withoutSpecialValue) {
        inputWidget->setValue(isEnabled ? curValue.toDouble() : inputWidget->minimum());
    }
    inputWidget->setEnabled(isEnabled);
}

/*CheckBoxController*/
CheckBoxController::CheckBoxController(QCheckBox* inputWidget, const QString& settingsPath, const QString& cmdLinePreffix, const QVariant& defaultValue) 
    : InputWidgetController(inputWidget, settingsPath, cmdLinePreffix, defaultValue),
      inputWidget(inputWidget)
{
    connect(inputWidget, SIGNAL(stateChanged(int)), SLOT(stateChanged(int)));
}

void CheckBoxController::addDependentParameter(ParameterDependence dependence) {
    dependentParameters.append(dependence);
    bool isEnabled = dependence.checkboxState == inputWidget->isChecked();
    dependence.parameterController->setWidgetEnabled(isEnabled);
}

void CheckBoxController::stateChanged(int newState) {
    foreach(const ParameterDependence& dependence, dependentParameters) {
        bool isEnabled = dependence.checkboxState == (newState == Qt::Checked);
        dependence.parameterController->setWidgetEnabled(isEnabled);
    }
}

void CheckBoxController::setWidgetValue(const QVariant& newValue) {
    if(!newValue.isNull()) {
        inputWidget->setChecked(newValue.toBool());
    }
}

QVariant CheckBoxController::getWidgetValue() {
    return inputWidget->isChecked();
}

/*RadioButtonController*/
RadioButtonController::RadioButtonController(QRadioButton* inputWidget, const QString& settingsPath, const QString& cmdLinePreffix, const QVariant& defaultValue) 
    : InputWidgetController(inputWidget, settingsPath, cmdLinePreffix, defaultValue),
      inputWidget(inputWidget)
{
}

void RadioButtonController::setWidgetValue(const QVariant& newValue) {
    if(!newValue.isNull()) {
        inputWidget->setChecked(newValue.toBool());
    }
}

QVariant RadioButtonController::getWidgetValue() {
    return inputWidget->isChecked();
}

/*ComboBoxController*/
ComboBoxController::ComboBoxController(QComboBox* inputWidget, const QString& settingsPath, const QString& cmdLinePreffix, const QVariant& defaultValue, const QStringList& parameters) 
    : InputWidgetController(inputWidget, settingsPath, cmdLinePreffix, defaultValue),
      inputWidget(inputWidget),
      parameters(parameters)
{
}

void ComboBoxController::setWidgetValue(const QVariant& newValue) {
    if(!newValue.isNull()) {
        inputWidget->setCurrentIndex(newValue.toInt());
    }
}

QVariant ComboBoxController::getWidgetValue() {
    return inputWidget->currentIndex();
}

void ComboBoxController::addParameterToCmdLineSettings(QStringList& settings) {
    if(!inputWidget->isEnabled()) {
        return;
    }
    if(cmdLinePrefix.isEmpty()) {
        return;
    }
    settings << cmdLinePrefix;
    int curIndex = inputWidget->currentIndex();
    if(parameters.size() > curIndex) {
        settings << parameters.at(curIndex);
    }
    else {
        settings << inputWidget->currentText();
    }
}

/*LineEditController*/
LineEditController::LineEditController(QLineEdit* inputWidget, const QString& settingsPath, const QString& cmdLinePreffix, const QVariant& defaultValue) 
    : InputWidgetController(inputWidget, settingsPath, cmdLinePreffix, defaultValue),
      inputWidget(inputWidget)
{
}

void LineEditController::setWidgetValue(const QVariant& newValue) {
    if(!newValue.isNull()) {
        inputWidget->setText(newValue.toString());
    }
}

QVariant LineEditController::getWidgetValue() {
    return inputWidget->text();
}

/*WidgetControllersContainer*/
WidgetControllersContainer::~WidgetControllersContainer() {
    qDeleteAll(widgetControllers);
}

InputWidgetController* WidgetControllersContainer::addWidgetController(QCheckBox* inputWidget, const QString& seetingsPath, const QString& cmdLinePreffix) {
    return addWidget(new CheckBoxController(inputWidget, seetingsPath, cmdLinePreffix, inputWidget->isChecked()));
}

InputWidgetController* WidgetControllersContainer::addWidgetController(QRadioButton* inputWidget, const QString& seetingsPath, const QString& cmdLinePreffix) {
    return addWidget(new RadioButtonController(inputWidget, seetingsPath, cmdLinePreffix, inputWidget->isChecked()));
}

InputWidgetController* WidgetControllersContainer::addWidgetController(QSpinBox* inputWidget, const QString& seetingsPath, const QString& cmdLinePreffix) {
    return addWidget(new SpinBoxController(inputWidget, seetingsPath, cmdLinePreffix, inputWidget->value()));
}

InputWidgetController* WidgetControllersContainer::addWidgetController(QDoubleSpinBox* inputWidget, const QString& seetingsPath, const QString& cmdLinePreffix) {
    return addWidget(new DoubleSpinBoxController(inputWidget, seetingsPath, cmdLinePreffix, inputWidget->value()));
}

InputWidgetController* WidgetControllersContainer::addWidgetController(QComboBox* inputWidget, const QString& seetingsPath, const QString& cmdLinePreffix, const QStringList& parameters) {
    return addWidget(new ComboBoxController(inputWidget, seetingsPath, cmdLinePreffix, inputWidget->currentIndex(), parameters));
}

InputWidgetController* WidgetControllersContainer::addWidgetController(QLineEdit* inputWidget, const QString& seetingsPath, const QString& cmdLinePreffix) {
    return addWidget(new LineEditController(inputWidget, seetingsPath, cmdLinePreffix, inputWidget->text()));
}

InputWidgetController* WidgetControllersContainer::addWidget(InputWidgetController* inputWidget) {
    SAFE_POINT(NULL != inputWidget, "Null pointer argument 'inputWidget' in function addWidgetController()", NULL);
    widgetControllers.append(inputWidget);
    return inputWidget;
}

void WidgetControllersContainer::storeSettings() {
    foreach(InputWidgetController* curController, widgetControllers) {
        curController->storeParameter();
    }
}

void WidgetControllersContainer::restoreDefault() {
    foreach(InputWidgetController* curController, widgetControllers) {
        curController->restoreDefault();
    }
}

void WidgetControllersContainer::getDataFromSettings() {
    foreach(InputWidgetController* curController, widgetControllers) {
        curController->restoreFromSettings();
    }
}

void WidgetControllersContainer::addParametersToCmdLine(QStringList& cmdLineSettings) {
    foreach(InputWidgetController* curController, widgetControllers) {
        curController->addParameterToCmdLineSettings(cmdLineSettings);
    }
}
} //namespace
