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

#ifndef _U2_HMMSEARCH_DIALOG_CONTROLLER_H_
#define _U2_HMMSEARCH_DIALOG_CONTROLLER_H_

#include "uhmmsearch.h"

#include <ui_HMMSearchDialog.h>

#include <U2Core/Task.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DNASequence.h>
#include <U2Core/AnnotationData.h>

#include <QDebug>

#include <QDialog>
#include <QPointer>

struct plan7_s;

namespace U2 {

class ADVSequenceObjectContext;
class CreateAnnotationWidgetController;
class CreateAnnotationModel;
class AnnotationTableObject;
class CreateAnnotationsTask;
class LoadUnloadedDocumentTask;
class DNATranslation;
class HMMSearchTask;
class HMMSearchTaskResult;
class U2SequenceObject;
class HMMReadTask;

class HMMSearchDialogController : public QDialog, public Ui_HMMSearchDialog {
    Q_OBJECT
public:
    HMMSearchDialogController(const U2SequenceObject* obj, QWidget* p = NULL);
    HMMSearchDialogController(ADVSequenceObjectContext* seqCtx, QWidget* p = NULL);
    ~HMMSearchDialogController();

public slots:
    void reject();

private slots:
    void sl_hmmFileClicked();
    void sl_okClicked();

    void sl_expSpinBoxValueChanged(int);

    void sl_onStateChanged();
    void sl_onProgressChanged();

private:
    void init(const U2SequenceObject* seqObj);
    DNASequence                         dnaSequence;
    Task*                               searchTask;
    CreateAnnotationWidgetController*   createController;
    QPushButton*                        okButton;
    QPushButton*                        cancelButton;
    ADVSequenceObjectContext*           seqCtx;
};


//////////////////////////////////////////////////////////////////////////
// tasks

class HMMSearchToAnnotationsTask : public Task {
    Q_OBJECT
public:
    HMMSearchToAnnotationsTask(const QString& hmmFile, const DNASequence& s, AnnotationTableObject* aobj, const QString& group,
        const QString &annDescription, U2FeatureType aType, const QString& aname, const UHMMSearchSettings& settings);

    virtual QList<Task*> onSubTaskFinished(Task* subTask);
    QString generateReport() const;

private:

    QString                     hmmFile;
    DNASequence                 dnaSequence;
    QString                     agroup;
    const QString               annDescription;
    U2FeatureType               aType;
    QString                     aname;
    UHMMSearchSettings          settings;

    HMMReadTask*                readHMMTask;
    HMMSearchTask*              searchTask;
    CreateAnnotationsTask*      createAnnotationsTask;
    QPointer<AnnotationTableObject> aobj;
};

} // namespace U2

#endif
