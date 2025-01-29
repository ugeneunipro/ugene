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

#pragma once

#include <QTreeWidgetItem>

namespace U2 {

class EnzymeTreeItem;

/**
 * @brief This class describes a group of enzymes in the enzyme dialog.
 * Each group contains enzymes, which names starts with some certain letter.
 */
class EnzymeGroupTreeItem : public QTreeWidgetItem {
public:
    /**
     * @brief Constructor.
     * @s Enzyme group name.
     */
    EnzymeGroupTreeItem(const QString& s);

    /**
     * @brief Update group and child enzymes.
     */
    void updateVisual();

    QString s;
    QSet<EnzymeTreeItem*> checkedEnzymes;
    bool operator<(const QTreeWidgetItem& other) const override;
};


}  // namespace U2
