/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <QWidget>

#include "ui_InsertEnzymeWidget.h"

#include <U2Core/U2AlphabetUtils.h>

namespace U2 {

/**
 * This widget contains restriction sites combobox, where you need to choose one.
 * The choosen site will be drawn schematically.
 */
class InsertEnzymeWidget : public QWidget, private Ui_InsertEnzymeWidget {
    Q_OBJECT
public:
    InsertEnzymeWidget(QWidget* parent, const DNAAlphabet* alphabet);

    QString getEnzymeSequence() const;

private:
    void updateEnzymesList(bool showEnzymesWithUndefinedSuppliers);

    const DNAAlphabet* alphabet = nullptr;
    QSet<QString> items;
};

}
