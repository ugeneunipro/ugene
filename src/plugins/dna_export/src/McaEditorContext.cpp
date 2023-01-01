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

#include "McaEditorContext.h"

#include <U2Core/U2SafePoints.h>

#include <U2Gui/GUIUtils.h>

#include <U2View/MaEditorFactory.h>
#include <U2View/McaEditor.h>

#include "ExportUtils.h"

namespace U2 {

McaEditorContext::McaEditorContext(QObject* parent)
    : GObjectViewWindowContext(parent, McaEditorFactory::ID) {
}

void McaEditorContext::sl_exportMca2Msa() {
    GObjectViewAction* action = qobject_cast<GObjectViewAction*>(sender());
    SAFE_POINT(nullptr != action, "action is NULL", );
    McaEditor* mcaEditor = qobject_cast<McaEditor*>(action->getObjectView());
    SAFE_POINT(nullptr != mcaEditor, "Mca Editor is NULL", );

    MultipleChromatogramAlignmentObject* mcaObject = mcaEditor->getMaObject();
    ExportUtils::launchExportMca2MsaTask(mcaObject);
}

void McaEditorContext::initViewContext(GObjectView* view) {
    McaEditor* mcaEditor = qobject_cast<McaEditor*>(view);
    SAFE_POINT(nullptr != mcaEditor, "Mca Editor is NULL", );
    CHECK(nullptr != mcaEditor->getMaObject(), );

    GObjectViewAction* action = new GObjectViewAction(this, view, tr("Export alignment without chromatograms..."));
    connect(action, SIGNAL(triggered()), SLOT(sl_exportMca2Msa()));
    addViewAction(action);
}

void McaEditorContext::buildStaticOrContextMenu(GObjectView* view, QMenu* menu) {
    McaEditor* mcaEditor = qobject_cast<McaEditor*>(view);
    SAFE_POINT(nullptr != mcaEditor, "Mca Editor is NULL", );
    SAFE_POINT(nullptr != menu, "Menu is NULL", );
    CHECK(nullptr != mcaEditor->getMaObject(), );

    QList<GObjectViewAction*> list = getViewActions(view);
    SAFE_POINT(1 == list.size(), "List size is incorrect", );
    QMenu* alignmentMenu = GUIUtils::findSubMenu(menu, MCAE_MENU_ALIGNMENT);
    SAFE_POINT(alignmentMenu != nullptr, "menu 'Alignment' is NULL", );
    alignmentMenu->addAction(list.first());
}

}  // namespace U2
