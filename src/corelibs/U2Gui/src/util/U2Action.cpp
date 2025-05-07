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

#include "U2Action.h"

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/MainWindow.h>

namespace U2 {

U2Action::U2Action(const IconParameters& _iconParameters, const QString& text, QObject* parent)
    : QAction(GUIUtils::getIconResource(_iconParameters), text, parent),
      iconParameters(_iconParameters) {

    auto mw = AppContext::getMainWindow();
    SAFE_POINT_NN(mw, );

    connect(mw, &MainWindow::si_colorModeSwitched, this, &U2Action::sl_colorModeSwitched);
}

void U2Action::setIconParameters(const IconParameters& _iconParameters) {
    SAFE_POINT(_iconParameters.isEmpty(), "Not expected to change icon", );

    iconParameters = _iconParameters;
}

void U2Action::sl_colorModeSwitched() {
    setIcon(GUIUtils::getIconResource(iconParameters));
}

}  // namespace U2
