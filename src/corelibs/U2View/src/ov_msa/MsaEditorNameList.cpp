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

#include "MsaEditorNameList.h"

#include <QMouseEvent>

#include <U2Gui/GUIUtils.h>

#include "MsaEditor.h"

namespace U2 {

MsaEditorNameList::MsaEditorNameList(MaEditorWgt* ui, QScrollBar* nhBar)
    : MaEditorNameList(ui, nhBar) {
    connect(editor, &GObjectViewController::si_buildMenu, this, &MsaEditorNameList::sl_buildMenu);
}

void MsaEditorNameList::sl_buildMenu(GObjectViewController*, QMenu* menu, const QString& menuType) {
    if (menuType == MsaEditorMenuType::CONTEXT || menuType == MsaEditorMenuType::STATIC) {
        buildMenu(menu);
    }
}

void MsaEditorNameList::buildMenu(QMenu* menu) {
    if (editor->getMainWidget()->getActiveChild() != ui) {
        return;
    }
    QMenu* editMenu = GUIUtils::findSubMenu(menu, MSAE_MENU_EDIT);
    SAFE_POINT(editMenu != nullptr, "editMenu not found", );

    editMenu->insertAction(editMenu->isEmpty() ? nullptr : editMenu->actions().last(), removeSequenceAction);

    CHECK(qobject_cast<MsaEditor*>(editor) != nullptr, );
    CHECK(rect().contains(mapFromGlobal(QCursor::pos())), );

    editMenu->insertAction(editMenu->isEmpty() ? nullptr : editMenu->actions().first(), editSequenceNameAction);
}

MsaEditor* MsaEditorNameList::getEditor() const {
    return qobject_cast<MsaEditor*>(editor);
}

QSize MsaEditorNameList::sizeHint() const {
    QSize s = QWidget::sizeHint();
    if (editor->isMultilineMode()) {
        return QSize(s.width(), minimumSizeHint().height());
    }
    return s;
}

QSize MsaEditorNameList::minimumSizeHint() const {
    QSize s = QWidget::minimumSizeHint();
    if (editor->isMultilineMode()) {
        int viewRowCount = editor->getCollapseModel()->getViewRowCount();
        int numSequences = editor->getNumSequences();
        int newHeight = (editor->getRowHeight() + 0) *
                        (qMax(1, qMin(viewRowCount, numSequences)) + 1);
        return QSize(s.width(), newHeight);
    }
    return s;
}

}  // namespace U2
