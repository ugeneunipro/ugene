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

#include <QKeyEvent>
#include <QMenu>
#include <U2Core/U2SafePoints.h>

#include "MultiClickMenu.h"

namespace U2 {

MultiClickMenu::MultiClickMenu(QMenu *menu)
: QObject(menu), menu(menu)
{
    CHECK(NULL != menu, );
    menu->installEventFilter(this);
}

bool MultiClickMenu::eventFilter(QObject *watched, QEvent *event) {
    CHECK(watched == menu, false);
    CHECK(isSelectEvent(event), false);

    QAction *action = menu->activeAction();
    CHECK(NULL != action, false);

    if (action->isEnabled()) {
        action->trigger();
        return true;
    }
    return false;
}

bool MultiClickMenu::isSelectEvent(QEvent *event) {
    if (event->type() == QEvent::MouseButtonRelease) {
        return true;
    }
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
        CHECK(NULL != keyEvent, false);
        return (keyEvent->key() == Qt::Key_Enter) || (keyEvent->key() == Qt::Key_Return);
    }
    return false;
}

} // U2
