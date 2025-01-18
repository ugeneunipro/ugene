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

#include "EnzymeTreeItem.h"


namespace U2 {

EnzymeTreeItem::EnzymeTreeItem(const SEnzymeData& ed)
    : enzyme(ed) {
    setText(Column::Id, enzyme->id);
    setCheckState(Column::Id, Qt::Unchecked);
    setText(Column::Accession, enzyme->accession);
    setText(Column::Type, enzyme->type);
    setData(Column::Type, Qt::ToolTipRole, getTypeInfo());
    setText(Column::Sequence, enzyme->seq);
    setData(Column::Sequence, Qt::ToolTipRole, enzyme->generateEnzymeTooltip());
    setText(Column::Organism, enzyme->organizm);
    setData(Column::Organism, Qt::ToolTipRole, enzyme->organizm);
    setText(5, enzyme->suppliers.join("; "));
    setData(5, Qt::ToolTipRole, enzyme->suppliers.join("\n"));
}

bool EnzymeTreeItem::operator<(const QTreeWidgetItem& other) const {
    int col = treeWidget()->sortColumn();
    const EnzymeTreeItem& ei = (const EnzymeTreeItem&)other;
    if (col == 0) {
        bool eq = enzyme->id == ei.enzyme->id;
        if (!eq) {
            return enzyme->id < ei.enzyme->id;
        }
        return this < &ei;
    }
    return text(col) < ei.text(col);
}

QString EnzymeTreeItem::getEnzymeInfo() const {
    QString result;
    result += QString("<a href=\"http://rebase.neb.com/rebase/enz/%1.html\">%1</a>")
        .arg(text(Column::Id));
    if (enzymesNumber != INCORRECT_ENZYMES_NUMBER) {
        if (enzymesNumber > MAXIMUM_ENZYMES_NUMBER) {
            result += tr(" (>%1 sites)").arg(MAXIMUM_ENZYMES_NUMBER);
        } else {
            result += " (" + tr("%n sites", "", enzymesNumber) + ")";
        }
    }
    auto typeString = data(Column::Type, Qt::ToolTipRole).toString();
    if (!typeString.isEmpty()) {
        auto lower = typeString.front().toLower();
        typeString = typeString.replace(0, 1, lower);
        result += ": " + typeString;
    }
    result += data(Column::Sequence, Qt::ToolTipRole).toString();
    return result;
}

QString EnzymeTreeItem::getTypeInfo() const {
    auto type = text(Column::Type);
    QString result;
    if (type == "M") {
        result = tr("An orphan methylase,<br>not associated with a restriction enzyme or specificity subunit");
    } else if (type.size() == 2) {
        if (type == "IE") {
            result = tr("An intron-encoded (homing) endonuclease");
        } else if (type.startsWith("R")) {
            result = tr("Type %1 restriction enzyme").arg(type.back());
        } else if (type.startsWith("M")) {
            result = tr("Type %1 methylase").arg(type.back());
        }
    } else if (type.size() == 3) {
        if (type.startsWith("R") && type.endsWith("*")) {
            result = tr("Type %1 restriction enzyme,<br>but only recognizes the sequence when it is methylated").arg(type.at(1));
        } else if (type.startsWith("RM")) {
            result = tr("Type %1 enzyme, which acts as both -<br>a restriction enzyme and a methylase").arg(type.back());
        }
    }

    return result;
}

}  // namespace U2
