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

#include "SubstMatrixDialog.h"

#include <QHeaderView>
#include <QPalette>
#include <QPushButton>
#include <QScrollBar>

#include <U2Core/AppContext.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/MainWindow.h>

#include "ui_SubstMatrixDialogBase.h"

namespace U2 {

SubstMatrixDialog::SubstMatrixDialog(const SMatrix& _m, QWidget* p)
    : QDialog(p), m(_m), base(new Ui_SubstMatrixDialogBase()) {
    assert(!m.isEmpty());
    base->setupUi(this);

    bttnClose = base->buttonBox->button(QDialogButtonBox::Close);
    bttnClose->setText(tr("Close"));

    setWindowTitle(tr("Scoring Matrix: %1").arg(m.getName()));
    setModal(true);

    QString info;
    info += "<b>" + tr("min score:") + "</b> " + QString::number(m.getMinScore()) + ", ";
    info += "<b>" + tr("max score:") + "</b> " + QString::number(m.getMaxScore()) + "<br>";
    info += "<pre>" + m.getDescription() + "</pre>";
    base->infoEdit->setHtml(info);

    connectGUI();
    prepareTable();
}

void SubstMatrixDialog::sl_closeWindow() {
    close();
}

void SubstMatrixDialog::connectGUI() {
    connect(bttnClose, SIGNAL(clicked()), SLOT(sl_closeWindow()));
    connect(base->tableMatrix, SIGNAL(cellEntered(int, int)), SLOT(sl_mouseOnCell(int, int)));
    connect(AppContext::getMainWindow(), &MainWindow::si_colorThemeSwitched, this, &SubstMatrixDialog::sl_colorThemeSwitched);
}

const QColor SubstMatrixDialog::DEFAULT_BORDER_CELL_COLOR_LIGHT = QColor(200, 200, 200);
const QColor SubstMatrixDialog::HIGHLIGHT_BORDER_CELL_COLOR_LIGHT = QColor(200, 230, 200);
const QColor SubstMatrixDialog::DEFAULT_INNER_CELL_COLOR_LIGHT = QColor(255, 255, 255);
const QColor SubstMatrixDialog::HIGHLIGHT_INNER_CELL_COLOR_LIGHT = QColor(200, 230, 200);

const QColor SubstMatrixDialog::DEFAULT_BORDER_CELL_COLOR_DARK = QColor(95, 95, 95);
const QColor SubstMatrixDialog::HIGHLIGHT_BORDER_CELL_COLOR_DARK = QColor(80, 175, 80);
const QColor SubstMatrixDialog::DEFAULT_INNER_CELL_COLOR_DARK = QColor(48, 48, 48);
const QColor SubstMatrixDialog::HIGHLIGHT_INNER_CELL_COLOR_DARK = QColor(80, 175, 80);

void SubstMatrixDialog::prepareTable() {
    base->tableMatrix->horizontalHeader()->setHidden(true);
    base->tableMatrix->verticalHeader()->setHidden(true);

    QByteArray alphaChars = m.getAlphabet()->getAlphabetChars();
    int n = alphaChars.size();
    base->tableMatrix->setRowCount(n + 1);
    base->tableMatrix->setColumnCount(n + 1);

    auto ptwi = new QTableWidgetItem("");
    Qt::ItemFlags flags = ptwi->flags();
    flags &= (~Qt::ItemIsEditable);
    ptwi->setFlags(flags);
    base->tableMatrix->setItem(0, 0, ptwi);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            char ch_i = alphaChars.at(i);
            char ch_j = alphaChars.at(j);
            float score = m.getScore(ch_i, ch_j);
            ptwi = new QTableWidgetItem(QString::number(score));
            ptwi->setBackground(getCellColor(ColorType::DefaultInner));
            ptwi->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            ptwi->setFlags(flags);
            base->tableMatrix->setItem(i + 1, j + 1, ptwi);
        }
    }
    for (int i = 0; i < n; i++) {
        char ch = alphaChars.at(i);
        QString title(ch);

        ptwi = new QTableWidgetItem(title);
        ptwi->setBackground(getCellColor(ColorType::DefaultBorder));
        ptwi->setFlags(flags);
        ptwi->setTextAlignment(Qt::AlignCenter);
        base->tableMatrix->setItem(i + 1, 0, ptwi);

        ptwi = new QTableWidgetItem(title);
        ptwi->setFlags(flags);
        ptwi->setTextAlignment(Qt::AlignCenter);
        ptwi->setBackground(getCellColor(ColorType::DefaultBorder));
        base->tableMatrix->setItem(0, i + 1, ptwi);
    }

    base->tableMatrix->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    base->tableMatrix->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    base->tableMatrix->setMinimumSize(CELL_WIDTH * (n + 1) + 20, CELL_WIDTH * (n + 1) + 20);  //+20 is for borders
}

