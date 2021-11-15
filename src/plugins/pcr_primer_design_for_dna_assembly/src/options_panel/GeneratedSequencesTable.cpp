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

#include "GeneratedSequencesTable.h"

#include <U2Core/L10n.h>
#include <U2Core/U2SafePoints.h>

#include <QFile>

namespace U2 {

GeneratedSequencesTable::GeneratedSequencesTable(QWidget* parent)
    : QTableWidget(parent) {

    fillTable();
}

void GeneratedSequencesTable::updateSequenceList(const QString& filter) {
    clearContents();
    QList<QString> filtred;
    for (const auto &sequence : qAsConst(sequences)) {
        CHECK_CONTINUE(sequence.contains(filter));

        filtred << sequence;
    }

    updateTable(filtred);
}

void GeneratedSequencesTable::fillTable() {
    QFile f(":/pcr_primer_design_for_dna_assembly/txt/generated_sequnces.txt");
    bool opened = f.open(QIODevice::ReadOnly);
    SAFE_POINT(opened, L10N::errorOpeningFileRead(GUrl(f.fileName())), );

    while (!f.atEnd()) {
        QString sequence = f.readLine().simplified();
        SAFE_POINT(sequence.size() == SEQUENCE_SIZE, L10N::internalError("Unexpected sequence size"), );

        sequences << sequence;
    }

    setColumnCount(2);
    updateTable(sequences);
}

void GeneratedSequencesTable::updateTable(QList<QString> data) {
    int size = data.size();
    setRowCount(size);
    for (int i = 0; i < size; i++) {
        auto item = new QTableWidgetItem(data[i]);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        setItem(i, 0, item);
    }
}

}
