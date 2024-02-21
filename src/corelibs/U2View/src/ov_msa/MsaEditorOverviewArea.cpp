/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "MsaEditorOverviewArea.h"

#include <QContextMenuEvent>
#include <QVBoxLayout>

#include "MsaEditor.h"
#include "MsaEditorSequenceArea.h"
#include "overview/MaGraphOverview.h"
#include "overview/MaOverviewContextMenu.h"
#include "overview/MaSimpleOverview.h"

namespace U2 {

const QString MsaEditorOverviewArea::OVERVIEW_AREA_OBJECT_NAME = "msa_overview_area";

MsaEditorOverviewArea::MsaEditorOverviewArea(MsaEditorMultilineWgt* wgt)
    : MaEditorOverviewArea(wgt, OVERVIEW_AREA_OBJECT_NAME) {
    // The MSAEditorOverviewArea can't be resized vertically.
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    MsaEditor* editor = wgt->editor;
    graphOverview = new MaGraphOverview(editor, wgt);
    graphOverview->setObjectName(OVERVIEW_AREA_OBJECT_NAME + "_graph");

    simpleOverview = new MaSimpleOverview(editor, wgt);
    simpleOverview->setObjectName(OVERVIEW_AREA_OBJECT_NAME + "_simple");
    simpleOverview->setVisible(false);  // "Simple Overview" is hidden by default.

    addOverview(simpleOverview);
    addOverview(graphOverview);

    connect(editor, SIGNAL(si_referenceSeqChanged(qint64)), graphOverview, SLOT(sl_highlightingChanged()));
    connect(editor, SIGNAL(si_referenceSeqChanged(qint64)), simpleOverview, SLOT(sl_highlightingChanged()));

    contextMenu = new MaOverviewContextMenu(this, simpleOverview, graphOverview);
    setContextMenuPolicy(Qt::DefaultContextMenu);

    connect(contextMenu, &MaOverviewContextMenu::si_graphTypeSelected, graphOverview, &MaGraphOverview::sl_graphTypeChanged);
    connect(contextMenu, &MaOverviewContextMenu::si_colorSelected, graphOverview, &MaGraphOverview::sl_graphColorChanged);
    connect(contextMenu, &MaOverviewContextMenu::si_graphOrientationSelected, graphOverview, &MaGraphOverview::sl_graphOrientationChanged);
    connect(contextMenu, &MaOverviewContextMenu::si_calculationMethodSelected, graphOverview, &MaGraphOverview::sl_calculationMethodChanged);

    updateFixedHeightGeometry();

    simpleOverview->installEventFilter(this);
    graphOverview->installEventFilter(this);
}

bool MsaEditorOverviewArea::eventFilter(QObject* watched, QEvent* event) {
    CHECK(watched == simpleOverview || watched == graphOverview, false);
    auto type = event->type();
    if (type == QEvent::Show || type == QEvent::Hide) {
        updateFixedHeightGeometry();
    }
    return false;
}

void MsaEditorOverviewArea::updateFixedHeightGeometry() {
    int height = (simpleOverview->isVisible() ? U2::MaSimpleOverview::FIXED_HEIGHT : 0) +
                 (graphOverview->isVisible() ? U2::MaGraphOverview::FIXED_HEIGHT : 0);
    setFixedHeight(height);
}

void MsaEditorOverviewArea::contextMenuEvent(QContextMenuEvent* event) {
    contextMenu->exec(event->globalPos());
}

void MsaEditorOverviewArea::setVisible(bool isVisible) {
    MaEditorOverviewArea::setVisible(isVisible);
    updateFixedHeightGeometry();
}

}  // namespace U2
