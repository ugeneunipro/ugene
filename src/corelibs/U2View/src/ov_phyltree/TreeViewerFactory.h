/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class MultiGSelection;

class U2VIEW_EXPORT TreeViewerFactory : public GObjectViewFactory {
    Q_OBJECT
public:
    TreeViewerFactory();

    bool canCreateView(const MultiGSelection& multiSelection) override;

    Task* createViewTask(const MultiGSelection& multiSelection, bool single = false) override;

    bool isStateInSelection(const MultiGSelection& multiSelection, const QVariantMap& stateData) override;

    Task* createViewTask(const QString& viewName, const QVariantMap& stateData) override;

    bool supportsSavedStates() const override {
        return true;
    }

    static const GObjectViewFactoryId ID;
};

}  // namespace U2
