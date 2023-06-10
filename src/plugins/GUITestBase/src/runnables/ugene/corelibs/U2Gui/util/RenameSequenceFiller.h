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

#pragma once

#include "utils/GTUtilsDialog.h"

namespace U2 {
using namespace HI;

class RenameSequenceFiller : public Filler {
public:
    // class can be used for checking name if newName == oldName
    RenameSequenceFiller(QString _newName, QString _oldName = QString(), bool _useCopyPaste = false)
        : Filler(""), newName(_newName), oldName(_oldName), useCopyPaste(_useCopyPaste) {
    }
    void commonScenario() override;

private:
    QString newName;
    QString oldName;
    bool useCopyPaste = false;
};

}  // namespace U2
