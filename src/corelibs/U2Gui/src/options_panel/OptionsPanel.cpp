/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "OptionsPanel.h"

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/OPWidgetFactory.h>
#include <U2Gui/OPWidgetFactoryRegistry.h>

#include "GroupHeaderImageWidget.h"
#include "GroupOptionsWidget.h"
#include "OptionsPanelWidget.h"

namespace U2 {

OptionsPanel::OptionsPanel(GObjectView* _objView)
    : objView(_objView) {
    widget = new OptionsPanelWidget();
}

OptionsPanel::~OptionsPanel() {
    if (nullptr == widget->parentWidget()) {
        delete widget;
    }
}

OptionsPanelWidget* OptionsPanel::getMainWidget() {
    return widget;
}

void OptionsPanel::addGroup(OPWidgetFactory* factory) {
    // Create a widget with icon at the right side
    OPGroupParameters groupParameters = factory->getOPGroupParameters();
    GroupHeaderImageWidget* headerImageWidget =
        widget->createHeaderImageWidget(groupParameters.getGroupId(), groupParameters.getIcon());

    headerImageWidget->setObjectName(groupParameters.getGroupId());

    // Listen for signals from the header image widget
    connect(headerImageWidget, SIGNAL(si_groupHeaderPressed(QString)), this, SLOT(sl_groupHeaderPressed(QString)));

    // Add the factory
    opWidgetFactories.append(factory);
}

void OptionsPanel::openGroupById(const QString& groupId, const QVariantMap& options) {
    if (widget->getState() == OPMainWidgetState_Closed) {
        widget->openOptionsPanel();
    } else if (activeGroupId != groupId) {
        closeOptionsGroup(activeGroupId);
    }
    openOptionsGroup(groupId, options);
}

void OptionsPanel::sl_groupHeaderPressed(QString groupId) {
    OPWidgetFactory* opWidgetFactory = findFactoryByGroupId(groupId);
    SAFE_POINT(nullptr != opWidgetFactory,
               QString("Internal error: can't open a group with ID '%1' on the Options Panel.").arg(groupId), );

    // Implement the logic of the groups opening/closing
    if (widget->getState() == OPMainWidgetState_Closed) {
        if (activeGroupId != groupId) {
            closeOptionsGroup(activeGroupId);
        }
        widget->openOptionsPanel();
        openOptionsGroup(groupId);
        return;
    }

    // The already opened group is the currently selected one
    if (activeGroupId == groupId) {
        widget->closeOptionsPanel();
        closeOptionsGroup(groupId);
        return;
    } else {  // Another group has been selected
        closeOptionsGroup(activeGroupId);
        openOptionsGroup(groupId);
    }
}

void OptionsPanel::openOptionsGroup(const QString& groupId, const QVariantMap& options) {
    GCounter::increment(QString("Opening tab: %1").arg(groupId), objView->getFactoryId());
    SAFE_POINT(!groupId.isEmpty(), "Empty 'groupId'!", );

    OPWidgetFactory* opWidgetFactory = findFactoryByGroupId(groupId);
    SAFE_POINT(opWidgetFactory != nullptr,
               QString("Internal error: can't open a group with ID '%1' on the Options Panel.").arg(groupId), );

    if (activeGroupId == groupId) {
        GroupOptionsWidget* optionsWidget = widget->focusOptionsWidget(groupId);
        if (optionsWidget != nullptr) {
            opWidgetFactory->applyOptionsToWidget(optionsWidget->getMainWidget(), options);
        }
        return;
    }

    GroupHeaderImageWidget* headerWidget = widget->findHeaderWidgetByGroupId(groupId);
    SAFE_POINT(headerWidget != nullptr,
               QString("Internal error: can't find a header widget for group '%1'").arg(groupId), );

    OPGroupParameters parameters = opWidgetFactory->getOPGroupParameters();

    // Get common widgets
    OPCommonWidgetFactoryRegistry* opCommonWidgetFactoryRegistry = AppContext::getOPCommonWidgetFactoryRegistry();
    QList<OPCommonWidgetFactory*> opCommonWidgetFactories = opCommonWidgetFactoryRegistry->getRegisteredFactories(groupId);

    QList<QWidget*> commonWidgets;
    foreach (OPCommonWidgetFactory* commonWidgetFactory, opCommonWidgetFactories) {
        SAFE_POINT(nullptr != commonWidgetFactory, "NULL OP common widget factory!", );
        QWidget* commonWidget = commonWidgetFactory->createWidget(objView, options);
        commonWidgets.append(commonWidget);
    }

    // Create the tab widget
    QWidget* mainWidget = opWidgetFactory->createWidget(objView, options);
    widget->createOptionsWidget(groupId, parameters.getTitle(), parameters.getDocumentationPage(), mainWidget, commonWidgets);
    headerWidget->setHeaderSelected();
    // Re-apply options in case if they were overriden by SavableTab.
    opWidgetFactory->applyOptionsToWidget(mainWidget, options);
    activeGroupId = groupId;
}

void OptionsPanel::closeOptionsGroup(const QString& groupId) {
    if (activeGroupId != groupId || groupId.isEmpty()) {
        return;
    }

    GroupHeaderImageWidget* headerWidget = widget->findHeaderWidgetByGroupId(groupId);
    SAFE_POINT(nullptr != headerWidget, QString("Internal error: can't find a header widget for group '%1'").arg(groupId), );

    widget->deleteOptionsWidget(groupId);
    headerWidget->setHeaderDeselected();
    activeGroupId = "";
}

OPWidgetFactory* OptionsPanel::findFactoryByGroupId(const QString& groupId) {
    foreach (OPWidgetFactory* factory, opWidgetFactories) {
        OPGroupParameters parameters = factory->getOPGroupParameters();
        if (parameters.getGroupId() == groupId) {
            return factory;
        }
    }

    return nullptr;
}

}  // namespace U2
