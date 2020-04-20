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

#include <U2Lang/ElementSelectorWidget.h>

#include "../PropertyWidget.h"
#include "WizardController.h"

#include "ElementSelectorController.h"

namespace U2 {

ElementSelectorController::ElementSelectorController(WizardController *wc,
    ElementSelectorWidget *_widget, int _labelSize)
: WidgetController(wc), widget(_widget), labelSize(_labelSize)
{

}

ElementSelectorController::~ElementSelectorController() {

}

QWidget * ElementSelectorController::createGUI(U2OpStatus & /*os*/) {
    QList<ComboItem> values;
    foreach (const SelectorValue &value, widget->getValues()) {
        values.append(qMakePair(value.getName(), value.getValue()));
    }
    ComboBoxWidget *cb = new ComboBoxWidget(values);
    connect(cb, SIGNAL(si_valueChanged(const QVariant &)), SLOT(sl_valueChanged(const QVariant &)));
    cb->setValue(wc->getSelectorValue(widget));

    LabeledPropertyWidget *result = new LabeledPropertyWidget(widget->getLabel(), cb, NULL);
    if (labelSize >= 0) {
        result->setLabelWidth(labelSize);
    }

    return result;
}

void ElementSelectorController::sl_valueChanged(const QVariant &newValue) {
    wc->setSelectorValue(widget, newValue);
}

} // U2
