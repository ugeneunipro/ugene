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

#ifndef _U2_CREATE_DOCUMENT_FROM_TEXT_DIALOG_CONTROLLER_H_
#define _U2_CREATE_DOCUMENT_FROM_TEXT_DIALOG_CONTROLLER_H_

#include <QDialog>

#include <U2Core/DocumentModel.h>
#include <U2Core/global.h>

#include <U2Gui/DialogUtils.h>

#include "SeqPasterWidgetController.h"

class Ui_CreateDocumentFromTextDialog;

namespace U2 {

class SaveDocumentController;

class U2GUI_EXPORT CreateDocumentFromTextDialogController : public QDialog {
    Q_OBJECT
public:
    CreateDocumentFromTextDialogController(QWidget* p = NULL);
    ~CreateDocumentFromTextDialogController();

    void accept();

private slots:
    void sl_filepathTextChanged();
    
private:
    void addSeqPasterWidget();
    QList<DNASequence> prepareSequences() const;
    void initSaveController();
    
    SeqPasterWidgetController *w;
    SaveDocumentController *saveController;
    QString filter;
    Ui_CreateDocumentFromTextDialog* ui;
};

}//ns

#endif
