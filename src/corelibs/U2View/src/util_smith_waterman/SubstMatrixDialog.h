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

#include <QDialog>

#include <U2Core/SMatrix.h>
#include <U2Core/global.h>

class Ui_SubstMatrixDialogBase;

namespace U2 {

class U2VIEW_EXPORT SubstMatrixDialog : public QDialog {
    Q_OBJECT
public:
    SubstMatrixDialog(const SMatrix& m, QWidget* parent);

private slots:
    void sl_closeWindow();
    void sl_mouseOnCell(int row, int column);
    void sl_colorThemeSwitched();

private:
    void connectGUI();
    void prepareTable();
    enum class ColorType {
        DefaultBorder,
        HighlightBorder,
        DefaultInner,
        HighlightInner
    };
    const QColor& getCellColor(ColorType type) const;

    int hlBorderColumn = -1;
    int hlBorderRow = -1;

    int hlInnerColumn = -1;
    int hlInnerRow = -1;

    SMatrix m;

    QPushButton* bttnClose = nullptr;
    Ui_SubstMatrixDialogBase* base = nullptr;

    static constexpr int CELL_WIDTH = 25;
    static const QColor DEFAULT_BORDER_CELL_COLOR_LIGHT;
    static const QColor HIGHLIGHT_BORDER_CELL_COLOR_LIGHT;
    static const QColor DEFAULT_INNER_CELL_COLOR_LIGHT;
    static const QColor HIGHLIGHT_INNER_CELL_COLOR_LIGHT;

    static const QColor DEFAULT_BORDER_CELL_COLOR_DARK;
    static const QColor HIGHLIGHT_BORDER_CELL_COLOR_DARK;
    static const QColor DEFAULT_INNER_CELL_COLOR_DARK;
    static const QColor HIGHLIGHT_INNER_CELL_COLOR_DARK;
};

}  // namespace U2
