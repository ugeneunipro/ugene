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

#pragma once

#include <QDialog>

#include <U2Core/global.h>

#include <ui_ExportMSA2SequencesDialog.h>

namespace U2 {

class MultipleSequenceAlignmentObject;
class SaveDocumentController;

class ExportMSA2SequencesDialog : public QDialog, private Ui_ExportMSA2SequencesDialog {
    Q_OBJECT
public:
    ExportMSA2SequencesDialog(const QString& defaultDir, const QString& defaultFilename, QWidget* p);

    void accept() override;

    /** Shows dialog and if user accepts it start the export task. */
    static void showDialogAndStartExportTask(MultipleSequenceAlignmentObject* msaObject);

    QString url;
    QString defaultDir;
    QString defaultFileName;
    DocumentFormatId format;
    bool trimGapsFlag = false;
    bool addToProjectFlag = true;

private:
    void initSaveController();

    SaveDocumentController* saveController;
};

}  // namespace U2
