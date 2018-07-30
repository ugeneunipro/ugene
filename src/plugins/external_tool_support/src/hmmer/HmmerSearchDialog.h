/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_HMMER_SEARCH_DIALOG_H_
#define _U2_HMMER_SEARCH_DIALOG_H_

#include <QButtonGroup>

#include <U2Gui/CreateAnnotationWidgetController.h>

#include "HmmerSearchTask.h"
#include "ui_HmmerSearchDialog.h"

namespace U2 {

class ADVSequenceObjectContext;
class U2SequenceObject;

class HmmerSearchDialogModel {
public:
    HmmerSearchSettings         searchSettings;
    QPointer<U2SequenceObject>  sequence;
};

class HmmerSearchDialog : public QDialog, public Ui_HmmerSearchDialog {
    Q_OBJECT
public:
    HmmerSearchDialog(U2SequenceObject *seqObj, QWidget *parent = NULL);
    HmmerSearchDialog(ADVSequenceObjectContext* seqCtx, QWidget *parent = NULL);

    static const QString DOM_E_PLUS_PREFIX;
    static const QString DOM_E_MINUS_PREFIX;
    static const QString HMM_FILES_DIR_ID;
    static const QString ANNOTATIONS_DEFAULT_NAME;

private slots:
    void sl_okButtonClicked();
    void sl_useEvalTresholdsButtonChanged(bool checked);
    void sl_useScoreTresholdsButtonChanged(bool checked);
    void sl_useExplicitScoreTresholdButton(bool checked);
    void sl_maxCheckBoxChanged(int state);
    void sl_domESpinBoxChanged(int newVal);
    void sl_queryHmmFileToolButtonClicked();
    void sl_domZCheckBoxChanged(int state);

private:
    void setModelValues();
    void getModelValues();
    void init(U2SequenceObject *seqObj);
    QString checkModel();

    QButtonGroup                        useScoreTresholdGroup;
    CreateAnnotationWidgetController *  annotationsWidgetController;
    HmmerSearchDialogModel              model;
    ADVSequenceObjectContext*           seqCtx;
};

}   // namespace U2

#endif // _U2_HMMER_SEARCH_DIALOG_H_
