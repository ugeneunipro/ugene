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

#ifndef _U2_EXPORT_SEQ2MSA_DIALOG_H_
#define _U2_EXPORT_SEQ2MSA_DIALOG_H_

#include <QDialog>

#include <U2Core/global.h>

#include <ui_ExportSequences2MSADialog.h>

namespace U2 {

class SaveDocumentController;

class ExportSequences2MSADialog : public QDialog, private Ui_ExportSequences2MSADialog {
    Q_OBJECT
public:
    ExportSequences2MSADialog(QWidget* p, const QString& defaultUrl = QString() );

    void setOkButtonText(const QString& text) const;
    void setFileLabelText(const QString& text) const;

    virtual void accept();

public:
    QString url;
    DocumentFormatId format;
    bool    addToProjectFlag;
    bool    useGenbankHeader;

private:
    void initSaveController(const QString &defaultUrl);

    SaveDocumentController* saveController;
    QPushButton* okButton;
};

}//namespace

#endif
