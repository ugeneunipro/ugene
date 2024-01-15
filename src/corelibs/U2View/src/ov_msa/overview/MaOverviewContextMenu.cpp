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

#include "MaOverviewContextMenu.h"

#include <U2Core/GUrlUtils.h>
#include <U2Core/MsaObject.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/ExportImageDialog.h>
#include <U2Gui/MainWindow.h>

#include "MaOverviewImageExportTask.h"
#include "MaSimpleOverview.h"
#include "ov_msa/MaEditor.h"
#include "ov_msa/MsaEditorOverviewArea.h"

namespace U2 {

MaOverviewContextMenu::MaOverviewContextMenu(QWidget* parent, MaSimpleOverview* sOverview, MaGraphOverview* gOverview)
    : QMenu(parent),
      simpleOverview(sOverview),
      graphOverview(gOverview) {
    SAFE_POINT(simpleOverview != nullptr, "Overview is NULL", );
    SAFE_POINT(graphOverview != nullptr, "Graph overview is NULL", );

    setObjectName("msa_overview_context_menu");

    initSimpleOverviewAction();
    initExportAsImageAction();
    addSeparator();
    initDisplaySettingsMenu();
    initCalculationMethodMenu();

    colorAction = new QAction(tr("Set color..."), this);
    colorAction->setObjectName("Set color");
    displaySettingsMenu->addAction(colorAction);

    connectSlots();
}

void MaOverviewContextMenu::connectSlots() {
    connect(showSimpleOverviewAction, SIGNAL(toggled(bool)), simpleOverview, SLOT(setVisible(bool)));

    connect(exportAsImageAction, SIGNAL(triggered()), SLOT(sl_exportAsImageTriggered()));
    connect(graphOverview, SIGNAL(si_renderingStateChanged(bool)), exportAsImageAction, SLOT(setDisabled(bool)));

    connect(graphTypeActionGroup, SIGNAL(triggered(QAction*)), SLOT(sl_graphTypeActionTriggered(QAction*)));

    connect(orientationActionGroup, SIGNAL(triggered(QAction*)), SLOT(sl_graphOrientationActionTriggered(QAction*)));

    connect(colorAction, SIGNAL(triggered()), SLOT(sl_colorActionTriggered()));

    connect(calculationMethodActionGroup, SIGNAL(triggered(QAction*)), SLOT(sl_calculationMethodActionTriggered(QAction*)));
}

void MaOverviewContextMenu::sl_exportAsImageTriggered() {
    MaOverviewImageExportController factory(simpleOverview, graphOverview);
    QWidget* p = (QWidget*)AppContext::getMainWindow()->getQMainWindow();
    const QString fileName = GUrlUtils::fixFileName(graphOverview->getEditor()->getMaObject()->getGObjectName());
    QObjectScopedPointer<ExportImageDialog> dialog = new ExportImageDialog(&factory, ExportImageDialog::MSA, fileName, ExportImageDialog::NoScaling, p);
    dialog->exec();
}

void MaOverviewContextMenu::sl_graphTypeActionTriggered(QAction* action) {
    if (action == lineGraphAction) {
        emit si_graphTypeSelected(MaGraphOverviewDisplaySettings::Line);
    }
    if (action == areaGraphAction) {
        emit si_graphTypeSelected(MaGraphOverviewDisplaySettings::Area);
    }
    if (action == histogramGraphAction) {
        emit si_graphTypeSelected(MaGraphOverviewDisplaySettings::Histogram);
    }
}

void MaOverviewContextMenu::sl_graphOrientationActionTriggered(QAction* action) {
    if (action == topToBottomOrientationAction) {
        emit si_graphOrientationSelected(MaGraphOverviewDisplaySettings::FromTopToBottom);
    } else {
        emit si_graphOrientationSelected(MaGraphOverviewDisplaySettings::FromBottomToTop);
    }
}

void MaOverviewContextMenu::sl_colorActionTriggered() {
    QObjectScopedPointer<QColorDialog> colorDialog = new U2ColorDialog(graphOverview->getDisplaySettings().color, this);
    colorDialog->exec();
    CHECK(!colorDialog.isNull(), );

    if (colorDialog->result() == QDialog::Accepted) {
        emit si_colorSelected(colorDialog->selectedColor());
    }
}

void MaOverviewContextMenu::sl_calculationMethodActionTriggered(QAction* action) {
    if (action == strictMethodAction) {
        emit si_calculationMethodSelected(MaGraphCalculationMethod::Strict);
    }
    if (action == gapMethodAction) {
        emit si_calculationMethodSelected(MaGraphCalculationMethod::Gaps);
    }
    if (action == clustalMethodAction) {
        emit si_calculationMethodSelected(MaGraphCalculationMethod::Clustal);
    }
    if (action == highlightingMethodAction) {
        emit si_calculationMethodSelected(MaGraphCalculationMethod::Highlighting);
    }
}

void MaOverviewContextMenu::initSimpleOverviewAction() {
    showSimpleOverviewAction = createCheckableAction(tr("Show simple overview"));
    showSimpleOverviewAction->setObjectName("Show simple overview");
    showSimpleOverviewAction->setChecked(simpleOverview->isVisible());
    addAction(showSimpleOverviewAction);
}

void MaOverviewContextMenu::initExportAsImageAction() {
    exportAsImageAction = new QAction(tr("Export overview as image"), this);
    exportAsImageAction->setObjectName("export_overview_as_image_action");
    addAction(exportAsImageAction);
}

void MaOverviewContextMenu::initDisplaySettingsMenu() {
    displaySettingsMenu = addMenu(tr("Display settings..."));
    displaySettingsMenu->menuAction()->setObjectName("Display settings");
    initGraphTypeSubmenu();
    initOrientationSubmenu();
}

void MaOverviewContextMenu::initCalculationMethodMenu() {
    calculationMethodMenu = addMenu(tr("Calculation method..."));

    calculationMethodActionGroup = new QActionGroup(calculationMethodMenu);
    strictMethodAction = createCheckableAction(tr("Strict"), calculationMethodActionGroup);
    gapMethodAction = createCheckableAction(tr("Gaps"), calculationMethodActionGroup);
    clustalMethodAction = createCheckableAction(tr("Clustal"), calculationMethodActionGroup);
    highlightingMethodAction = createCheckableAction(tr("Highlighting"), calculationMethodActionGroup);

    calculationMethodMenu->menuAction()->setObjectName("Calculation method");
    strictMethodAction->setObjectName("Strict");
    gapMethodAction->setObjectName("Gaps");
    clustalMethodAction->setObjectName("Clustal");
    highlightingMethodAction->setObjectName("Highlighting");

    switch (graphOverview->getState().method) {
        case MaGraphCalculationMethod::Strict:
            strictMethodAction->setChecked(true);
            break;
        case MaGraphCalculationMethod::Gaps:
            gapMethodAction->setChecked(true);
            break;
        case MaGraphCalculationMethod::Clustal:
            clustalMethodAction->setChecked(true);
            break;
        case MaGraphCalculationMethod::Highlighting:
            highlightingMethodAction->setChecked(true);
            break;
    }

    calculationMethodMenu->addActions(calculationMethodActionGroup->actions());
}

void MaOverviewContextMenu::initGraphTypeSubmenu() {
    graphTypeMenu = displaySettingsMenu->addMenu(tr("Graph type"));
    graphTypeMenu->menuAction()->setObjectName("Graph type");

    graphTypeActionGroup = new QActionGroup(graphTypeMenu);
    histogramGraphAction = createCheckableAction(tr("Histogram"), graphTypeActionGroup);
    lineGraphAction = createCheckableAction(tr("Line graph"), graphTypeActionGroup);
    areaGraphAction = createCheckableAction(tr("Area graph"), graphTypeActionGroup);

    histogramGraphAction->setObjectName("Histogram");
    lineGraphAction->setObjectName("Line graph");
    areaGraphAction->setObjectName("Area graph");

    switch (graphOverview->getDisplaySettings().type) {
        case MaGraphOverviewDisplaySettings::Histogram:
            histogramGraphAction->setChecked(true);
            break;
        case MaGraphOverviewDisplaySettings::Line:
            lineGraphAction->setChecked(true);
            break;
        default:
            areaGraphAction->setChecked(true);
    }

    graphTypeMenu->addActions(graphTypeActionGroup->actions());
}

void MaOverviewContextMenu::initOrientationSubmenu() {
    orientationMenu = displaySettingsMenu->addMenu(tr("Orientation"));
    orientationMenu->menuAction()->setObjectName("Orientation");

    orientationActionGroup = new QActionGroup(orientationMenu);
    topToBottomOrientationAction = createCheckableAction(tr("Top to bottom"), orientationActionGroup);
    bottomToTopOrientationAction = createCheckableAction(tr("Bottom to top"), orientationActionGroup);
    orientationMenu->addActions(orientationActionGroup->actions());

    topToBottomOrientationAction->setObjectName("Top to bottom");
    bottomToTopOrientationAction->setObjectName("Bottom to top");

    if (graphOverview->getDisplaySettings().orientation == MaGraphOverviewDisplaySettings::FromBottomToTop) {
        bottomToTopOrientationAction->setChecked(true);
    } else {
        topToBottomOrientationAction->setChecked(true);
    }
}

QAction* MaOverviewContextMenu::createCheckableAction(const QString& text, QActionGroup* group) {
    QAction* a = new QAction(text, this);
    a->setCheckable(true);

    if (group != nullptr) {
        group->addAction(a);
    }

    return a;
}

}  // namespace U2
