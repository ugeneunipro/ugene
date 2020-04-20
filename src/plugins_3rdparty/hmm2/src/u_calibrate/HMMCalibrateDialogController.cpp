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
#include <U2Core/AppResources.h>
#include <U2Core/IOAdapter.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include "HMMCalibrateDialogController.h"
#include "HMMCalibrateTask.h"
#include "HMMIO.h"

namespace U2 {

HMMCalibrateDialogController::HMMCalibrateDialogController(QWidget* w) 
    : QDialog(w),
      saveController(NULL) {
    task = NULL;
    setupUi(this);
    new HelpButton(this, buttonBox, "24748981");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Calibrate"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Close"));

    okButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    connect(hmmFileButton, SIGNAL(clicked()), SLOT(sl_hmmFileButtonClicked()));
    connect(okButton, SIGNAL(clicked()), SLOT(sl_okButtonClicked()));

    initSaveController();
}

void HMMCalibrateDialogController::sl_hmmFileButtonClicked() {
    LastUsedDirHelper lod(HMMIO::HMM_ID);
    lod.url = U2FileDialog::getOpenFileName(this, tr("Select file with HMM model"), lod, HMMIO::getHMMFileFilter());
    if (lod.url.isEmpty()) {
        return;
    }
    hmmFileEdit->setText(QFileInfo(lod.url).absoluteFilePath());
}

void HMMCalibrateDialogController::sl_okButtonClicked() {
    if (task != NULL) {
        accept(); //go to background
        return;
    }

    // try prepare model
   UHMMCalibrateSettings s;
   s.nThreads = AppResourcePool::instance()->getIdealThreadCount();
   QString errMsg;

   QString inFile = hmmFileEdit->text();
   QString outFile = inFile;
   if (inFile.isEmpty() && !QFileInfo(inFile).exists()) {
       errMsg = tr("Incorrect HMM file!");
       hmmFileEdit->setFocus();
   }

   if (expertGroupBox->isChecked() && errMsg.isEmpty()) {
       if (fixedBox->value() < 0) {
           errMsg = tr("Illegal fixed length value!");
           fixedBox->setFocus();
       } else {
           s.fixedlen = fixedBox->value();
       }

       s.lenmean = meanBox->value();
       s.nsample = numBox->value();
       s.lensd  = (float)sdBox->value();
       if (seedBox->value()!=0) {
           s.seed = seedBox->value();
       }
   }

   if (outputGroupBox->isChecked() && errMsg.isEmpty()) {
        outFile = saveController->getSaveFileName();
        if (outFile.isEmpty()) {
            errMsg = tr("Invalid output file name");
            outFileEdit->setFocus();
        }
   }

   if (!errMsg.isEmpty())  {
       QMessageBox::critical(this, tr("Error"), errMsg);
       return;
   }

   // run task
   task = new HMMCalibrateToFileTask(inFile, outFile, s);
   task->setReportingEnabled(true);
   connect(task, SIGNAL(si_stateChanged()), SLOT(sl_onStateChanged()));
   connect(task, SIGNAL(si_progressChanged()), SLOT(sl_onProgressChanged()));
   AppContext::getTaskScheduler()->registerTopLevelTask(task);
   statusLabel->setText(tr("Starting calibration process"));

   //update buttons
   okButton->setText(tr("Hide"));
   cancelButton->setText(tr("Cancel"));

   // new default behavior: hide dialog and use taskview to track the progress and results
   accept(); //go to background
}


void HMMCalibrateDialogController::sl_onStateChanged() {
    Task* t = qobject_cast<Task*>(sender());
    assert(task!=NULL);
    if (task!=t || t->getState() != Task::State_Finished) {
        return;
    }
    task->disconnect(this);
    const TaskStateInfo& si = task->getStateInfo();
    if (si.hasError()) {
        statusLabel->setText(tr("Calibration finished with errors: %1").arg(si.getError()));
    } else if (task->isCanceled()) {
        statusLabel->setText(tr("Calibration was cancelled"));
    } else {
        statusLabel->setText(tr("Calibration finished successfuly!"));
    }
    okButton->setText(tr("Calibrate"));
    cancelButton->setText(tr("Close"));

    task = NULL;
}

void HMMCalibrateDialogController::sl_onProgressChanged() {
    assert(task==sender());
    statusLabel->setText(tr("Progress: %1%").arg(task->getProgress()));
}

void HMMCalibrateDialogController::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultDomain = HMMIO::HMM_ID;
    config.defaultFormatId = HMMIO::HMM_ID;
    config.fileDialogButton = outFileButton;
    config.fileNameEdit = outFileEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Select file with HMM model");

    SaveDocumentController::SimpleFormatsInfo formats;
    formats.addFormat(HMMIO::HMM_ID, tr("HMM models"), QStringList(HMMIO::HMM_EXT));

    saveController = new SaveDocumentController(config, formats, this);
}

void HMMCalibrateDialogController::reject() {
    if (task!=NULL) {
        task->cancel();
    }
    QDialog::reject();
}

}//namespace
