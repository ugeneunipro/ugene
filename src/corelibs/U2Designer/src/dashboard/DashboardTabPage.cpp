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

#include "DashboardTabPage.h"

#include "DashboardWidget.h"

#include <U2Core/AppContext.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>

namespace U2 {

DashboardTabPage::DashboardTabPage(const QString& tabObjectName) {
    setObjectName(tabObjectName);
    sl_colorModeSwitched();

    auto mainWidget = new QWidget();

    auto layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    mainWidget->setLayout(layout);

    auto styleRootWidget = new QWidget();
    styleRootWidget->setObjectName("tabPageStyleRoot");
    layout->addWidget(styleRootWidget);

    auto styleRootWidgetLayout = new QHBoxLayout();
    styleRootWidgetLayout->setContentsMargins(0, 0, 0, 0);
    styleRootWidgetLayout->setSpacing(0);
    styleRootWidget->setLayout(styleRootWidgetLayout);

    styleRootWidgetLayout->addStretch(1);
    auto centerWidget = new QWidget();
    centerWidget->setMinimumWidth(1150);
    styleRootWidgetLayout->addWidget(centerWidget);
    styleRootWidgetLayout->addStretch(1);

    auto centerWidgetLayout = new QHBoxLayout();
    centerWidgetLayout->setSpacing(0);
    centerWidgetLayout->setContentsMargins(0, 0, 0, 0);
    centerWidget->setLayout(centerWidgetLayout);

    int widgetSpacing = 20;
    leftColumnLayout = new QVBoxLayout();
    leftColumnLayout->setSpacing(widgetSpacing);
    leftColumnLayout->setContentsMargins(widgetSpacing, widgetSpacing, widgetSpacing / 2, widgetSpacing);
    leftColumnLayout->addStretch(1000);
    centerWidgetLayout->addLayout(leftColumnLayout);

    rightColumnLayout = new QVBoxLayout();
    rightColumnLayout->setSpacing(widgetSpacing);
    rightColumnLayout->setContentsMargins(widgetSpacing / 2, widgetSpacing, widgetSpacing, widgetSpacing);
    rightColumnLayout->addStretch(1000);
    centerWidgetLayout->addLayout(rightColumnLayout);

    setWidget(mainWidget);
    setWidgetResizable(true);  // make the widget to fill whole available space

    connect(AppContext::getMainWindow(), &MainWindow::si_colorModeSwitched, this, &DashboardTabPage::sl_colorModeSwitched);
}

DashboardWidget* DashboardTabPage::addDashboardWidget(const QString& title, QWidget* contentWidget) {
    auto layout = leftColumnLayout->count() <= rightColumnLayout->count() ? leftColumnLayout : rightColumnLayout;
    auto dashboardWidget = new DashboardWidget(title, contentWidget);
    layout->insertWidget(layout->count() - 1, dashboardWidget);  // the last is stretch.
    return dashboardWidget;
}

void DashboardTabPage::sl_colorModeSwitched() {
    QString backgroundImagePath = GUIUtils::getResourceName("U2Designer", "background.png");
    setStyleSheet(QString("QWidget#tabPageStyleRoot {"
                  " background: url('%1') repeat scroll 0 0 transparent; "
                  " padding: 10px;"
                  "}").arg(backgroundImagePath));

}

}  // namespace U2
