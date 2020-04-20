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

#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include "HMMBuildDialogController.h"
#include "HMMIO.h"
#include "TaskLocalStorage.h"
#include "hmmer2/funcs.h"

namespace U2 {
HMMBuildDialogController::HMMBuildDialogController(const QString& _pn, const MultipleSequenceAlignment& _ma, QWidget* p) 
    : QDialog(p),
      ma(_ma->getCopy()),
      profileName(_pn),
      saveController(NULL) {
    setupUi(this);
    new HelpButton(this, buttonBox, "24748980");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Build"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Close"));

    if (!ma->isEmpty()) {
        msaFileButton->setHidden(true);
        msaFileEdit->setHidden(true);
        msaFileLabel->setHidden(true);
    }

    initSaveController();

    okButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(msaFileButton, SIGNAL(clicked()), SLOT(sl_msaFileClicked()));
    connect(okButton, SIGNAL(clicked()), SLOT(sl_okClicked()));
    
    task = NULL;
}

void HMMBuildDialogController::sl_msaFileClicked() {
    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(this, tr("Select file with alignment"),
        lod, DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, true));
    
    if (lod.url.isEmpty()) {
        return;
    }

    msaFileEdit->setText(QFileInfo(lod.url).absoluteFilePath());
}

void HMMBuildDialogController::sl_okClicked() {
    if (task != NULL) {
        accept(); //go to background
        return;
    }

    //prepare model
    UHMMBuildSettings s;
    s.name = profileName;
    QString errMsg;

    QString inFile = msaFileEdit->text();
    if (ma->isEmpty() && (inFile.isEmpty() || !QFileInfo(inFile).exists())) {
        errMsg = tr("Incorrect alignment file!");
        msaFileEdit->setFocus();
    }
    QString outFile = saveController->getSaveFileName();
    if (outFile.isEmpty() && errMsg.isEmpty()) {
        errMsg = tr("Incorrect HMM file!");
        resultFileEdit->setFocus();
    }
    if (expertGroup->isChecked() && errMsg.isEmpty()) {
        s.name = nameEdit->text().trimmed();
        s.strategy = P7_LS_CONFIG;
        if (hmmfsButton->isChecked()) {
            s.strategy = P7_FS_CONFIG;
        } else if (hmmsButton->isChecked()) {
            s.strategy = P7_BASE_CONFIG;
        } else if (hmmswButton->isChecked()) {
            s.strategy = P7_SW_CONFIG;
        }
    }
    
    if (!errMsg.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), errMsg);
        return;
    }

    task = ma->isEmpty() ? new HMMBuildToFileTask(inFile, outFile, s) : new HMMBuildToFileTask(ma, outFile, s);
    task->setReportingEnabled(true);
    connect(task, SIGNAL(si_stateChanged()), SLOT(sl_onStateChanged()));
    connect(task, SIGNAL(si_progressChanged()), SLOT(sl_onProgressChanged()));
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
    statusLabel->setText(tr("Starting build process"));

    //update buttons
    okButton->setText(tr("Hide"));
    cancelButton->setText(tr("Cancel"));

    // new default behavior: hide dialog and use taskview to track the progress and results
    accept(); //go to background
}


void HMMBuildDialogController::reject() {
    if (task!=NULL) {
        task->cancel();
    }
    QDialog::reject();
}

void HMMBuildDialogController::sl_onStateChanged() {
    Task* t = qobject_cast<Task*>(sender());
    assert(task!=NULL);
    if (task!=t || t->getState() != Task::State_Finished) {
        return;
    }
    task->disconnect(this);
    const TaskStateInfo& si = task->getStateInfo();
    if (si.hasError()) {
        statusLabel->setText(tr("HMM build finished with errors: %1").arg(si.getError()));
    } else if (task->isCanceled()) {
        statusLabel->setText(tr("HMM build canceled"));
    } else {
        statusLabel->setText(tr("HMM build finished successfuly!"));
    }
    okButton->setText(tr("Build"));
    cancelButton->setText(tr("Close"));

    AppContext::getTaskScheduler()->disconnect(this);
    task = NULL;

}

