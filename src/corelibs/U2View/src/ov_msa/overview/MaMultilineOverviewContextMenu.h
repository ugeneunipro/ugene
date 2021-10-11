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

#ifndef _U2_MSA_MULTILINE_OVERVIEW_CONTEXT_MENU_H_
#define _U2_MSA_MULTILINE_OVERVIEW_CONTEXT_MENU_H_

#include <QMenu>

#include "MaGraphMultilineOverview.h"

namespace U2 {

class MaSimpleMultilineOverview;
class MSAEditorMultilineOverviewArea;

class MaMultilineOverviewContextMenu : public QMenu {
    Q_OBJECT
public:
    MaMultilineOverviewContextMenu(QWidget *parent, MaSimpleMultilineOverview *so, MaGraphMultilineOverview *go);

private:
    void connectSlots();

signals:
    void si_graphTypeSelected(MaGraphMultilineOverviewDisplaySettings::GraphType type);
    void si_graphOrientationSelected(MaGraphMultilineOverviewDisplaySettings::OrientationMode orientation);
    void si_colorSelected(QColor color);
    void si_calculationMethodSelected(MaGraphMultilineCalculationMethod method);

public slots:
    void sl_exportAsImageTriggered();
    void sl_graphTypeActionTriggered(QAction *);
    void sl_graphOrientationActionTriggered(QAction *);
    void sl_colorActionTriggered();
    void sl_caclulationMethodActionTriggered(QAction *);

private:
    void initSimpleOverviewAction();
    void initExportAsImageAction();
    void initDisplaySettingsMenu();
    void initCalculationMethodMenu();

    void initGraphTypeSubmenu();
    void initOrientationSubmenu();

    QAction *createCheckableAction(const QString &text, QActionGroup *group = nullptr);

    MaSimpleMultilineOverview *simpleOverview;
    MaGraphMultilineOverview *graphOverview;

    QAction *showSimpleOverviewAction;
    QAction *exportAsImageAction;

    QMenu *displaySettingsMenu;
    QMenu *graphTypeMenu;

    QActionGroup *graphTypeActionGroup;
    QAction *histogramGraphAction;
    QAction *lineGraphAction;
    QAction *areaGraphAction;

    QMenu *orientationMenu;

    QActionGroup *orientationActionGroup;
    QAction *topToBottomOrientationAction;
    QAction *bottomToTopOrientationAction;

    QAction *colorAction;

    QMenu *calculationMethodMenu;

    QActionGroup *calculationMethodActionGroup;
    QAction *strictMethodAction;
    QAction *gapMethodAction;
    QAction *clustalMethodAction;
    QAction *highlightingMethodAction;
};

}  // namespace U2

#endif  // _U2_MSA_MULTILINE_OVERVIEW_CONTEXT_MENU_H_
