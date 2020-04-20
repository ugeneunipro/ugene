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

#ifndef GUITESTINGWINDOWIMPL_H
#define GUITESTINGWINDOWIMPL_H

#include <U2Gui/MainWindow.h>
#include <U2Core/AppContext.h>

#include "ui_GUITestingWindow.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QMenuBar;
class QRadioButton;
class QSpinBox;
class QToolButton;

namespace U2 {
class EventFilter;

class GUITestingWindow:public QWidget, public Ui_GUITestWindow
{
    Q_OBJECT
public:
    GUITestingWindow();
    void updateTable();
    void appendCode(QString s);
    bool is_CodeGenerated(){return isCodeGenerated;}
    bool is_FillerGenerated(){return isFillerGenerated;}
    void setFillerGenerated(bool b){isFillerGenerated = b;}
private:
    void prepareGUI();
    EventFilter* filter;
    bool isCodeGenerated;
    bool isFillerGenerated;
private slots:
    void sl_getParentInfo();
    void sl_switchCodeGeneration();
    void sl_readyToGenerateFiller();
};

class EventFilter : public QObject
{
    Q_OBJECT
    friend class GUITestingWindow;

public:
    EventFilter(GUITestingWindow* _w);
    const QString& getClassName() const { return className; }
    const QString& getObjName() const { return objName; }
    const QString& getActionText() const { return text; }
    const QString& getActionName() const { return actionName; }
    QObject* getBufferObject() { return bufferObj; }
    void setBufferObject(QPointer<QObject> o) { bufferObj = o; }

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    void generateMouseMeassage();

    void getInfo(QWidget* w);

    QString generateCode(QWidget* w );
    QString setValuesWhenFocusGone(QWidget* w);
    QString menuBarCode(QMenuBar* menuBar) const;
    QString menuCode(QMenu* menu);
    QString checkBoxCode(QCheckBox* check) const;
    QString radioButtinCode(QRadioButton* radio) const;
    QString toolButtonCode(QToolButton* toolButton) const;
    QString contextMenuCode(QWidget* w) const;

    QString generateFillerHeader();

    QString generateParametersConstructorCode();
        QString defaultVarValuesCode(QWidget* widget) const;
        QString defaultVarValuesCode(QCheckBox* checkBox) const;
        QString defaultVarValuesCode(QGroupBox* groupBox) const;
        QString defaultVarValuesCode(QComboBox* comboBox) const;
        QString defaultVarValuesCode(QLineEdit* lineEdit) const;
        QString defaultVarValuesCode(QSpinBox* spinBox) const;
        QString defaultVarValuesCode(QDoubleSpinBox* spinBox) const;
        QString defaultVarValuesCode(QToolButton* toolButton) const;
        QString defaultVarValuesCode(QPushButton* pushButton) const;
        QString defaultVarValuesCode(QRadioButton* radio) const;

    QString generateParametersVariablesCode() const;
        QString widgetVariableCode(QWidget* widget) const;
        QString widgetVariableCode(QCheckBox* checkBox) const;
        QString widgetVariableCode(QGroupBox* groupBox) const;
        QString widgetVariableCode(QComboBox* comboBox) const;
        QString widgetVariableCode(QLineEdit* lineEdit) const;
        QString widgetVariableCode(QSpinBox* spinBox) const;
        QString widgetVariableCode(QDoubleSpinBox* spinBox) const;
        QString widgetVariableCode(QToolButton* toolButton) const;
        QString widgetVariableCode(QPushButton* pushButton) const;
        QString widgetVariableCode(QRadioButton* radio) const;

    QString generateFillerSource() const;

    QString generateWidgetsProcessing() const;
        QString widgetsProcessingCode(QWidget* widget) const;
        QString widgetsProcessingCode(QCheckBox* checkBox) const;
        QString widgetsProcessingCode(QGroupBox* groupBox) const;
        QString widgetsProcessingCode(QComboBox* comboBox) const;
        QString widgetsProcessingCode(QLineEdit* lineEdit) const;
        QString widgetsProcessingCode(QSpinBox* spinBox) const;
        QString widgetsProcessingCode(QDoubleSpinBox* spinBox) const;
        QString widgetsProcessingCode(QToolButton* toolButton) const;
        QString widgetsProcessingCode(QPushButton* pushButton) const;
        QString widgetsProcessingCode(QRadioButton* radio) const;

    QString className;
    QString objName;
    QString actionName;
    QString text;

    QMouseEvent* m;
    QPoint prevPos;
    QPointer<QWidget> focusWidget;
    bool isSubmenuClicked;
    Qt::MouseButton prevButton;
    GUITestingWindow* gtw;
    QPointer<QObject> bufferObj;
    QStringList classes;
};

}
#endif // GUITESTINGWINDOWIMPL_H
