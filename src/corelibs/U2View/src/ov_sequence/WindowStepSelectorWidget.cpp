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

#include <limits.h>

#include <QPushButton>
#include <QMessageBox>
#include <QLineEdit>

#include "WindowStepSelectorWidget.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
// WindowStepSelectorWidget

WindowStepSelectorWidget::WindowStepSelectorWidget(QWidget* p, const U2Region& winRange, int win, int step) : QWidget(p)
{
    assert(win >= step);

    windowEdit = new QSpinBox(this);
    windowEdit->setRange(winRange.startPos, winRange.endPos());
    windowEdit->setValue(win);
    windowEdit->setAlignment(Qt::AlignLeft);
    windowEdit->setObjectName("windowEdit");

    stepsPerWindowEdit = new QSpinBox(this);
    stepsPerWindowEdit->setRange(1, winRange.endPos());
    stepsPerWindowEdit->setValue(win/step);
    stepsPerWindowEdit->setAlignment(Qt::AlignLeft);
    stepsPerWindowEdit->setObjectName("stepsPerWindowEdit");

    formLayout = new QFormLayout(this);
    formLayout->setMargin(0);
    formLayout->addRow(tr("Window"), windowEdit);
    formLayout->addRow(tr("Steps per window"), stepsPerWindowEdit);
    setLayout(formLayout);
}

int WindowStepSelectorWidget::getWindow() const {
    assert(validate().isEmpty());
    return windowEdit->value();
}

int WindowStepSelectorWidget::getStep() const {
    assert(validate().isEmpty());
    return windowEdit->value() / stepsPerWindowEdit->value();
}

QString WindowStepSelectorWidget::validate() const {
    int win = windowEdit->value();
    int stepsPerWindow = stepsPerWindowEdit->value();
    if (win % stepsPerWindow !=0) {
        stepsPerWindowEdit->setFocus(Qt::NoFocusReason);
        return tr("Illegal step value");
    }
    int step = win / stepsPerWindow;
    if (step > win) {
        stepsPerWindowEdit->setFocus(Qt::NoFocusReason);
        return tr("Invalid step value");
    }
    return QString();
}
//////////////////////////////////////////////////////////////////////////
///

class MinMaxDoubleSpinBox : public QDoubleSpinBox {
public:
    QLineEdit* getLineEdit() const { return lineEdit();}
};

MinMaxSelectorWidget::MinMaxSelectorWidget(QWidget* p, double min, double max, bool enabled) {
    Q_UNUSED(p);

    minmaxGroup = new QGroupBox(QString(tr("Cutoff for minimum and maximum values")), this);
    minmaxGroup->setCheckable(true);
    minmaxGroup->setChecked(enabled);
    minmaxGroup->setObjectName("minmaxGroup");

    //for range use min max of type
    minBox = new MinMaxDoubleSpinBox;
    minBox->setRange(INT_MIN,INT_MAX );
    minBox->setValue(min);
    minBox->setDecimals(2);
    minBox->setAlignment(Qt::AlignLeft);
    minBox->setObjectName("minBox");

    maxBox = new MinMaxDoubleSpinBox ;
    maxBox->setRange(INT_MIN,INT_MAX);
    maxBox->setValue(max);
    maxBox->setDecimals(2);
    maxBox->setAlignment(Qt::AlignLeft);
    maxBox->setObjectName("maxBox");

    normalPalette = maxBox->palette();

    QFormLayout* l = new QFormLayout;
    l->setSizeConstraint(QLayout::SetMinAndMaxSize);
    l->addRow(tr("Minimum"), minBox);
    l->addRow(tr("Maximum"), maxBox);
    minmaxGroup->setLayout(l);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->setMargin(0);
    mainLayout->addWidget(minmaxGroup);
    setLayout(mainLayout);

    connect(minBox, SIGNAL(valueChanged(const QString &)), SLOT(sl_valueChanged(const QString &)));
    connect(maxBox, SIGNAL(valueChanged(const QString &)), SLOT(sl_valueChanged(const QString &)));
}

double MinMaxSelectorWidget::getMin() const {
    assert(validate().isEmpty());
    return minBox->value();
}

double MinMaxSelectorWidget::getMax() const {
    assert(validate().isEmpty());
    return maxBox->value();
}

bool MinMaxSelectorWidget::getState() const {
    assert(validate().isEmpty());
    return minmaxGroup->isChecked();
}

void MinMaxSelectorWidget::sl_valueChanged(const QString &) {
    double min = minBox->value();
    double max = maxBox->value();
    QPalette p = normalPalette;
    if (min >= max) {
        p.setColor(QPalette::Base, QColor(255,200,200));
    }
    ((MinMaxDoubleSpinBox*)minBox)->getLineEdit()->setPalette(p);
    ((MinMaxDoubleSpinBox*)maxBox)->getLineEdit()->setPalette(p);
}


QString MinMaxSelectorWidget::validate() const {
    if (!minmaxGroup->isChecked()) return QString();
    double min = minBox->value();
    double max = maxBox->value();
    if (min >= max) {
        minBox->setFocus(Qt::NoFocusReason);
        return tr("Invalid cutoff range");
    }
    return QString();
}

//////////////////////////////////////////////////////////////////////////
/// Dialog


WindowStepSelectorDialog::WindowStepSelectorDialog(QWidget* p, const U2Region& winRange, int win, int step, double min, double max, bool e) : QDialog(p) {
    wss = new WindowStepSelectorWidget(this, winRange, win, step);
    mms = new MinMaxSelectorWidget(this, min, max, e);
    QVBoxLayout* l = new QVBoxLayout();
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch(10);
    QPushButton* cancelButton = new QPushButton(tr("Cancel"), this);
    QPushButton* okButton = new QPushButton(tr("OK"), this);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);

    l->addWidget(wss);
    l->addWidget(mms);
    l->addLayout(buttonsLayout);

    setLayout(l);
    setWindowTitle(tr("Graph Settings"));
    setWindowIcon(QIcon(":core/images/graphs.png"));

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumWidth(400);

    connect(cancelButton, SIGNAL(clicked(bool)), SLOT(sl_onCancelClicked(bool)));
    connect(okButton, SIGNAL(clicked(bool)), SLOT(sl_onOkClicked(bool)));

    okButton->setDefault(true);
}

void WindowStepSelectorDialog::sl_onCancelClicked(bool v) {
    Q_UNUSED(v);
    reject();
}

void WindowStepSelectorDialog::sl_onOkClicked(bool v) {
    Q_UNUSED(v);
    QString err = wss->validate();
    QString mmerr = mms->validate();
    if (err.isEmpty() && mmerr.isEmpty()) {
        accept();
        return;
    }
    QMessageBox::critical(this, tr("Error!"), err.append(' ').append(mmerr));
}


} //namespace
