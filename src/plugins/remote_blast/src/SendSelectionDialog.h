/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <QMessageBox>

#include <U2Core/DNASequenceObject.h>

#include <U2Gui/CreateAnnotationWidgetController.h>

#include "RemoteBLASTConsts.h"
#include "RemoteBLASTTask.h"
#include "ui_RemoteBLASTDialog.h"

namespace U2 {

class ADVSequenceObjectContext;
class AnnotationTableObject;
class CreateAnnotationWidgetController;

class SendSelectionDialog : public QDialog, Ui_RemoteBLASTDialog {
    Q_OBJECT
public:
    // \param seqCtx Sequence context.
    // \param isAminoSeq True if sequence is amino.
    // \param selectedPrimerPairNames This is the list is not empty, we show this names of primer groups, which primers are be about to be BLASTed.
    // If this list is empty, we show regular Annotation Controller settings.
    // \param parent Parent widget.
    SendSelectionDialog(ADVSequenceObjectContext* seqCtx, bool isAminoSeq, const QStringList& selectedPrimerPairNames, QWidget* parent);
    QString getGroupName() const;
    const QString& getAnnotationDescription() const;
    AnnotationTableObject* getAnnotationObject() const;
    const CreateAnnotationModel* getModel() const;
    QString getUrl() const;
private slots:
    void sl_scriptSelected(int index);
    void sl_megablastChecked(int state);
    void sl_serviceChanged(int);
    void sl_OK();
    void sl_Cancel();

private:
    void setupDataBaseList();
    void setUpSettings();
    void saveSettings();
    void alignComboBoxes();

public:
    bool translateToAmino = false;
    RemoteBLASTTaskSettings cfg;

private:
    bool isAminoSeq = false;
    CreateAnnotationWidgetController* annWgtController = nullptr;
    bool extImported = false;
    ADVSequenceObjectContext* seqCtx = nullptr;
};

}  // namespace U2
