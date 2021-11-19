/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MSA_EDITOR_OVERVIEW_H_
#define _U2_MSA_EDITOR_OVERVIEW_H_

#include <QAction>

#include "overview/MaEditorOverviewArea.h"

namespace U2 {

class MsaEditorMultilineWgt;
class MaEditorWgt;
class MaSimpleOverview;
class MaGraphOverview;
class MaOverviewContextMenu;

class U2VIEW_EXPORT MSAEditorOverviewArea : public MaEditorOverviewArea {
    Q_OBJECT
public:
    MSAEditorOverviewArea(MsaEditorMultilineWgt *ui);

    void contextMenuEvent(QContextMenuEvent* event) override;

    static const QString OVERVIEW_AREA_OBJECT_NAME;

    void setVisible(bool isVisible) override;

private:
    bool eventFilter(QObject* watched, QEvent* event) override;

    /** Updates fixed-height value of the widget. Recomputes the new height by checking children visibility. */
    void updateFixedHeightGeometry();

    MaGraphOverview* graphOverview = nullptr;
    MaSimpleOverview* simpleOverview = nullptr;
    MaOverviewContextMenu* contextMenu = nullptr;
};

}  // namespace U2

#endif  // _U2_MSA_EDITOR_OVERVIEW_H_