void HMMBuildDialogController::sl_onProgressChanged() {
    assert(task==sender());
    statusLabel->setText(tr("Progress: %1%").arg(task->getProgress()));
}

void HMMBuildDialogController::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultDomain = HMMIO::HMM_ID;
    config.defaultFormatId = HMMIO::HMM_ID;
    config.fileDialogButton = resultFileButton;
    config.fileNameEdit = resultFileEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Select file with HMM profile");

    SaveDocumentController::SimpleFormatsInfo formats;
    formats.addFormat(HMMIO::HMM_ID, tr("HMM models"), QStringList(HMMIO::HMM_EXT));

    saveController = new SaveDocumentController(config, formats, this);
}


//////////////////////////////////////////////////////////////////////////
//  HMMBuildTask

HMMBuildToFileTask::HMMBuildToFileTask(const QString& inFile, const QString& _outFile, const UHMMBuildSettings& s) 
    : Task("", TaskFlag_ReportingIsSupported),
      settings(s),
      outFile(_outFile),
      loadTask(NULL),
      buildTask(NULL)
{
    setTaskName(tr("Build HMM profile '%1' -> '%2'").arg(QFileInfo(inFile).fileName()).arg(QFileInfo(outFile).fileName()));
    setVerboseLogMode(true);

    assert(!(inFile.isEmpty() || outFile.isEmpty()));
    //todo: support all formats with alignments!

    DocumentFormatConstraints c;
    c.checkRawData = true;
    c.supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    c.rawData = IOAdapterUtils::readFileHeader(inFile);
    c.addFlagToExclude(DocumentFormatFlag_CannotBeCreated);
    QList<DocumentFormatId> formats = AppContext::getDocumentFormatRegistry()->selectFormats(c);
    if (formats.isEmpty()) {
        stateInfo.setError(  tr("Error reading alignment file") );
        return;
    }

    DocumentFormatId alnFormat = formats.first();

    
    loadTask = new LoadDocumentTask(alnFormat, inFile, 
                                    AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(inFile)),
                                    QVariantMap());
    addSubTask(loadTask);
}

HMMBuildToFileTask::HMMBuildToFileTask(const MultipleSequenceAlignment& _ma, const QString& _outFile, const UHMMBuildSettings& s) 
: Task("", TaskFlags_FOSCOE | TaskFlag_ReportingIsSupported),
settings(s), outFile(_outFile), ma(_ma->getCopy()), loadTask(NULL), buildTask(NULL)
{
    setTaskName(tr("Build HMM profile to '%1'").arg(QFileInfo(outFile).fileName()));
    setVerboseLogMode(true);

    assert(!(outFile.isEmpty() || ma->isEmpty()));
    if (settings.name.isEmpty()) {
        QFileInfo fi(outFile);
        settings.name = fi.baseName(); //FIXME temporary workaround
    }
    buildTask = new HMMBuildTask(settings, ma);
    addSubTask(buildTask);
}


QList<Task*> HMMBuildToFileTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    
    if (hasError() || isCanceled()) {
        return res;
    }

    if (subTask == loadTask) {
        assert(ma->isEmpty());
        Document*  doc = loadTask->getDocument();
        //assert(doc);
        if(!doc) {
            stateInfo.setError(tr("Incorrect input file"));
            return res;
        }
        QList<GObject*> list = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
        if (list.isEmpty()) {
            stateInfo.setError(  tr("Alignment object not found!") );
        } else {
            MultipleSequenceAlignmentObject* msa = qobject_cast<MultipleSequenceAlignmentObject*>(list.first());
            const MultipleSequenceAlignment ma = msa->getMultipleAlignment();
            if (settings.name.isEmpty()) {
                settings.name = msa->getGObjectName() == MA_OBJECT_NAME ? doc->getName() : msa->getGObjectName();
            }
            buildTask = new HMMBuildTask(settings, ma);
            res.append(buildTask);
        }
    }
    return res;
}

void HMMBuildToFileTask::run() {
    TaskLocalData::createHMMContext(getTaskId(), true);
    _run();
    TaskLocalData::freeHMMContext(getTaskId());
}

