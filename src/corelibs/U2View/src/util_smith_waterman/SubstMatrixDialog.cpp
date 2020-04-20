/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#include "SubstMatrixDialog.h"

#include <U2Gui/HelpButton.h>
#include <QPushButton>
#include <QScrollBar>
#include <QHeaderView>
#include <QTableWidgetItem>

namespace U2 {

SubstMatrixDialog::SubstMatrixDialog(const SMatrix& _m, QWidget* p)
: QDialog(p), hlBorderColumn(-1), hlBorderRow(-1), hlInnerColumn(-1), hlInnerRow(-1), m(_m)
{
    assert(!m.isEmpty());
    setupUi(this);

    bttnClose = buttonBox->button(QDialogButtonBox::Close);
    bttnClose->setText(tr("Close"));

    setWindowTitle(tr("Scoring Matrix: %1").arg(m.getName()));
    setModal(true);

    QString info;
    info+="<b>" + tr("min score:")+"</b> " + QString::number(m.getMinScore()) + ", ";
    info+="<b>" + tr("max score:")+"</b> " + QString::number(m.getMaxScore()) + "<br>";
    info+="<pre>" + m.getDescription() + "</pre>";
    infoEdit->setHtml(info);

    connectGUI();
    prepareTable();

}


void SubstMatrixDialog::sl_closeWindow() {
    close();
}

void SubstMatrixDialog::connectGUI() {
    connect(bttnClose, SIGNAL(clicked()), SLOT(sl_closeWindow()));
    connect(tableMatrix, SIGNAL(cellEntered(int, int)), SLOT(sl_mouseOnCell(int, int)));
}

#define  CELL_WIDTH 25
#define  DEFAULT_BORDER_CELL_COLOR QColor(200, 200, 200)
#define  HIGHLIGHT_BORDER_CELL_COLOR QColor(200, 230, 200)
#define  DEFAULT_INNER_CELL_COLOR QColor(255, 255, 255)
#define  HIGHLIGHT_INNER_CELL_COLOR QColor(200, 230, 200)

void SubstMatrixDialog::prepareTable() {
    tableMatrix->horizontalHeader()->setHidden(true);
    tableMatrix->verticalHeader()->setHidden(true);

    QByteArray alphaChars = m.getAlphabet()->getAlphabetChars();
    int n = alphaChars.size();
    tableMatrix->setRowCount(n + 1);
    tableMatrix->setColumnCount(n + 1);

    QTableWidgetItem* ptwi = new QTableWidgetItem("");
    Qt::ItemFlags flags = ptwi->flags();
    flags &= (~Qt::ItemIsEditable);
    ptwi->setFlags(flags);
    tableMatrix->setItem(0, 0, ptwi);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            char ch_i = alphaChars.at(i);
            char ch_j = alphaChars.at(j);
            float score = m.getScore(ch_i, ch_j);
            ptwi = new QTableWidgetItem(QString::number(score));
            ptwi->setBackgroundColor(DEFAULT_INNER_CELL_COLOR);
            ptwi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            ptwi->setFlags(flags);
            tableMatrix->setItem(i + 1, j + 1, ptwi);
        }
    }
    for (int i = 0; i < n; i++) {
        char ch = alphaChars.at(i);
        QString title(ch);

        ptwi = new QTableWidgetItem(title);
        ptwi->setBackgroundColor(DEFAULT_BORDER_CELL_COLOR);
        ptwi->setFlags(flags);
        ptwi->setTextAlignment(Qt::AlignCenter);
        tableMatrix->setItem(i+1, 0, ptwi);

        ptwi = new QTableWidgetItem(title);
        ptwi->setFlags(flags);
        ptwi->setTextAlignment(Qt::AlignCenter);
        ptwi->setBackgroundColor(DEFAULT_BORDER_CELL_COLOR);
        tableMatrix->setItem(0, i+1, ptwi);
    }

    tableMatrix->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableMatrix->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableMatrix->setMinimumSize(CELL_WIDTH * (n + 1) + 20, CELL_WIDTH * (n + 1) + 20); //+20 is for borders
}

void SubstMatrixDialog::sl_mouseOnCell(int row, int column) {
    //update mid-cell
    if (row != 0 && column != 0 && !(column == hlInnerColumn && row == hlInnerRow)) {
        QTableWidgetItem* prevItem = tableMatrix->item(hlInnerRow, hlInnerColumn);
        if (prevItem != NULL) {
            prevItem->setBackgroundColor(DEFAULT_INNER_CELL_COLOR);
        }
        QTableWidgetItem* newItem = tableMatrix->item(row, column);
        if (newItem != NULL) {
            newItem->setBackgroundColor(HIGHLIGHT_INNER_CELL_COLOR);
        }
        hlInnerColumn = column;
        hlInnerRow = row;
    }

    //update row header
    if (row != hlBorderRow && row != 0) {
        QTableWidgetItem* pw = tableMatrix->item(row, 0);
        if (pw!=NULL) {
            pw->setBackgroundColor(HIGHLIGHT_BORDER_CELL_COLOR);
        }
        pw = tableMatrix->item(hlBorderRow, 0);
        if (pw!=NULL) {
            pw->setBackgroundColor(DEFAULT_BORDER_CELL_COLOR);
        }

        hlBorderRow = row;
    }

    //update column header
    if (column != hlBorderColumn && column != 0) {
        QTableWidgetItem* pw = tableMatrix->item(0, column);
        if (pw!=NULL) {
            pw->setBackgroundColor(HIGHLIGHT_BORDER_CELL_COLOR);
        }
        pw = tableMatrix->item(0, hlBorderColumn);
        if (pw != NULL) {
            pw->setBackgroundColor(DEFAULT_BORDER_CELL_COLOR);
        }

        hlBorderColumn = column;
    }
}

} // namespace
