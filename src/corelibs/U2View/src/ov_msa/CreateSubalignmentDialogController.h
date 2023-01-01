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

#ifndef _U2_CREATE_SUBALIGNIMENT_DIALOG_CONTROLLER_H_
#define _U2_CREATE_SUBALIGNIMENT_DIALOG_CONTROLLER_H_

#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/Task.h>
#include <U2Core/U2Region.h>

#include "ui_CreateSubalignmentDialog.h"

namespace U2 {

class SaveDocumentController;

class U2VIEW_EXPORT CreateSubalignmentDialogController : public QDialog, private Ui_CreateSubalignmentDialog {
    Q_OBJECT
public:
    CreateSubalignmentDialogController(MultipleSequenceAlignmentObject* obj, const QList<qint64>& preSelectedRowIdList, const U2Region& preSelectedColumnsRegion, QWidget* p = nullptr);

    void accept() override;

    /** Returns 'true' if 'addToProject' option was checked in the dialog and the saved sub-alignment must be added to the active project. */
    bool getAddToProjFlag() const;

    /** Returns path to the saved sub-alignment. */
    QString getSavePath() const;

    /** Returns document format of the sub-alignment document. */
    DocumentFormatId getFormatId() const;

    /** Returns selected columns range in the original alignment. */
    const U2Region& getSelectedColumnsRegion() const;

    /** Returns selected row ids in the original alignment. */
    const QList<qint64>& getSelectedRowIds() const;

private slots:
    void sl_allButtonClicked();
    void sl_invertButtonClicked();
    void sl_noneButtonClicked();
    void sl_regionChanged();

private:
    void initSaveController();
    void updateSelectedRowIds();

    MultipleSequenceAlignmentObject* msaObject;
    QList<qint64> selectedRowIds;
    U2Region selectedColumnRegion;
    SaveDocumentController* saveController;
};

class CreateSubalignmentTask;
class CreateSubalignmentSettings;

class U2VIEW_EXPORT CreateSubalignmentAndOpenViewTask : public Task {
    Q_OBJECT
public:
    CreateSubalignmentAndOpenViewTask(MultipleSequenceAlignmentObject* mobj, const CreateSubalignmentSettings& settings);
    QList<Task*> onSubTaskFinished(Task* subTask);

private:
    CreateSubalignmentTask* csTask;
};

}  // namespace U2

#endif
