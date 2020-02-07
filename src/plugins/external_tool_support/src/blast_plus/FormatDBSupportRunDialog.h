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

#ifndef _U2_FORMATDB_SUPPORT_RUN_DIALOG_H
#define _U2_FORMATDB_SUPPORT_RUN_DIALOG_H

#include <QDialog>
#include <U2Gui/DialogUtils.h>
#include <ui_FormatDBSupportRunDialog.h>
#include "FormatDBSupportTask.h"

namespace U2 {

class FormatDBSupportRunDialog : public QDialog, public Ui_FormatDBSupportRunDialog {
    Q_OBJECT
public:
    FormatDBSupportRunDialog(const QString& name, FormatDBSupportTaskSettings& settings, QWidget* parent);
private slots:
    void sl_formatDB();
    void sl_lineEditChanged();

    void sl_onBrowseInputFiles();
    void sl_onBrowseInputDir();
    void sl_onBrowseDatabasePath();
private:
    QString                         name;//name of tool that used for formating database
    FormatDBSupportTaskSettings&    settings;
    QPushButton*                    formatButton;
    QPushButton*                    cancelButton;
};

}//namespace
#endif // _U2_FORMATDB_SUPPORT_RUN_DIALOG_H
