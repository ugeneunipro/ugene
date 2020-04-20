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

#ifndef _U2_BLAST_DB_CMD_DIALOG_H
#define _U2_BLAST_DB_CMD_DIALOG_H

#include <QDialog>

#include <U2Gui/DialogUtils.h>
#include <ui_BlastDBCmdDialog.h>
#include "BlastDBCmdSupportTask.h"
#include "utils/BlastDBSelectorWidgetController.h"

namespace U2 {

class SaveDocumentController;

class BlastDBCmdDialog : public QDialog, public Ui_BlastDBCmdDialog {
    Q_OBJECT
public:
    BlastDBCmdDialog( BlastDBCmdSupportTaskSettings& settings, QWidget* parent);
    void setQueryId(const QString& queryId);

private slots:
    void accept();
    void sl_update();

private:
    void initSaveController();

    BlastDBSelectorWidgetController *dbSelector;
    SaveDocumentController *saveController;
    BlastDBCmdSupportTaskSettings&    settings;
    QPushButton* fetchButton;
};

}//namespace
#endif // _U2_BLAST_DB_CMD_DIALOG_H
