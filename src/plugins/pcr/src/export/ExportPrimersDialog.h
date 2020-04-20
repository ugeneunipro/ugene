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

#ifndef _U2_EXPORT_PRIMERS_DIALOG_H_
#define _U2_EXPORT_PRIMERS_DIALOG_H_

#include "Primer.h"
#include "ui_ExportPrimersDialog.h"

namespace U2 {

class Document;
class SaveDocumentController;

class ExportPrimersDialog : public QDialog, private Ui_ExportPrimersDialog {
    Q_OBJECT
public:
    ExportPrimersDialog(const QList<Primer> &primers, QWidget *parent);

private slots:
    void sl_updateState();
    void sl_connect();
    void sl_connectionCompleted();
    void sl_documentAdded(Document *document);
    void sl_documentRemoved(Document *document);
    void sl_folderBrowse();
    void accept();

private:
    void init();
    void initDatabases();
    void initSaveController();
    void connectSignals();
    void connectProjectSignals();
    bool isFileMode() const;

    QList<Primer> primers;
    SaveDocumentController *saveController;

    static const QString LOCAL_FILE;
    static const QString SHARED_DB;
};

}   // namespace U2

#endif // _U2_EXPORT_PRIMERS_DIALOG_H_
