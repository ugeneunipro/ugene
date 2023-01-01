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

#include "AlignMsaAction.h"

#include <U2Core/U2SafePoints.h>

#include <U2View/MSAEditor.h>

namespace U2 {

AlignMsaAction::AlignMsaAction(QObject* parent, const QString& toolId, MSAEditor* _msaEditor, const QString& text, int order)
    : ExternalToolSupportAction(parent, _msaEditor, text, order, QStringList(toolId)), msaEditor(_msaEditor) {
    sl_updateState();

    MultipleSequenceAlignmentObject* msaObject = msaEditor->getMaObject();
    connect(msaObject, SIGNAL(si_lockedStateChanged()), SLOT(sl_updateState()));
    connect(msaObject, SIGNAL(si_alignmentBecomesEmpty(bool)), SLOT(sl_updateState()));
}

MSAEditor* AlignMsaAction::getMsaEditor() const {
    return msaEditor;
}

void AlignMsaAction::sl_updateState() {
    auto* msaObject = msaEditor->getMaObject();
    setEnabled(!msaObject->isStateLocked() && !msaEditor->isAlignmentEmpty());
}

}  // namespace U2
