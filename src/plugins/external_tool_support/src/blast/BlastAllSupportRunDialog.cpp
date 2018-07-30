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

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/AppResources.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Gui/DialogUtils.h>
#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultiTask.h>
#include <U2Core/ProjectService.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/HelpButton.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/OpenViewTask.h>
#include <U2Gui/RegionSelector.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include <QToolButton>
#include <QMessageBox>

#include "BlastAllSupportRunDialog.h"

namespace U2 {

////////////////////////////////////////
//BlastAllSupportRunDialog
BlastAllSupportRunDialog::BlastAllSupportRunDialog(ADVSequenceObjectContext* seqCtx, QString &lastDBPath, QString &lastDBName, QWidget *parent)
: BlastRunCommonDialog(parent, BlastAll, false, QStringList()), lastDBPath(lastDBPath), lastDBName(lastDBName), seqCtx(seqCtx), regionSelector(NULL)
{
    dnaso = seqCtx->getSequenceObject();
    CreateAnnotationModel ca_m;
    ca_m.hideAnnotationType = true;
    ca_m.hideAnnotationName = true;
    ca_m.hideLocation = true;
    ca_m.sequenceObjectRef = GObjectReference(dnaso);
    ca_m.sequenceLen = dnaso->getSequenceLength();
    ca_c = new CreateAnnotationWidgetController(ca_m, this);
    annotationWidgetLayout->addWidget(ca_c->getWidget());

    int lastRow = settingsGridLayout->rowCount();
    regionSelector = new RegionSelector(this, seqCtx->getSequenceLength(), false, seqCtx->getSequenceSelection());
    settingsGridLayout->addWidget(regionSelector, lastRow, 0, 1, 3);

    okButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    programName->removeItem(2);//gpu-blastp
    if(dnaso->getAlphabet()->getType() == DNAAlphabet_AMINO){
        programName->removeItem(0);//blastn
        programName->removeItem(1);//blastx
        programName->removeItem(2);//tblastx
        settings.isNucleotideSeq=false;
    }else{
        programName->removeItem(1);//blastp
        programName->removeItem(2);//tblastn
        settings.isNucleotideSeq=true;
    }
    dbSelector->databasePathLineEdit->setText(lastDBPath);
    dbSelector->baseNameLineEdit->setText(lastDBName);
    connect(cancelButton,SIGNAL(clicked()),SLOT(reject()));
}

U2Region BlastAllSupportRunDialog::getSelectedRegion() const {
    return regionSelector->isWholeSequenceSelected() ? U2Region(0, seqCtx->getSequenceLength()) : regionSelector->getRegion();
}

void BlastAllSupportRunDialog::sl_lineEditChanged(){
    okButton->setEnabled(dbSelector->isInputDataValid());
}

void BlastAllSupportRunDialog::sl_runQuery(){
    QString error = ca_c->validate();
    if (!error.isEmpty()) {
        QMessageBox::critical(NULL, tr("Wrong parameters for creating annotations"), error);
        return;
    }
    settings.outputResFile=ca_c->getModel().newDocUrl;
    if(ca_c->isNewObject()) {
        U2OpStatusImpl os;
        const U2DbiRef dbiRef = AppContext::getDbiRegistry( )->getSessionTmpDbiRef( os );
        SAFE_POINT_OP( os, );
        settings.aobj = new AnnotationTableObject( "Annotations", dbiRef );
        settings.aobj->addObjectRelation(GObjectRelation(ca_c->getModel().sequenceObjectRef, ObjectRole_Sequence));
    }
    else {
        bool objectPrepared = ca_c->prepareAnnotationObject();
        if (!objectPrepared){
            QMessageBox::warning(this, tr("Error"), tr("Cannot create an annotation object. Please check settings"));
            return;
        }
        settings.aobj = ca_c->getModel().getAnnotationObject();
    }
    if (!dbSelector->validateDatabaseDir()) {
        return;
    }
    settings.groupName = ca_c->getModel().groupName;
    settings.annDescription = ca_c->getModel().description;
    settings.alphabet = dnaso->getAlphabet();
    getSettings(settings);
    lastDBPath = dbSelector->databasePathLineEdit->text();
    lastDBName = dbSelector->baseNameLineEdit->text();
    settings.outputType = 7;//By default set output file format to xml
    if(seqCtx != NULL){
        seqCtx->getAnnotatedDNAView()->tryAddObject(settings.aobj);
    }
    accept();
}
////////////////////////////////////////
//BlastAllWithExtFileSpecifySupportRunDialog
BlastAllWithExtFileSpecifySupportRunDialog::BlastAllWithExtFileSpecifySupportRunDialog(QString &lastDBPath, QString &lastDBName, QWidget *parent)
: BlastRunCommonDialog(parent, BlastAll, false, QStringList()), lastDBPath(lastDBPath), lastDBName(lastDBName), hasValidInput(false)
{
    ca_c = NULL;
    wasNoOpenProject = false;
    //create input file widget
    QWidget *widget = new QWidget(parent);
    inputFileLineEdit = new FileLineEdit("","", false, widget);
    inputFileLineEdit->setReadOnly(true);
    inputFileLineEdit->setText("");
    QToolButton * selectToolPathButton = new QToolButton(widget);
    selectToolPathButton->setObjectName("browseInput");
    selectToolPathButton->setVisible(true);
    selectToolPathButton->setText("...");

    okButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(selectToolPathButton, SIGNAL(clicked()), inputFileLineEdit, SLOT(sl_onBrowse()));
    connect(inputFileLineEdit,SIGNAL(textChanged(QString)),this, SLOT(sl_inputFileLineEditChanged(QString)));

    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(inputFileLineEdit);
    layout->addWidget(selectToolPathButton);

    QGroupBox* inputFileGroupBox = new QGroupBox(tr("Select input file"),widget);
    inputFileGroupBox->setLayout(layout);
    QBoxLayout* parentLayout = qobject_cast<QBoxLayout*>(this->layout());
    assert(parentLayout);
    parentLayout->insertWidget(0, inputFileGroupBox);

    programName->removeItem(2);//gpu-blastp

    dbSelector->databasePathLineEdit->setText(lastDBPath);
    dbSelector->baseNameLineEdit->setText(lastDBName);
    connect(cancelButton,SIGNAL(clicked()),SLOT(sl_cancel()));
    connect(this,SIGNAL(rejected()),SLOT(sl_cancel()));
}

const QList<BlastTaskSettings> &BlastAllWithExtFileSpecifySupportRunDialog::getSettingsList() const {
    return settingsList;
}

void BlastAllWithExtFileSpecifySupportRunDialog::sl_lineEditChanged(){
    okButton->setEnabled(dbSelector->isInputDataValid() && hasValidInput);
}

namespace {
    const char *INPUT_URL_PROP = "input_url";
}

void BlastAllWithExtFileSpecifySupportRunDialog::sl_inputFileLineEditChanged(const QString &url){
    hasValidInput = false;
    sl_lineEditChanged();
    CHECK(!url.isEmpty(), );

    Project *proj = AppContext::getProject();
    if (NULL == proj) {
        wasNoOpenProject = true;
    } else {
        Document *doc = proj->findDocumentByURL(url);
        if (doc != NULL) {
            if (doc->isLoaded()) {
                tryApplyDoc(doc);
            } else {
                LoadUnloadedDocumentAndOpenViewTask *loadTask= new LoadUnloadedDocumentAndOpenViewTask(doc);
                loadTask->setProperty(INPUT_URL_PROP, url);
                connect(loadTask, SIGNAL(si_stateChanged()), SLOT(sl_inputFileOpened()));
                AppContext::getTaskScheduler()->registerTopLevelTask(loadTask);
            }
            return;
        }
    }

    loadDoc(url);
}

void BlastAllWithExtFileSpecifySupportRunDialog::onFormatError() {
    QMessageBox::critical(this, tr("Wrong input file"), tr("This file has the incompatible format for the BLAST+ search."));
    inputFileLineEdit->setText("");
}

void BlastAllWithExtFileSpecifySupportRunDialog::loadDoc(const QString &url) {
    FormatDetectionConfig config;
    config.useExtensionBonus = true;
    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(url, config);
    CHECK_EXT(!formats.isEmpty() && (NULL != formats.first().format), onFormatError(), );

    DocumentFormat *format = formats.first().format;
    CHECK_EXT(format->getSupportedObjectTypes().contains(GObjectTypes::SEQUENCE), onFormatError(), );

    LoadDocumentTask *loadTask= new LoadDocumentTask(format->getFormatId(), url, AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url)));
    AddDocumentAndOpenViewTask *openTask = new AddDocumentAndOpenViewTask(loadTask);
    openTask->setProperty(INPUT_URL_PROP, url);

    connect(openTask, SIGNAL(si_stateChanged()), SLOT(sl_inputFileOpened()));
    AppContext::getTaskScheduler()->registerTopLevelTask(openTask);
}

