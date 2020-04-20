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

#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>

#include "RadioController.h"

namespace U2 {

static const char VAR_ID[] = "var_id";

RadioController::RadioController(WizardController *wc, RadioWidget *_rw)
: WidgetController(wc), rw(_rw)
{

}

RadioController::~RadioController() {

}

QWidget * RadioController::createGUI(U2OpStatus &/*os*/) {
    QWidget *result = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(result);
    l->setMargin(0);
#if defined(Q_OS_LINUX)
    l->setSpacing(0);
#endif

    QButtonGroup *group = new QButtonGroup(result);
    connect(group, SIGNAL(buttonClicked(QAbstractButton *)), SLOT(sl_buttonClicked(QAbstractButton *)));
    QString id = wc->getVariableValue(rw->var()).toString();
    foreach (const RadioWidget::Value &value, rw->values()) {
        QRadioButton *b = new QRadioButton(value.label);
        b->setObjectName(value.label);
        group->addButton(b);
        l->addWidget(b);

        b->setChecked(value.id == id);
        b->setProperty(VAR_ID, value.id);
        if (!value.tooltip.isEmpty()) {
            QHBoxLayout *labelLayout = new QHBoxLayout;
            labelLayout->setContentsMargins(20, 0, 0, 10);
            l->addLayout(labelLayout);

            QLabel *label = new QLabel(value.tooltip);
            label->setWordWrap(true);
            label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            label->setMinimumSize(0, 0);

            labelLayout->addWidget(label);

            b->setToolTip(value.tooltip);
        }
    }

    return result;
}

void RadioController::sl_buttonClicked(QAbstractButton *button) {
    QString id = button->property(VAR_ID).toString();
    wc->setVariableValue(rw->var(), id);
}

} // U2
