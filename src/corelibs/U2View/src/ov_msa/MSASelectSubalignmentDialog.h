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

#ifndef _U2_MSA_SELECT_SUBALIGNMENT_DIALOG_H_
#define _U2_MSA_SELECT_SUBALIGNMENT_DIALOG_H_

#include "ui_SelectSubalignmentDialog.h"

#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2Region.h>

namespace U2 {

class MaEditor;

class SelectSubalignmentDialog : public QDialog, Ui_SelectSubalignmentDialog {
    Q_OBJECT
public:
    SelectSubalignmentDialog( MaEditor *editor, const U2Region& region = U2Region(), const QList<int>& selectedIndexes = QList<int>(), QWidget *p = NULL);

    void accept();

    const U2Region getRegion() const { return window; }

    const QStringList& getSelectedSeqNames() const { return selectedNames; }
    const QList<int>& getSelectedSeqIndexes() const { return selectedIndexes; }

public slots:
    void sl_allButtonClicked();
    void sl_noneButtonClicked();
    void sl_invertButtonClicked();

private:
    void init();

    MaEditor* editor;

    U2Region window;
    QStringList selectedNames;
    QList<int> selectedIndexes;
};

} // namespace

#endif // _U2_MSA_SELECT_SUBALIGNMENT_DIALOG_H_
