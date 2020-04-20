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

#ifndef _U2_EXPORT_MCA_2_MSA_DIALOG_H_
#define _U2_EXPORT_MCA_2_MSA_DIALOG_H_

#include <QDialog>

#include "ui_ExportMca2MsaDialog.h"

namespace U2 {

class SaveDocumentController;

class ExportMca2MsaDialog : public QDialog, public Ui_ExportMca2MsaDialog {
    Q_OBJECT
public:
    ExportMca2MsaDialog(const QString &defaultFilePath, QWidget *parent);

    QString getSavePath() const;
    QString getFormatId() const;
    bool getAddToProjectOption() const;
    bool getIncludeReferenceOption() const;

private:
    void initSaveController(const QString &defaultFilePath);

    const QString defaultFilePath;
    SaveDocumentController *saveController;
};

}   // namespace U2

#endif // _U2_EXPORT_MCA_2_MSA_DIALOG_H_
