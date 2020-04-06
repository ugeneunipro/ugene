/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2019 UniPro <ugene@unipro.ru>
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

#include <U2Core/U2SafePoints.h>

#include "..\AssemblyBrowser.h"
#include "AssemblyAnnotationsRenderArea.h"

namespace U2 {

AssemblyAnnotationsRenderArea::AssemblyAnnotationsRenderArea
            (AssemblyBrowserUi *_ui, PanView *d, PanViewRenderer *renderer)
                              : PanViewRenderArea(d, renderer),
                                ui(_ui) {}

void AssemblyAnnotationsRenderArea::paintEvent(QPaintEvent *e) {
    AssemblyBrowser* browser = ui->getWindow();
    SAFE_POINT(nullptr != browser, tr("Assembly Browser is missed"), );

    if (browser->areCellsVisible()) {
        PanViewRenderArea::paintEvent(e);
    } else  {
        cachedView->fill(Qt::transparent);
    }
}

} // U2
