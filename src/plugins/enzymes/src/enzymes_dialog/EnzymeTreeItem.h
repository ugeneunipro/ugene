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

#include <QCoreApplication>
#include <QTreeWidgetItem>

#include <U2Algorithm/EnzymeModel.h>

namespace U2 {

/**
 * @brief This class describes a tree item of a certain enzyme.
 */
class EnzymeTreeItem : public QTreeWidgetItem {
    Q_DECLARE_TR_FUNCTIONS(EnzymeTreeItem)
public:
    EnzymeTreeItem(const SEnzymeData& ed);
    const SEnzymeData enzyme;
    static constexpr int INCORRECT_ENZYMES_NUMBER = -1;
    static constexpr int MAXIMUM_ENZYMES_NUMBER = 10'000;

    // Number of enzymes in the current sequence
    int enzymesNumber = INCORRECT_ENZYMES_NUMBER;
    // True if FindEnzymesTask, which calculates number of enzymes,
    // has already been run
    bool hasNumberCalculationTask = false;
    bool operator<(const QTreeWidgetItem& other) const override;
    // Get text information about this enzyme
    QString getEnzymeInfo() const;

private:
    enum Column {
        Id = 0,
        Accession,
        Type,
        Sequence,
        Organism,
    };

    QString getTypeInfo() const;
};

}  // namespace U2
