/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C)2008-2018 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option)any later version.
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

#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/CreateAnnotationTask.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/L10n.h>
#include <U2Core/U1AnnotationUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "HMMIO.h"
#include "HMMSearchDialogController.h"
#include "HMMSearchTask.h"
#include "TaskLocalStorage.h"
#include "hmmer2/funcs.h"

namespace U2 {

HMMSearchDialogController::HMMSearchDialogController(const U2SequenceObject* seqObj, QWidget* p)
: QDialog(p), seqCtx(NULL)
{
    init(seqObj);
}
HMMSearchDialogController::HMMSearchDialogController(ADVSequenceObjectContext* seqCtx, QWidget* p)
: QDialog(p), seqCtx(seqCtx)
{

    init(seqCtx->getSequenceObject());
}
void HMMSearchDialogController::init(const U2SequenceObject *seqObj){
    qDebug() << "HMMSearchDialogController was created";
    U2OpStatusImpl os;
    dnaSequence = seqObj->getWholeSequence(os);
    CHECK_OP_EXT(os, QMessageBox::critical(QApplication::activeWindow(), L10N::errorTitle(), os.getError()), );

    searchTask = NULL;
    setupUi(this);
    new HelpButton(this, buttonBox, "24748982");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Search"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Close"));

    CreateAnnotationModel cm;
    cm.hideLocation = true;
    cm.sequenceObjectRef = seqObj;
    cm.useAminoAnnotationTypes = seqObj->getAlphabet()->isAmino();
    cm.data->type = U2FeatureTypes::MiscSignal;
    cm.data->name = "hmm_signal";
    cm.sequenceLen = seqObj->getSequenceLength();
    createController = new CreateAnnotationWidgetController(cm, this);

    QWidget* w = createController->getWidget();
    QVBoxLayout* l = qobject_cast<QVBoxLayout*>(layout());
    l->insertWidget(1, w);
#ifdef UGENE_CELL
    algoCombo->addItem(tr("Cell BE optimized"), HMMSearchAlgo_CellOptimized);
#endif
    if(AppResourcePool::isSSE2Enabled()){
        algoCombo->addItem(tr("SSE optimized"), HMMSearchAlgo_SSEOptimized);
    }
    algoCombo->addItem(tr("Conservative"), HMMSearchAlgo_Conservative);

    okButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(hmmFileButton, SIGNAL(clicked()), SLOT(sl_hmmFileClicked()));
    connect(okButton, SIGNAL(clicked()), SLOT(sl_okClicked()));
    connect(domEvalueCuttofBox,SIGNAL(valueChanged(int)), SLOT(sl_expSpinBoxValueChanged(int)));
}

HMMSearchDialogController::~HMMSearchDialogController() {
    qDebug() << "HMMSearchDialogController was destroyed";
}

void HMMSearchDialogController::sl_expSpinBoxValueChanged(int i){
    if(i>=0){
        domEvalueCuttofBox->setPrefix("1E+");
    }
    else {
        domEvalueCuttofBox->setPrefix("1E");
    }
}

void HMMSearchDialogController::reject(){
    if (searchTask!=NULL){
        searchTask->cancel();
    }
    QDialog::reject();
}

void HMMSearchDialogController::sl_hmmFileClicked(){
    LastUsedDirHelper lod(HMMIO::HMM_ID);
    lod.url = U2FileDialog::getOpenFileName(this, tr("Select file with HMM model"), lod, HMMIO::getHMMFileFilter());
    if (lod.url.isEmpty()){
        return;
    }
    hmmFileEdit->setText(QFileInfo(lod.url).absoluteFilePath());
}

void HMMSearchDialogController::sl_okClicked(){
    if (searchTask!= NULL){
        accept(); //go to background
        return;
    }

    QString errMsg;
    QString hmmFile = hmmFileEdit->text();
    if (hmmFile.isEmpty()){
        hmmFileEdit->setFocus();
        errMsg = tr("HMM file not set!");
    }


    UHMMSearchSettings s;
    if (expertOptionsBox->isChecked()&& errMsg.isEmpty()){
        s.domE = pow(10,(float)domEvalueCuttofBox->value());
        s.domT = (float)minScoreBox->value();
        s.eValueNSeqs = evalueAsNSeqBox->value();
    }
    s.alg = HMMSearchAlgo(algoCombo->itemData(algoCombo->currentIndex()).toInt());

    if (errMsg.isEmpty()){
        errMsg = createController->validate();
    }

    if (!errMsg.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), errMsg);
        return;
    }

    bool objectPrepared = createController->prepareAnnotationObject();
    if (!objectPrepared){
        QMessageBox::warning(this, tr("Error"), tr("Cannot create an annotation object. Please check settings"));
        return;
    }

    const CreateAnnotationModel& cm = createController->getModel();
    if(seqCtx != NULL){
        seqCtx->getAnnotatedDNAView()->tryAddObject(cm.getAnnotationObject());
    }
    QString annotationName = cm.data->name;
    searchTask = new HMMSearchToAnnotationsTask(hmmFile, dnaSequence, cm.getAnnotationObject(), cm.groupName, cm.description, cm.data->type, annotationName, s);
    searchTask->setReportingEnabled(true);
    connect(searchTask, SIGNAL(si_stateChanged()), SLOT(sl_onStateChanged()));
    connect(searchTask, SIGNAL(si_progressChanged()), SLOT(sl_onProgressChanged()));
    AppContext::getTaskScheduler()->registerTopLevelTask(searchTask);
    statusLabel->setText(tr("Starting search process"));

    //update buttons
    okButton->setText(tr("Hide"));
    cancelButton->setText(tr("Cancel"));

    // new default behavior: hide dialog and use taskview to track the progress and results
    accept(); //go to background
}