void BlastAllWithExtFileSpecifySupportRunDialog::sl_inputFileOpened(){
    Task *t = qobject_cast<Task*>(sender());
    CHECK(NULL != t, );
    CHECK(t->isFinished() && !t->hasError(), );

    Project *proj = AppContext::getProject();
    SAFE_POINT(NULL != proj, "No opened project", );

    QString url = t->property(INPUT_URL_PROP).toString();
    Document *doc = proj->findDocumentByURL(url);
    SAFE_POINT(NULL != doc, "No loaded document", );

    tryApplyDoc(doc);
}
void BlastAllWithExtFileSpecifySupportRunDialog::tryApplyDoc(Document *doc) {
    int numOfSequences = 0;
    foreach (GObject *obj, doc->getObjects()) {
        if (obj->getGObjectType()==GObjectTypes::SEQUENCE) {
            numOfSequences++;
        }
    }
    settingsList.clear();
    sequencesRefList.clear();
    if (0 == numOfSequences) {
        QMessageBox::critical(this, tr("Wrong input file"), tr("This file does not contain any sequence."));
        inputFileLineEdit->setText("");
        return;
    }

    hasValidInput = true;
    foreach (GObject *obj, doc->getObjects()) {
        if (obj->getGObjectType() != GObjectTypes::SEQUENCE) {
            continue;
        }
        U2SequenceObject *seq = dynamic_cast<U2SequenceObject*>(obj);
        SAFE_POINT(NULL != seq, "NULL sequence object", );

        BlastTaskSettings localSettings;
        U2OpStatusImpl os;
        localSettings.querySequence = seq->getWholeSequenceData(os);
        CHECK_OP_EXT(os, QMessageBox::critical(this, L10N::errorTitle(), os.getError()), );
        localSettings.alphabet = seq->getAlphabet();
        if (localSettings.alphabet->getType() != DNAAlphabet_AMINO) {
            localSettings.isNucleotideSeq = true;
        }
        localSettings.queryFile = doc->getURLString();
        localSettings.querySequenceObject = seq;
        settingsList.append(localSettings);
        sequencesRefList.append(GObjectReference(obj));
    }

    CreateAnnotationModel ca_m;
    ca_m.hideAnnotationType = true;
    ca_m.hideAnnotationName = true;
    ca_m.hideLocation = true;
    ca_m.sequenceObjectRef = sequencesRefList[0];
    ca_m.sequenceLen = 10;
    ca_m.defaultIsNewDoc = true;
    if (NULL == ca_c) {
        ca_c = new CreateAnnotationWidgetController(ca_m, this);
        annotationWidgetLayout->addWidget(ca_c->getWidget());
    } else {
        ca_c->updateWidgetForAnnotationModel(ca_m);
    }

    sl_lineEditChanged();
}
void BlastAllWithExtFileSpecifySupportRunDialog::sl_runQuery(){
    QString error = ca_c->validate();
    if (!error.isEmpty()) {
        QMessageBox::critical(this, tr("Wrong parameters for creating annotations"), error);
        return;
    }

    for(int i=0; i<settingsList.length();i++){
        settingsList[i].outputResFile=ca_c->getModel().newDocUrl;
        if(ca_c->isNewObject()) {
            U2OpStatusImpl os;
            const U2DbiRef dbiRef = AppContext::getDbiRegistry( )->getSessionTmpDbiRef( os );
            SAFE_POINT_OP( os, );
            settingsList[i].aobj = new AnnotationTableObject( sequencesRefList[i].objName+" annotations", dbiRef );
            settingsList[i].aobj->addObjectRelation(GObjectRelation(sequencesRefList[i], ObjectRole_Sequence));
        }
        else {
            assert(false);//allways created new document for annotations
        }
        settingsList[i].groupName=ca_c->getModel().groupName;

        getSettings(settingsList[i]);

        //setup filters
        settingsList[i].outputType=7;//By default set output file format to xml
    }
    bool docAlreadyInProject=false;
    Project* proj=AppContext::getProject();
    foreach(Document* doc, proj->getDocuments()){
        if(doc->getURL() == inputFileLineEdit->text()){
            docAlreadyInProject=true;
        }
    }
    if(!docAlreadyInProject){
        QString url = inputFileLineEdit->text();
        Task * t = AppContext::getProjectLoader()->openWithProjectTask(url);
        if (t != NULL) {
            AppContext::getTaskScheduler()->registerTopLevelTask(t);
        }
    }
    if (!dbSelector->validateDatabaseDir()) {
        return;
    }
    lastDBPath = dbSelector->databasePathLineEdit->text();
    lastDBName = dbSelector->baseNameLineEdit->text();
    accept();
}
void BlastAllWithExtFileSpecifySupportRunDialog::sl_cancel(){
    if(qobject_cast<BlastAllWithExtFileSpecifySupportRunDialog*>(sender()) == NULL){
        reject();
        return;
    }
    if(wasNoOpenProject){
        ProjectService *projService = AppContext::getProjectService();
        CHECK(NULL != projService, );
        AppContext::getTaskScheduler()->registerTopLevelTask(projService->closeProjectTask());
    }
}
}//namespace
