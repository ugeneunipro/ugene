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

#ifndef _U2_EXPORT_SEQUENCES_DIALOG_H_
#define _U2_EXPORT_SEQUENCES_DIALOG_H_

#include <QDialog>

#include <U2Core/global.h>

#include <ui_ExportSequencesDialog.h>

namespace U2 {

class SaveDocumentController;

class ExportSequencesDialog : public QDialog, Ui_ExportSequencesDialog {
    Q_OBJECT
public:
    ExportSequencesDialog(bool multiMode, bool allowComplement, bool allowTranslation,
                          bool allowBackTranslation, const QString &defaultFileName,
                          const QString &sourceFileBaseName,
                          const DocumentFormatId &f, QWidget *p);

    void updateModel();
    bool multiMode;
    DocumentFormatId formatId;
    QString file;
    QString sequenceName;

    TriState strand;//Yes-> direct, No->complement, Unkn -> both
    bool translate;
    bool translateAllFrames;
    bool useSpecificTable;
    QString translationTable;

    bool backTranslate;
    bool mostProbable;

    bool merge;
    bool addToProject;
    int mergeGap;

    bool withAnnotations;

    void disableAllFramesOption(bool v);
    void disableStrandOption(bool v);
    void disableAnnotationsOption(bool v);

private slots:
    void sl_exportClicked();
    void sl_translationTableEnabler();
    void sl_formatChanged(const QString &newFormatId);

private:
    void initSaveController(const DocumentFormatId &defaultFormatId);

    SaveDocumentController* saveController;
    const QString defaultFileName;
    QList<QString> tableID;
};

}//namespace

#endif
