/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "PropertyWizardController.h"

#include <U2Core/U2SafePoints.h>

#include <U2Lang/BaseTypes.h>
#include <U2Lang/WorkflowUtils.h>

#include "../PropertyWidget.h"

namespace U2 {

/************************************************************************/
/* PropertyWizardController */
/************************************************************************/
PropertyWizardController::PropertyWizardController(WizardController* wc, AttributeWidget* _widget)
    : WidgetController(wc), widget(_widget), _tags(nullptr) {
    actor = WorkflowUtils::actorById(wc->getCurrentActors(), widget->getActorId());
    wc->addPropertyController(widget->getInfo(), this);
}

PropertyWizardController::~PropertyWizardController() {
}

Attribute* PropertyWizardController::attribute() {
    return actor->getParameter(widget->getAttributeId());
}

void PropertyWizardController::sl_valueChanged(const QVariant& newValue) {
    wc->setAttributeValue(widget->getInfo(), newValue);
}

void PropertyWizardController::updateGUI(const QVariant& newValue) {
    emit si_updateGUI(newValue);
}

void PropertyWizardController::updateVisibility(bool newValue) {
    emit si_updateVisibility(newValue);
}

DelegateTags* PropertyWizardController::tags() const {
    return _tags;
}

/************************************************************************/
/* InUrlDatasetsController */
/************************************************************************/
InUrlDatasetsController::InUrlDatasetsController(WizardController* wc, AttributeWidget* widget)
    : PropertyWizardController(wc, widget), dsc(nullptr) {
}

InUrlDatasetsController::~InUrlDatasetsController() {
    delete dsc;
}

QWidget* InUrlDatasetsController::createGUI(U2OpStatus& /*os*/) {
    if (dsc != nullptr) {
        delete dsc;
    }
    QList<Dataset> sets;
    QVariant value = wc->getAttributeValue(widget->getInfo());
    if (value.canConvert<QList<Dataset>>()) {
        sets = value.value<QList<Dataset>>();
    } else {
        coreLog.error("Can not convert value to dataset list");
        sets.clear();
        sets << Dataset();
    }
    auto attr = dynamic_cast<URLAttribute*>(attribute());
    SAFE_POINT(attr != nullptr, "Unexpected attribute value", nullptr);
    const QSet<GObjectType> compatibleObjTypes = attr != nullptr ? attr->getCompatibleObjectTypes() : QSet<GObjectType>();
    dsc = new AttributeDatasetsController(sets, compatibleObjTypes);
    connect(dsc, SIGNAL(si_attributeChanged()), SLOT(sl_datasetsChanged()));
    return dsc->getWidget();
}

void InUrlDatasetsController::sl_datasetsChanged() {
    sl_valueChanged(QVariant::fromValue<QList<Dataset>>(dsc->getDatasets()));
}

/************************************************************************/
/* DefaultPropertyController */
/************************************************************************/
DefaultPropertyController::DefaultPropertyController(WizardController* wc, AttributeWidget* widget, int _labelSize)
    : PropertyWizardController(wc, widget), labelSize(_labelSize), noDelegate(false) {
}

DefaultPropertyController::~DefaultPropertyController() {
}

QWidget* DefaultPropertyController::createGUI(U2OpStatus& os) {
    CHECK_EXT(AttributeInfo::DEFAULT == widget->getProperty(AttributeInfo::TYPE),
              os.setError("Widget type is not default"),
              nullptr);

    PropertyWidget* propWidget = createPropertyWidget(os);
    CHECK_OP(os, nullptr);
    connect(propWidget, SIGNAL(si_valueChanged(const QVariant&)), SLOT(sl_valueChanged(const QVariant&)));
    connect(this, SIGNAL(si_updateGUI(const QVariant&)), propWidget, SLOT(processDelegateTags()));
    connect(this, SIGNAL(si_updateGUI(const QVariant&)), propWidget, SLOT(setValue(const QVariant&)));
    propWidget->setSchemaConfig(wc);
    propWidget->setValue(wc->getAttributeValue(widget->getInfo()));

    QString label = widget->getProperty(AttributeInfo::LABEL);
    if (label.isEmpty()) {
        label = attribute()->getDisplayName();
    }
    auto result = new LabeledPropertyWidget(label, propWidget, nullptr);
    if (labelSize >= 0) {
        result->setLabelWidth(labelSize);
    }
    if (attribute()->isRequiredAttribute()) {
        propWidget->setRequired();
    }
    connect(this, SIGNAL(si_updateVisibility(bool)), result, SLOT(setVisible(bool)));

    result->setToolTip("<html>" + attribute()->getDocumentation() + "</html>");
    return result;
}

PropertyWidget* DefaultPropertyController::createPropertyWidget(U2OpStatus& os) {
    PropertyWidget* result = nullptr;
    PropertyDelegate* delegate = nullptr;
    {
        ConfigurationEditor* editor = actor->getEditor();
        if (editor != nullptr) {
            delegate = editor->getDelegate(widget->getAttributeId());
        }
    }

    if (delegate != nullptr) {
        if (noDelegate) {
            result = new DefaultPropertyWidget();
        } else {
            result = delegate->createWizardWidget(os, nullptr);
        }

        _tags = wc->getTagsWithoutController(widget->getInfo());
        if (_tags == nullptr) {
            _tags = new DelegateTags(*delegate->tags());
        }

        result->setDelegateTags(tags());
        CHECK_OP(os, nullptr);
    } else if (BaseTypes::BOOL_TYPE() == attribute()->getAttributeType()) {
        result = ComboBoxWidget::createBooleanWidget();
    } else if (BaseTypes::URL_DATASETS_TYPE() == attribute()->getAttributeType()) {
        result = new URLWidget("", true, false, false, new DelegateTags());
    } else {
        result = new DefaultPropertyWidget();
    }

    return result;
}

void DefaultPropertyController::setNoDelegate(bool value) {
    noDelegate = value;
}

}  // namespace U2
