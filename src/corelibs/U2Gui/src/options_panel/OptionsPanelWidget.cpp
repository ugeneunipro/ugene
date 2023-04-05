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

#include "OptionsPanelWidget.h"

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QSpacerItem>

#include <U2Core/U2SafePoints.h>

#include "GroupHeaderImageWidget.h"
#include "GroupOptionsWidget.h"

namespace U2 {

OptionsScrollArea::OptionsScrollArea(QWidget* parent)
    : QScrollArea(parent) {
    setObjectName("OP_SCROLL_AREA");
    setWidgetResizable(true);
    setStyleSheet("QWidget#OP_SCROLL_AREA { "
                  "border-style: none;"
                  " }");
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    setMaximumWidth(GroupOptionsWidget::getMaxWidgetWidth());
    hide();
}

QSize OptionsScrollArea::sizeHint() const {
    int BORDERS_APPROX_SIZE = 15;
    return QSize(GroupOptionsWidget::getMinWidgetWidth() + BORDERS_APPROX_SIZE, 0);
}

OptionsPanelWidget::OptionsPanelWidget(QWidget* parent)
    : QFrame(parent) {
    setObjectName("OP_MAIN_WIDGET");

    setStyleSheet("QWidget#OP_MAIN_WIDGET { "
                  "border-style: solid;"
                  "border-color: palette(shadow);"
                  "border-top-width: 1px;"
                  "border-bottom-width: 1px;"
                  " }");

    initOptionsLayout();
    QWidget* groupsWidget = initGroupsLayout();
    initMainLayout(groupsWidget);
}

QWidget* OptionsPanelWidget::initGroupsLayout() {
    groupsLayout = new QVBoxLayout();
    groupsLayout->setContentsMargins(0, 60, 0, 0);
    groupsLayout->setSpacing(0);

    // External groups layout is used to add a spacer below the image headers
    auto externalGroupsLayout = new QVBoxLayout();
    externalGroupsLayout->setContentsMargins(0, 0, 0, 0);
    externalGroupsLayout->setSpacing(0);
    externalGroupsLayout->addLayout(groupsLayout);
    externalGroupsLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // The widget is used to add additional decoration to the groups panel
    auto groupsWidget = new QWidget();
    groupsWidget->setLayout(externalGroupsLayout);
    groupsWidget->setStyleSheet(
        "background: palette(mid);"
        "border-style: solid;"
        "border-left-width: 1px;"
        "border-top-width: 0px;"
        "border-right-width: 1px;"
        "border-bottom-width: 0px;"
        "border-color: palette(shadow);");
    return groupsWidget;
}

void OptionsPanelWidget::initOptionsLayout() {
    // The widget used to add decoration and scroll to the options widgets
    optionsScrollArea = new OptionsScrollArea(this);

    optionsWidgetLayout = new QVBoxLayout();
    optionsWidgetLayout->setContentsMargins(0, 0, 0, 0);
    optionsWidgetLayout->setSpacing(0);

    auto optionsWidget = new QWidget(optionsScrollArea);
    optionsWidget->setObjectName("OP_OPTIONS_WIDGET");
    optionsWidget->setLayout(optionsWidgetLayout);
    optionsWidget->setStyleSheet("QWidget#OP_OPTIONS_WIDGET { "
                                 "background: palette(window);"
                                 "border-style: none;"
                                 "border-color: palette(shadow);"
                                 " }");

    optionsScrollArea->setWidget(optionsWidget);
}

void OptionsPanelWidget::initMainLayout(QWidget* groupsWidget) {
    auto mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignRight);  // prevents blinking when the options panel has been opened/closed
    mainLayout->addWidget(groupsWidget);
    setLayout(mainLayout);
}

QWidget* OptionsPanelWidget::getOptionsWidget() const {
    return optionsScrollArea;
}

GroupHeaderImageWidget* OptionsPanelWidget::createHeaderImageWidget(const QString& groupId, const QPixmap& image) {
    auto widget = new GroupHeaderImageWidget(groupId, image);

    // Add widget to the layout and "parent" it
    groupsLayout->addWidget(widget);

    headerWidgets.append(widget);

    return widget;
}

GroupOptionsWidget* OptionsPanelWidget::createOptionsWidget(const QString& groupId,
                                                            const QString& title,
                                                            const QString& documentationPage,
                                                            QWidget* mainWidget,
                                                            const QList<QWidget*>& commonWidgets) {
    SAFE_POINT(mainWidget != nullptr, "NULL main widget!", nullptr);
    auto innerWidgets = new QWidget();
    auto layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setMargin(0);

    foreach (QWidget* commonWidget, commonWidgets) {
        SAFE_POINT(commonWidget != nullptr, "NULL common widget!", nullptr);
        layout->addWidget(commonWidget);
    }

    layout->addWidget(mainWidget);

    innerWidgets->setLayout(layout);

    auto groupWidget = new GroupOptionsWidget(groupId, title, documentationPage, innerWidgets, mainWidget);

    // Add widget to the layout and "parent" it
    optionsWidgetLayout->insertWidget(0, groupWidget);

    optionsWidgets.insert(0, groupWidget);

    groupWidget->setFocus();

    return groupWidget;
}

void OptionsPanelWidget::openOptionsPanel() {
    optionsScrollArea->show();
}

void OptionsPanelWidget::closeOptionsPanel() {
    optionsScrollArea->hide();
}

OPMainWidgetState OptionsPanelWidget::getState() const {
    return optionsScrollArea->isVisible() ? OPMainWidgetState_Opened : OPMainWidgetState_Closed;
}

GroupHeaderImageWidget* OptionsPanelWidget::findHeaderWidgetByGroupId(const QString& groupId) {
    foreach (GroupHeaderImageWidget* widget, headerWidgets) {
        if (widget->getGroupId() == groupId) {
            return widget;
        }
    }
    return nullptr;
}

GroupOptionsWidget* OptionsPanelWidget::findOptionsWidgetByGroupId(const QString& groupId) {
    for (GroupOptionsWidget* widget : qAsConst(optionsWidgets)) {
        if (widget->getGroupId() == groupId) {
            return widget;
        }
    }
    return nullptr;
}

void OptionsPanelWidget::deleteOptionsWidget(const QString& groupId) {
    GroupOptionsWidget* optionsWidget = findOptionsWidgetByGroupId(groupId);
    SAFE_POINT(optionsWidget != nullptr,
               QString("Internal error: failed to find an options widget for group '%1' to delete it.").arg(groupId), );

    optionsWidgetLayout->removeWidget(optionsWidget);
    optionsWidgets.removeAll(optionsWidget);
    delete optionsWidget;
}

GroupOptionsWidget* OptionsPanelWidget::focusOptionsWidget(const QString& groupId) {
    GroupOptionsWidget* optionsWidget = findOptionsWidgetByGroupId(groupId);
    SAFE_POINT(optionsWidget != nullptr,
               QString("Internal error: failed to find an options widget for group '%1' to activate it.").arg(groupId),
               nullptr);
    optionsWidget->hide();
    optionsWidget->show();
    return optionsWidget;
}

}  // namespace U2
