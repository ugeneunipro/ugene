/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "MSAEditorMultilineOverviewArea.h"

#include <QActionGroup>
#include <QContextMenuEvent>
#include <QVBoxLayout>

#include "MSAEditor.h"
#include "MSAEditorSequenceArea.h"
#include "overview/MaGraphMultilineOverview.h"
#include "overview/MaMultilineOverviewContextMenu.h"
#include "overview/MaSimpleMultilineOverview.h"

namespace U2 {

const QString MSAEditorMultilineOverviewArea::OVERVIEW_AREA_OBJECT_NAME = "msa_multiline_overview_area";

MSAEditorMultilineOverviewArea::MSAEditorMultilineOverviewArea(MaEditorMultilineWgt *ui)
    : MaEditorMultilineOverviewArea(ui, OVERVIEW_AREA_OBJECT_NAME) {
    // TODO:ichebyki
    // change ui
    graphOverview = new MaGraphMultilineOverview(ui);
    graphOverview->setObjectName(OVERVIEW_AREA_OBJECT_NAME + "_graph");

    // TODO:ichebyki
    // change ui
    simpleOverview = new MaSimpleMultilineOverview(ui);
    simpleOverview->setObjectName(OVERVIEW_AREA_OBJECT_NAME + "_simple");
    simpleOverview->setVisible(false);  // "Simple Overview" is hidden by default.

    addOverview(simpleOverview);
    addOverview(graphOverview);

    MaEditorWgt *child = nullptr;
    for (int i = 0; (child = ui->getEditor()->getUI(i)) != nullptr; i++) {
        connect(child->getSequenceArea(), SIGNAL(si_highlightingChanged()), simpleOverview, SLOT(sl_highlightingChanged()));
        connect(child->getSequenceArea(), SIGNAL(si_highlightingChanged()), graphOverview, SLOT(sl_highlightingChanged()));
    }
    connect(ui->getEditor(), SIGNAL(si_referenceSeqChanged(qint64)), graphOverview, SLOT(sl_highlightingChanged()));
    connect(ui->getEditor(), SIGNAL(si_referenceSeqChanged(qint64)), simpleOverview, SLOT(sl_highlightingChanged()));

    contextMenu = new MaMultilineOverviewContextMenu(this, simpleOverview, graphOverview);
    setContextMenuPolicy(Qt::DefaultContextMenu);

    connect(contextMenu, SIGNAL(si_graphTypeSelected(MaGraphOverviewDisplaySettings::GraphType)), graphOverview, SLOT(sl_graphTypeChanged(MaGraphOverviewDisplaySettings::GraphType)));
    connect(contextMenu, SIGNAL(si_colorSelected(QColor)), graphOverview, SLOT(sl_graphColorChanged(QColor)));
    connect(contextMenu, SIGNAL(si_graphOrientationSelected(MaGraphOverviewDisplaySettings::OrientationMode)), graphOverview, SLOT(sl_graphOrientationChanged(MaGraphOverviewDisplaySettings::OrientationMode)));
    connect(contextMenu, SIGNAL(si_calculationMethodSelected(MaGraphCalculationMethod)), graphOverview, SLOT(sl_calculationMethodChanged(MaGraphCalculationMethod)));

    setMaximumHeight(graphOverview->FIXED_HEIGHT + simpleOverview->FIXED_HEIGTH + 5);
}

void MSAEditorMultilineOverviewArea::contextMenuEvent(QContextMenuEvent *event) {
    contextMenu->exec(event->globalPos());
}

void MSAEditorMultilineOverviewArea::cancelRendering() {
    graphOverview->cancelRendering();
    MaEditorMultilineOverviewArea::cancelRendering();
}

void MSAEditorMultilineOverviewArea::sl_show() {
    MaEditorMultilineOverviewArea::sl_show();
    if (graphOverview->isVisible()) {
        graphOverview->sl_unblockRendering(true);
    } else {
        graphOverview->sl_blockRendering();
        cancelRendering();
    }
}

}  // namespace U2
