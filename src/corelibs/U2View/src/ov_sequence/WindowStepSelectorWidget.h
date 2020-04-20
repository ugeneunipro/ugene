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

#ifndef _U2_WINDOW_STEP_SELECTOR_WIDGET_
#define _U2_WINDOW_STEP_SELECTOR_WIDGET_

#include <U2Core/U2Region.h>
#include <QDialog>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QFormLayout>

namespace U2 {

class U2VIEW_EXPORT WindowStepSelectorWidget :  public QWidget {
    Q_OBJECT
public:
    WindowStepSelectorWidget(QWidget* p, const U2Region& winRange, int win, int step);
    QString validate() const;

    int getWindow() const;
    int getStep() const;
    QFormLayout* getFormLayout() const {return formLayout;}
private:
    QSpinBox*   windowEdit;
    QSpinBox*   stepsPerWindowEdit;
    QFormLayout* formLayout;
};

class U2VIEW_EXPORT MinMaxSelectorWidget : public QWidget {
    Q_OBJECT
public:
    MinMaxSelectorWidget(QWidget* p, double min, double max, bool enabled);
    QString validate() const;

    double getMin() const;
    double getMax() const;
    bool getState() const;

private slots:
    void sl_valueChanged(const QString &);

private:
    QGroupBox* minmaxGroup;
    QDoubleSpinBox* minBox;
    QDoubleSpinBox* maxBox;
    QPalette normalPalette;
};

class U2VIEW_EXPORT WindowStepSelectorDialog : public QDialog {
    Q_OBJECT
public:
    WindowStepSelectorDialog(QWidget* p, const U2Region& winRange, int win, int step, double min, double max, bool e);
    WindowStepSelectorWidget* getWindowStepSelector() const {return wss;}
    MinMaxSelectorWidget* getMinMaxSelector() const {return mms;}

private slots:
    void sl_onCancelClicked(bool);
    void sl_onOkClicked(bool);
private:
    WindowStepSelectorWidget* wss;
    MinMaxSelectorWidget* mms;
};

} // namespace

#endif