const QColor& SubstMatrixDialog::getCellColor(ColorType type) const {
    bool isDark = AppContext::getMainWindow()->isDarkTheme();
    switch (type) {
        case ColorType::DefaultBorder:
            return isDark ? DEFAULT_BORDER_CELL_COLOR_DARK : DEFAULT_BORDER_CELL_COLOR_LIGHT;
            break;
        case ColorType::HighlightBorder:
            return isDark ? HIGHLIGHT_BORDER_CELL_COLOR_DARK : HIGHLIGHT_BORDER_CELL_COLOR_LIGHT;
            break;
        case ColorType::DefaultInner:
            return isDark ? DEFAULT_INNER_CELL_COLOR_DARK : DEFAULT_INNER_CELL_COLOR_LIGHT;
            break;
        case ColorType::HighlightInner:
            return isDark ? HIGHLIGHT_INNER_CELL_COLOR_DARK : HIGHLIGHT_INNER_CELL_COLOR_LIGHT;
            break;
        default:
            FAIL("Unexpected type", QColor());
            return QColor();
    }
}

void SubstMatrixDialog::sl_mouseOnCell(int row, int column) {
    // update mid-cell
    if (row != 0 && column != 0 && !(column == hlInnerColumn && row == hlInnerRow)) {
        QTableWidgetItem* prevItem = base->tableMatrix->item(hlInnerRow, hlInnerColumn);
        if (prevItem != nullptr) {
            prevItem->setBackground(getCellColor(ColorType::DefaultInner));
        }
        QTableWidgetItem* newItem = base->tableMatrix->item(row, column);
        if (newItem != nullptr) {
            newItem->setBackground(getCellColor(ColorType::HighlightInner));
        }
        hlInnerColumn = column;
        hlInnerRow = row;
    }

    // update row header
    if (row != hlBorderRow && row != 0) {
        QTableWidgetItem* pw = base->tableMatrix->item(row, 0);
        if (pw != nullptr) {
            pw->setBackground(getCellColor(ColorType::HighlightBorder));
        }
        pw = base->tableMatrix->item(hlBorderRow, 0);
        if (pw != nullptr) {
            pw->setBackground(getCellColor(ColorType::DefaultBorder));
        }

        hlBorderRow = row;
    }

    // update column header
    if (column != hlBorderColumn && column != 0) {
        QTableWidgetItem* pw = base->tableMatrix->item(0, column);
        if (pw != nullptr) {
            pw->setBackground(getCellColor(ColorType::HighlightBorder));
        }
        pw = base->tableMatrix->item(0, hlBorderColumn);
        if (pw != nullptr) {
            pw->setBackground(getCellColor(ColorType::DefaultBorder));
        }

        hlBorderColumn = column;
    }
}

void SubstMatrixDialog::sl_colorThemeSwitched() {
    QByteArray alphaChars = m.getAlphabet()->getAlphabetChars();
    int n = alphaChars.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            auto item = base->tableMatrix->item(i + 1, j + 1);
            item->setBackground(getCellColor(ColorType::DefaultInner));
        }
    }
    for (int i = 0; i < n; i++) {
        auto item = base->tableMatrix->item(i + 1, 0);
        item->setBackground(getCellColor(ColorType::DefaultBorder));
        item = base->tableMatrix->item(0, i + 1);
        item->setBackground(getCellColor(ColorType::DefaultBorder));
    }
}

}  // namespace U2
