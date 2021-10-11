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

#ifndef _U2_MSA_EDITOR_MULTILINE_OVERVIEW_H_
#define _U2_MSA_EDITOR_MULTILINE_OVERVIEW_H_

#include <QAction>
#include <QWidget>

#include <U2Core/global.h>

#include "overview/MaEditorMultilineOverviewArea.h"

namespace U2 {

class MaEditorMultilineWgt;
class MaSimpleMultilineOverview;
class MaGraphMultilineOverview;
class MaMultilineOverviewContextMenu;

class U2VIEW_EXPORT MSAEditorMultilineOverviewArea : public MaEditorMultilineOverviewArea {
    Q_OBJECT
public:
    MSAEditorMultilineOverviewArea(MaEditorMultilineWgt *ui);

    void contextMenuEvent(QContextMenuEvent *event) override;

    void cancelRendering() override;

    static const QString OVERVIEW_AREA_OBJECT_NAME;

private slots:
    void sl_show() override;

private:
    MaGraphMultilineOverview *graphOverview = nullptr;
    MaSimpleMultilineOverview *simpleOverview = nullptr;
    MaMultilineOverviewContextMenu *contextMenu = nullptr;
};

}  // namespace U2

#endif  // _U2_MSA_EDITOR_MULTILINE_OVERVIEW_H_
