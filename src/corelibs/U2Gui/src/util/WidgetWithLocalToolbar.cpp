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

#include "WidgetWithLocalToolbar.h"
#include "LocalToolbar.h"

#include <U2Core/U2SafePoints.h>

#include <QHBoxLayout>


namespace U2 {

WidgetWithLocalToolbar::WidgetWithLocalToolbar(QWidget *p)
    : QWidget(p) {

    toolBar = new LocalToolbar(this);
    contentWidget = new QWidget(this);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addWidget(toolBar);
    layout->addWidget(contentWidget);

    contentWidget->setMouseTracking(true);
    setMouseTracking(true);
    QWidget::setLayout(layout);
}

void WidgetWithLocalToolbar::setContentLayout(QLayout *l) {
    delete contentWidget->layout();

    contentWidget->setLayout(l);
    update();
}

QToolButton* WidgetWithLocalToolbar::addActionToLocalToolbar(QAction *action) {
    SAFE_POINT(action != NULL, "Trying to add NULL action to toolbar", NULL);
    toolBar->addAction(action);
    update();

    return qobject_cast<QToolButton*>(toolBar->widgetForAction(action));
}

void WidgetWithLocalToolbar::setLocalToolBarObjectName(const QString &name) {
    toolBar->setObjectName(name);
}

void WidgetWithLocalToolbar::setLocalToolbarVisible(bool visible) {
    toolBar->setVisible(visible);
    update();
}

} // namespace
