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

#include "MsaEditorNameList.h"

#include <QMouseEvent>

#include <U2Gui/GUIUtils.h>

#include "MSAEditor.h"

namespace U2 {

MsaEditorNameList::MsaEditorNameList(MaEditorWgt* ui, QScrollBar* nhBar)
    : MaEditorNameList(ui, nhBar) {
    connect(editor, SIGNAL(si_buildMenu(GObjectView*, QMenu*, const QString&)), SLOT(sl_buildMenu(GObjectView*, QMenu*, const QString&)));
}

void MsaEditorNameList::sl_buildMenu(GObjectView*, QMenu* menu, const QString& menuType) {
    if (menuType == MsaEditorMenuType::CONTEXT || menuType == MsaEditorMenuType::STATIC) {
        buildMenu(menu);
    }
}

void MsaEditorNameList::buildMenu(QMenu *menu) {
    if (editor->getMaEditorMultilineWgt()->getActiveChild() != ui) {
        return;
    }
    QMenu *editMenu = GUIUtils::findSubMenu(menu, MSAE_MENU_EDIT);
    SAFE_POINT(editMenu != nullptr, "editMenu not found", );

    editMenu->insertAction(editMenu->isEmpty() ? nullptr : editMenu->actions().last(), removeSequenceAction);

    CHECK(qobject_cast<MSAEditor*>(editor) != nullptr, );
    CHECK(rect().contains(mapFromGlobal(QCursor::pos())), );

    editMenu->insertAction(editMenu->isEmpty() ? nullptr : editMenu->actions().first(), editSequenceNameAction);
}

MSAEditor* MsaEditorNameList::getEditor() const {
    return qobject_cast<MSAEditor*>(editor);
}

QSize MsaEditorNameList::sizeHint() const
{
    QSize s = QWidget::sizeHint();
    if (editor->getMultilineMode()) {
        return QSize(s.width(), minimumSizeHint().height());
    }
    return s;
}

QSize MsaEditorNameList::minimumSizeHint() const
{
    QSize s = QWidget::minimumSizeHint();
    if (editor->getMultilineMode()) {
        int newHeight = (editor->getSequenceRowHeight() + 1) * qMax(1, editor->getNumSequences());
        return QSize(s.width(), newHeight);
    }
    return s;
}

}  // namespace U2
