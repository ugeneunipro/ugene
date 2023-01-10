/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MSA_EDITOR_SORT_SEQUENCES_WIDGET_H_
#define _U2_MSA_EDITOR_SORT_SEQUENCES_WIDGET_H_

#include <QComboBox>
#include <QPushButton>
#include <QWidget>

namespace U2 {

class MSAEditor;

class MsaEditorSortSequencesWidget : public QWidget {
    Q_OBJECT;

public:
    MsaEditorSortSequencesWidget(QWidget* parent, MSAEditor* msaEditor);

private slots:
    void sl_sortClicked();
    void sl_msaObjectStateChanged();

private:
    MSAEditor* msaEditor;
    QComboBox* sortByCombo;
    QComboBox* sortOrderCombo;
    QPushButton* sortButton;
};

}  // namespace U2

#endif
