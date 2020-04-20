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

#include "MsaHighlightingSavableTab.h"
#include "MSAHighlightingTab.h"

#include <U2Gui/U2WidgetStateStorage.h>
#include <U2Core/U2SafePoints.h>

namespace U2 {

MsaHighlightingSavableTab::MsaHighlightingSavableTab(QWidget *wrappedWidget, MWMDIWindow *contextWindow)
    : MsaOpSavableTab(wrappedWidget, contextWindow)
{
    SAFE_POINT(NULL != qobject_cast<MSAHighlightingTab *>(wrappedWidget), "Invalid widget provided", );
}

MsaHighlightingSavableTab::~MsaHighlightingSavableTab() {
    U2WidgetStateStorage::saveWidgetState(*this);
    widgetStateSaved = true;
}

bool MsaHighlightingSavableTab::childCanBeSaved(QWidget *child) const {
    if (widgetsNotToSave.contains(child->objectName())) {
        return false;
    } else {
        return U2SavableWidget::childCanBeSaved(child);
    }
}

void MsaHighlightingSavableTab::disableSavingForWidgets(const QStringList &s) {
    widgetsNotToSave.append(s);
}

}
