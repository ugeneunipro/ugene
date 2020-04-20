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

#ifndef _U2_INPUT_WIDGETS_CONTROLLERS_H
#define _U2_INPUT_WIDGETS_CONTROLLERS_H

#include <QStringList>
#include <QVariant>

#include <U2Core/global.h>

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QRadioButton;
class QSpinBox;

namespace U2 {

class InputWidgetController;

class U2GUI_EXPORT ParameterDependence {
public:
    ParameterDependence(InputWidgetController* parameterController, bool checkboxState)
        : parameterController(parameterController), checkboxState(checkboxState)
    {
    }
    InputWidgetController* parameterController;
    bool                   checkboxState;
};

class U2GUI_EXPORT InputWidgetController : public QObject {
    Q_OBJECT
public:
    void restoreFromSettings();
    void storeParameter();
    void restoreDefault();
    virtual void addParameterToCmdLineSettings(QStringList& settings);
    virtual void setWidgetEnabled(bool isEnabled);
    virtual void addDependentParameter(ParameterDependence dependence) { Q_UNUSED(dependence); }
protected:
    virtual void setWidgetValue(const QVariant& newValue) = 0;
    virtual QVariant getWidgetValue() = 0;

    InputWidgetController(QWidget* baseWidget, const QString& settingsPath, const QString& cmdLinePrefix, const QVariant& defaultValue);
    QString  cmdLinePrefix;
    QString  settingsPath;
    QVariant curValue;
    QVariant defaultValue;
    QWidget* baseWidget;
};

class U2GUI_EXPORT SpinBoxController : public InputWidgetController {
    Q_OBJECT
public:
    SpinBoxController(QSpinBox* inputWidget, const QString& settingsPath, const QString& cmdLinePrefix, const QVariant& defaultValue);
    void setWidgetEnabled(bool isEnabled);
protected:
    void setWidgetValue(const QVariant& newValue);
    QVariant getWidgetValue();
private:
    QSpinBox* inputWidget;
    int minimumValue;
};

class U2GUI_EXPORT DoubleSpinBoxController : public InputWidgetController {
    Q_OBJECT
public:
    DoubleSpinBoxController(QDoubleSpinBox* inputWidget, const QString& settingsPath, const QString& cmdLinePrefix, const QVariant& defaultValue);
    void setWidgetEnabled(bool isEnabled);
protected:
    void setWidgetValue(const QVariant& newValue);
    QVariant getWidgetValue();
private:
    QDoubleSpinBox* inputWidget;
    double minimumValue;
};

class U2GUI_EXPORT CheckBoxController : public InputWidgetController {
    Q_OBJECT
public:
    CheckBoxController(QCheckBox* inputWidget, const QString& settingsPath, const QString& cmdLinePrefix, const QVariant& defaultValue);

    void addDependentParameter(ParameterDependence dependence);
protected:
    void setWidgetValue(const QVariant& newValue);
    QVariant getWidgetValue();
private slots:
    void stateChanged(int newState);
private:
    QCheckBox* inputWidget;
    QList<ParameterDependence> dependentParameters;
};

class U2GUI_EXPORT RadioButtonController : public InputWidgetController {
    Q_OBJECT
public:
    RadioButtonController(QRadioButton* inputWidget, const QString& settingsPath, const QString& cmdLinePrefix, const QVariant& defaultValue);
protected:
    void setWidgetValue(const QVariant& newValue);
    QVariant getWidgetValue();
private:
    QRadioButton* inputWidget;
};

class U2GUI_EXPORT ComboBoxController : public InputWidgetController {
    Q_OBJECT
public:
    ComboBoxController(QComboBox* inputWidget, const QString& settingsPath, const QString& cmdLinePrefix, const QVariant& defaultValue, const QStringList& parameters = QStringList());

    void addParameterToCmdLineSettings(QStringList& settings);
protected:
    void setWidgetValue(const QVariant& newValue);
    QVariant getWidgetValue();
private:
    QComboBox*  inputWidget;
    QStringList parameters;
};

class U2GUI_EXPORT LineEditController : public InputWidgetController {
    Q_OBJECT
public:
    LineEditController(QLineEdit* inputWidget, const QString& settingsPath, const QString& cmdLinePrefix, const QVariant& defaultValue);
protected:
    void setWidgetValue(const QVariant& newValue);
    QVariant getWidgetValue();
private:
    QLineEdit*  inputWidget;
};

class U2GUI_EXPORT WidgetControllersContainer {
public:
    WidgetControllersContainer() {}
    ~WidgetControllersContainer();

    InputWidgetController* addWidgetController(QCheckBox* inputWidget, const QString& seetingsPath, const QString& cmdLinePreffix);
    InputWidgetController* addWidgetController(QRadioButton* inputWidget, const QString& seetingsPath, const QString& cmdLinePreffix);
    InputWidgetController* addWidgetController(QSpinBox* inputWidget, const QString& seetingsPath, const QString& cmdLinePreffix);
    InputWidgetController* addWidgetController(QDoubleSpinBox* inputWidget, const QString& seetingsPath, const QString& cmdLinePreffix);
    InputWidgetController* addWidgetController(QComboBox* inputWidget, const QString& seetingsPath, const QString& cmdLinePreffix, const QStringList& parameters = QStringList());
    InputWidgetController* addWidgetController(QLineEdit* inputWidget, const QString& seetingsPath, const QString& cmdLinePreffix);

    void storeSettings();
    void restoreDefault();
    void getDataFromSettings();
    void addParametersToCmdLine(QStringList& cmdLineSettings);

private:
    InputWidgetController* addWidget(InputWidgetController* inputWidget);
    QList<InputWidgetController*> widgetControllers;
};
} //namespace

#endif // _U2_INPUT_WIDGETS_CONTROLLERS_H