void HMMBuildToFileTask::_run() {
    if (stateInfo.hasError()) {
        return;
    }
    assert(buildTask!=NULL);
    assert(buildTask->getState() == Task::State_Finished);
    if (buildTask->getStateInfo().hasError()) {
        stateInfo.setError(  buildTask->getStateInfo().getError() );
        return;
    }
    plan7_s* hmm = buildTask->getHMM();
    assert(hmm!=NULL);
	IOAdapterFactory* iof;
	iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(outFile));
	
    //todo: write command line too!
    HMMIO::writeHMM2(iof, outFile, stateInfo, hmm);
}

QString HMMBuildToFileTask::generateReport() const {
    QString res;
    res+="<table>";
    res+="<tr><td width=200><b>" + tr("Source alignment") + "</b></td><td>" + (loadTask ==  NULL ? settings.name : loadTask->getURL().getURLString()) + "</td></tr>";
    res+="<tr><td><b>" + tr("Profile name") + "</b></td><td>" + settings.name + "</td></tr>";
    if (hasError()) {
        res+="<tr><td width=200><b>" + tr("Task was not finished") + "</b></td><td></td></tr>";
        res+="</table>";
        return res;
    }

    res+="<tr><td><b>" + tr("Profile file") + "</b></td><td>" + outFile + "</td></tr>";
    res+="<tr><td><b>" + tr("Expert options") + "</b></td><td>";
    switch(settings.strategy) {
        case P7_BASE_CONFIG:  res+= "-g"; break;
        case P7_LS_CONFIG:    res+= tr("none"); break;
        case P7_FS_CONFIG:    res+= "-f"; break;
        case P7_SW_CONFIG:    res+= "-s"; break;
    }
    res+="</td></tr>";

    res+="</table>";
    return res;
}


HMMBuildTask::HMMBuildTask(const UHMMBuildSettings& s, const MultipleSequenceAlignment& _ma) 
  : Task("", TaskFlag_None), ma(_ma->getCopy()), settings(s), hmm(NULL)
{
    GCOUNTER( cvar, tvar, "HMMBuildTask" );
    setTaskName(tr("Build HMM profile '%1'").arg(s.name));
}

HMMBuildTask::~HMMBuildTask() {
    if (hmm!=NULL) {
        FreePlan7(hmm);
    }
}

void HMMBuildTask::run() {
    TaskLocalData::createHMMContext(getTaskId(), true);
    _run();
    TaskLocalData::freeHMMContext(getTaskId());
}

void HMMBuildTask::_run() {
    if (ma->getNumRows() == 0) {
        stateInfo.setError(  tr("Multiple alignment is empty") );
        return;
    }
    if (ma->getLength() == 0) {
        stateInfo.setError(  tr("Multiple alignment is of 0 length") );
        return;
    }
    //todo: check HMM for RAW alphabet!
    if (ma->getAlphabet()->isRaw()) {
        stateInfo.setError(  tr("Invalid alphabet! Only amino and nucleic alphabets are supported") );
        return;
    }

    //everything ok here: fill msa

    msa_struct* msa = MSAAlloc(ma->getNumRows(), ma->getLength());
    if (msa == NULL) {
        stateInfo.setError(  tr("Error creating MSA structure") );
        return;
    }
    U2OpStatus2Log os;
    for (int i=0; i<ma->getNumRows();i++) {
        const MultipleSequenceAlignmentRow row = ma->getMsaRow(i);
        QByteArray seq = row->toByteArray(os, ma->getLength());
        free(msa->aseq[i]);
        msa->aseq[i] = sre_strdup(seq.constData(), seq.size());
        QByteArray name = row->getName().toLatin1();
        msa->sqname[i] = sre_strdup(name.constData(), name.size());
        msa->wgt[i] = 1.0; // default weights 
    }
    
    int atype = ma->getAlphabet()->isNucleic() ? hmmNUCLEIC :hmmAMINO;
    try {
        hmm = UHMMBuild::build(msa, atype, settings, stateInfo);
    } catch (const HMMException &e) {
        stateInfo.setError(  e.error );
    }
    
    assert(hmm!=NULL || stateInfo.hasError());
    
    MSAFree(msa);
}

}//namespace
