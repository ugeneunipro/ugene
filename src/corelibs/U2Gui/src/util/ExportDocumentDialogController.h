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

#ifndef _U2_EXPORT_DOCUMENT_DIALOG_CONTROLLER_H_
#define _U2_EXPORT_DOCUMENT_DIALOG_CONTROLLER_H_

#include <QDialog>

#include <U2Core/global.h>
#include <U2Core/DocumentModel.h>

class Ui_ExportDocumentDialog;

namespace U2 {

class SaveDocumentController;

class U2GUI_EXPORT ExportDocumentDialogController : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(ExportDocumentDialogController)
public:
    ExportDocumentDialogController(Document* d, QWidget* p);
    ExportDocumentDialogController(GObject *object, QWidget *parent, const QString &initUrl = "");
    ~ExportDocumentDialogController();

    QString getDocumentURL() const;
    
    bool getAddToProjectFlag() const;
    void setAddToProjectFlag(bool checked);

    DocumentFormatId getDocumentFormatId() const;

    Document *getSourceDoc() const;
    GObject *getSourceObject() const;

private:
    void initSaveController(const QList<GObject *> &objects, const QString &fileUrl);
    static DocumentFormatConstraints getAcceptableConstraints(const QList<GObject *> &objects);

    SaveDocumentController* saveController;
    Ui_ExportDocumentDialog* ui;
    // the following fields are used to determine whether the object was used for
    // creating a Document from single GObeject or another Document
    Document *sourceDoc;
    GObject *sourceObject;
};

} // _U2_EXPORT_DOCUMENT_DIALOG_CONTROLLER_H_

#endif
