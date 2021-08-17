/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#ifndef _U2_GENERATED_SEQUENCES_TABLE_H_
#define _U2_GENERATED_SEQUENCES_TABLE_H_

#include <QTableWidget>

namespace U2 {

/**
 * @GeneratedSequencesTable class
 * This class is aimed to visualize a set of pre-generated sequence from the resource "generated_sequnces.txt" file
 */
class GeneratedSequencesTable : public QTableWidget {
public:
    GeneratedSequencesTable(QWidget* parent = nullptr);

    /**
     * Update the sequence list with the corresponding filter
     * Fill the table with sequences from @sequences, which contain @filter
     */
    void updateSequenceList(const QString& filter);

private:
    /**
     * Fill the table with data from the "generated_sequnces.txt" file
     */
    void fillTable();
    /**
     * Fill the table with sequences from @data
     */
    void updateTable(QList<QString> data);

    QList<QString> sequences;

    static constexpr int SEQUENCE_SIZE = 8;
};

}

#endif
