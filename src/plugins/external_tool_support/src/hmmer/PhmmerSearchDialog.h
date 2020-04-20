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

#ifndef _U2_PHMMER_SEARCH_DIALOG_H_
#define _U2_PHMMER_SEARCH_DIALOG_H_

#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequence.h>

#include <U2Gui/CreateAnnotationWidgetController.h>

#include "PhmmerSearchSettings.h"
#include "ui_PhmmerSearchDialog.h"

namespace U2 {

class ADVSequenceObjectContext;

class PhmmerSearchDialogModel {
public:
    PhmmerSearchSettings phmmerSettings;
    QPointer<U2SequenceObject> dbSequence;
};

class PhmmerSearchDialog : public QDialog, public Ui_PhmmerSearchDialog {
    Q_OBJECT
public:
    PhmmerSearchDialog(U2SequenceObject *seqObj, QWidget *parent = NULL);
    PhmmerSearchDialog(ADVSequenceObjectContext* seqCtx, QWidget *parent = NULL);

private slots:
    void accept();
    void sl_queryToolButtonClicked();
    void sl_useEvalTresholdsButtonChanged(bool checked);
    void sl_useScoreTresholdsButtonChanged(bool checked);
    void sl_domZCheckBoxChanged(int state);
    void sl_maxCheckBoxChanged(int state);
    void sl_domESpinBoxChanged(int newVal);

private:
    void setModelValues();
    void getModelValues();
    void init(U2SequenceObject *seqObj);
    QString checkModel();

    PhmmerSearchDialogModel model;
    CreateAnnotationWidgetController *annotationsWidgetController;
    ADVSequenceObjectContext* seqCtx;

    static const QString    QUERY_FILES_DIR;
    static const QString    DOM_E_PLUS_PREFIX;
    static const QString    DOM_E_MINUS_PREFIX;
    static const QString    ANNOTATIONS_DEFAULT_NAME;
    static const int        ANNOTATIONS_WIDGET_LOCATION = 1;
};

}   // namespace U2

#endif // _U2_PHMMER_SEARCH_DIALOG_H_
