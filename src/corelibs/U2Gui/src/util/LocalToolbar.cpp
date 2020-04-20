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

#include "LocalToolbar.h"

#include <QLayout>
#include <QToolButton>

#define TOOLBAR_BUTTON_SIZE 25

namespace U2 {

LocalToolbar::LocalToolbar(QWidget *p)
    : OrderedToolbar(p, Qt::Vertical) {
    setBackgroundRole(QPalette::Window);
    setAutoFillBackground(true);

    layout()->setSpacing(0);
    layout()->setMargin(0);

    setMinimumWidth(TOOLBAR_BUTTON_SIZE + 4);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
}

void LocalToolbar::addAction(QAction* a) {
    QToolBar::addAction(a);

    QToolButton* button = qobject_cast<QToolButton*>(widgetForAction(a));
    button->setFixedSize(TOOLBAR_BUTTON_SIZE, TOOLBAR_BUTTON_SIZE);
}

} // namespace
