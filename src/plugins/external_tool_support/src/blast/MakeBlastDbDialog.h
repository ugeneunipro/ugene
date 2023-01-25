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

#ifndef _U2_MAKE_BLAST_DB_DIALOG_H
#define _U2_MAKE_BLAST_DB_DIALOG_H

#include <QDialog>

#include "MakeBlastDbTask.h"

#include <ui_MakeBlastDbDialog.h>

namespace U2 {

class MakeBlastDbDialog : public QDialog, public Ui_MakeBlastDbDialog {
    Q_OBJECT
public:
    MakeBlastDbDialog(QWidget* parent, const MakeBlastDbSettings& settings = {});

    /** Returns the original settings adjusted after the dialog is accepted. */
    const MakeBlastDbSettings& getTaskSettings() const;

private slots:
    void sl_makeBlastDb();
    void sl_lineEditChanged();

    void sl_onBrowseInputFiles();
    void sl_onBrowseInputDir();
    void sl_onBrowseDatabasePath();

private:
    MakeBlastDbSettings settings;
    QPushButton* makeButton;
    QPushButton* cancelButton;
};

}  // namespace U2
#endif  // _U2_MAKE_BLAST_DB_DIALOG_H