void HMMSearchDialogController::sl_onStateChanged(){
    Task* t = qobject_cast<Task*>(sender());
    assert(searchTask != NULL);
    if (searchTask!=t || t->getState()!= Task::State_Finished){
        return;
    }
    searchTask->disconnect(this);
    const TaskStateInfo& si = searchTask->getStateInfo();
    if (si.hasError()){
        statusLabel->setText(tr("HMM search finished with error: %1").arg(si.getError()));
    } else {
        statusLabel->setText(tr("HMM search finished successfuly!"));
    }
    okButton->setText(tr("Search"));
    cancelButton->setText(tr("Close"));

    searchTask = NULL;
}

void HMMSearchDialogController::sl_onProgressChanged(){
    assert(searchTask == sender());
    statusLabel->setText(tr("Progress: %1%").arg(qMax(0, searchTask->getProgress())));
}

//////////////////////////////////////////////////////////////////////////
// TASKS

HMMSearchToAnnotationsTask::HMMSearchToAnnotationsTask(const QString& _hmmFile,
                                                       const DNASequence& s,
                                                       AnnotationTableObject *ao,
                                                       const QString& _agroup,
                                                       const QString &annDescription,
                                                       U2FeatureType aType,
                                                       const QString& _aname,
                                                       const UHMMSearchSettings& _settings)
: Task("", TaskFlags_NR_FOSCOE | TaskFlag_ReportingIsSupported),
hmmFile(_hmmFile), dnaSequence(s), agroup(_agroup), annDescription(annDescription), aType(aType), aname(_aname), settings(_settings),
readHMMTask(NULL), searchTask(NULL), createAnnotationsTask(NULL), aobj(ao)
{
    setVerboseLogMode(true);
    setTaskName(tr("HMM search, file '%1'").arg(QFileInfo(hmmFile).fileName()));

    readHMMTask = new HMMReadTask(hmmFile);
    readHMMTask->setSubtaskProgressWeight(0);

    if (dnaSequence.alphabet->isRaw()) {
        stateInfo.setError(tr("RAW alphabet is not supported!"));
    } else {
        addSubTask(readHMMTask);
    }
}

QList<Task*> HMMSearchToAnnotationsTask::onSubTaskFinished(Task* subTask){
    Q_UNUSED(subTask);

    QList<Task*> res;

    if (hasError()|| isCanceled()){
        return res;
    }

    if (aobj.isNull()) {
        stateInfo.setError(tr("Annotation object was removed"));
        return res;
    }

    if (searchTask == NULL){
        assert(readHMMTask->isFinished());
        plan7_s* hmm = readHMMTask->getHMM();
        assert(hmm!=NULL);

        searchTask = new HMMSearchTask(hmm, dnaSequence, settings);
        res.append(searchTask);
    } else if (createAnnotationsTask == NULL){
        assert(searchTask->isFinished()&& !searchTask->hasError());
        QList<SharedAnnotationData> annotations = searchTask->getResultsAsAnnotations(aType, aname);
        U1AnnotationUtils::addDescriptionQualifier(annotations, annDescription);
        if (!annotations.isEmpty()) {
            createAnnotationsTask = new CreateAnnotationsTask(aobj, annotations, agroup);
            createAnnotationsTask->setSubtaskProgressWeight(0);
            res.append(createAnnotationsTask);
        }
    }
    return res;
}

QString HMMSearchToAnnotationsTask::generateReport()const {
    QString res;
    res+="<table>";
    res+="<tr><td width=200><b>" + tr("HMM profile used")+ "</b></td><td>" + QFileInfo(hmmFile).absoluteFilePath()+ "</td></tr>";

    if (hasError()|| isCanceled()){
        res+="<tr><td width=200><b>" + tr("Task was not finished")+ "</b></td><td></td></tr>";
        res+="</table>";
        return res;
    }

    res+="<tr><td><b>" + tr("Result annotation table")+ "</b></td><td>" + aobj->getDocument()->getName()+ "</td></tr>";
    res+="<tr><td><b>" + tr("Result annotation group")+ "</b></td><td>" + agroup + "</td></tr>";
    res+="<tr><td><b>" + tr("Result annotation name")+  "</b></td><td>" + aname + "</td></tr>";

    int nResults = createAnnotationsTask == NULL ? 0 : createAnnotationsTask->getAnnotationCount();
    res+="<tr><td><b>" + tr("Results count")+  "</b></td><td>" + QString::number(nResults)+ "</td></tr>";
    res+="</table>";
    return res;
}


}//namespace
