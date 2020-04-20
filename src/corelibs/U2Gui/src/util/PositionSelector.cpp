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

#include <math.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>

#include <U2Gui/U2LongLongValidator.h>
#include <U2Gui/HelpButton.h>

#include "PositionSelector.h"

namespace U2 {

PositionSelector::PositionSelector(QWidget* p, qint64 s, qint64 e, bool fixedSize)
: QWidget(p), rangeStart(s), rangeEnd(e), posEdit(NULL), autoclose(false), dialog(NULL) 
{
    init(fixedSize);

    QToolButton* goButton = new QToolButton(this);
    goButton->setText(tr("Go!"));
    goButton->setToolTip(tr("Go to position"));
    goButton->setObjectName("Go!");
    connect(goButton, SIGNAL(clicked(bool)), SLOT(sl_onButtonClicked(bool)));

    layout()->addWidget(goButton);
}

void PositionSelector::init(bool fixedSize) {
    posEdit = new QLineEdit(this);
    posEdit->setObjectName("go_to_pos_line_edit");
    posEdit->setValidator(new U2LongLongValidator(rangeStart, rangeEnd, posEdit));
    if (fixedSize) {
        int w = qMax(((int)log10((double)rangeEnd))*10, 70);
        posEdit->setFixedWidth(w);
    } else {
        posEdit->setMinimumWidth(120);
    }
    posEdit->setToolTip(tr("Enter position"));
    connect(posEdit, SIGNAL(returnPressed()), SLOT(sl_onReturnPressed()));

    QHBoxLayout* l = new QHBoxLayout(this);
    if (fixedSize) {
        l->setContentsMargins(5, 0, 5, 0);
        l->setSizeConstraint(QLayout::SetFixedSize);
    } else {
        l->setMargin(0);
    }

    setLayout(l);
    
    if (dialog!=NULL) {
        QLabel* posLabel = new QLabel(tr("Position"), this);
        posLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        l->addWidget(posLabel);
    }

    l->addWidget(posEdit);
}

PositionSelector::PositionSelector(QDialog* dialog, qint64 rangeStart, qint64 rangeEnd, bool _a)
: QWidget(dialog), rangeStart(rangeStart), rangeEnd(rangeEnd), posEdit(NULL), autoclose(_a), dialog(dialog)
{
    init(false);

    QPushButton* okButton = new QPushButton(this);
    okButton->setText(tr("Go!"));
    okButton->setObjectName("okButton");
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked(bool)), SLOT(sl_onButtonClicked(bool)));

    QPushButton* cancelButton = new QPushButton(this);
    cancelButton->setText(tr("Cancel"));
    cancelButton->setObjectName("cancelButton");
    connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));

    QPushButton* helpButton = new QPushButton(this);
    helpButton->setText(tr("Help"));
    helpButton->setObjectName("helpButton");
    new HelpButton(dialog, helpButton, "24748755");

    QHBoxLayout* l3 = new QHBoxLayout();
    l3->setMargin(0);
    l3->addStretch();
    l3->addWidget(okButton);
    l3->addWidget(cancelButton);
    l3->addWidget(helpButton);

    QVBoxLayout* l2 = new QVBoxLayout();
    l2->addWidget(this);
    l2->addStretch();
    l2->addLayout(l3);

    dialog->setLayout(l2);
    dialog->setMinimumWidth(200);
    dialog->setMaximumWidth(400);

    //todo: add checkbox to handle 'autoclose' state
}

PositionSelector::~PositionSelector(){ 
}

void PositionSelector::updateRange(qint64 _rangeStart, qint64 _rangeEnd) {
    rangeStart = _rangeStart;
    rangeEnd = _rangeEnd;

    const QValidator* oldValidator = posEdit->validator();
    posEdit->setValidator(new U2LongLongValidator(_rangeStart, _rangeEnd, posEdit));
    delete oldValidator;

    // force the validation
    QString position = posEdit->text();
    posEdit->clear();
    posEdit->insert(position);
}


void PositionSelector::sl_onButtonClicked(bool checked) {
    Q_UNUSED(checked);
    exec();
}

void PositionSelector::sl_onReturnPressed(){
    exec();
}

void PositionSelector::exec() {
    bool ok = false;
    QString text = posEdit->text().remove(' ').remove(',');
    qint64 v = text.toLongLong(&ok);
    if (!ok || v < rangeStart || v > rangeEnd) {
        return;
    }

    emit si_positionChanged(v);
    
    if (dialog!=NULL && autoclose) {
        dialog->accept();
    }
}


} //namespace
