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

#ifndef _U2_ALIGN_TO_REFERENCE_BLAST_DIALOG_H_
#define _U2_ALIGN_TO_REFERENCE_BLAST_DIALOG_H_

#include <QDialog>
#include <QTemporaryFile>

#include <U2Core/Task.h>

#include <U2Gui/U2SavableWidget.h>

#include "ui_AlignToReferenceBlastDialog.h"

namespace U2 {

class CmdlineInOutTaskRunner;
class SaveDocumentController;
class LoadDocumentTask;

class AlignToReferenceBlastCmdlineTask : public Task {
    Q_OBJECT
public:
    class Settings {
    public:
        enum RowNaming {
            SequenceName,
            FileName
        };

        Settings() = default;

        QString getRowNamingPolicyString() const;

        QString referenceUrl;
        QStringList readUrls;
        int minIdentity = 60;
        int minLength = 0;
        int qualityThreshold = 30;
        RowNaming rowNaming = SequenceName;
        QString resultAlignmentFile;
        bool addResultToProject = true;
    };

    AlignToReferenceBlastCmdlineTask(const Settings& settings);

private:
    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    void run() override;
    ReportResult report() override;
    QString generateReport() const override;

    Settings settings;
    CmdlineInOutTaskRunner* cmdlineTask = nullptr;
    LoadDocumentTask* loadRef = nullptr;

    QTemporaryFile reportFile;
    QString reportString;

    static const QString ALIGN_TO_REF_CMDLINE;
    static const QString TRIM_ARG;
    static const QString MIN_LEN_ARG;
    static const QString THRESHOLD_ARG;
    static const QString READS_ARG;
    static const QString MIN_IDENTITY_ARG;
    static const QString REF_ARG;
    static const QString RESULT_ALIGNMENT_ARG;
    static const QString ROW_NAMING_ARG;
};

class AlignToReferenceBlastDialog : public QDialog, public Ui_AlignToReferenceBlastDialog {
    Q_OBJECT
public:
    AlignToReferenceBlastDialog(QWidget* parent);
    AlignToReferenceBlastCmdlineTask::Settings getSettings() const;

public slots:
    void accept();

private slots:
    void sl_setReference();
    void sl_addRead();
    void sl_removeRead();
    void sl_referenceChanged(const QString&);

private:
    void initSaveController();
    void connectSlots();

    SaveDocumentController* saveController;

    AlignToReferenceBlastCmdlineTask::Settings settings;
    U2SavableWidget savableWidget;
    QString defaultOutputUrl;
};

}  // namespace U2

#endif  // _U2_ALIGN_TO_REFERENCE_BLAST_DIALOG_H_
