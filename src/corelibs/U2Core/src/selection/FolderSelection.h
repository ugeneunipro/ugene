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

#include <U2Core/Folder.h>
#include <U2Core/SelectionModel.h>

namespace U2 {

class U2CORE_EXPORT FolderSelection : public GSelection {
public:
    FolderSelection(QObject* p = nullptr);

    bool isEmpty() const override;
    void clear() override;

    const QList<Folder>& getSelection() const;
    void setSelection(const QList<Folder>& folders);

private:
    QList<Folder> selection;
};

}  // namespace U2
