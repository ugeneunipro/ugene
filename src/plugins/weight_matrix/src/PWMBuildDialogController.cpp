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

#include <U2Algorithm/PWMConversionAlgorithm.h>
#include <U2Algorithm/PWMConversionAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DIProperties.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/DocumentFormatUtils.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include "PWMBuildDialogController.h"
#include "WeightMatrixIO.h"
#include "WeightMatrixPlugin.h"

#define SETTINGS_ROOT   QString("plugin_weight_matrix/")

namespace U2 {

PWMBuildDialogController::PWMBuildDialogController(QWidget* w)
    : QDialog(w),
      saveController(NULL),
      logoArea(NULL) {
    task = NULL;
    setupUi(this);
    new HelpButton(this, buttonBox, "24749004");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Start"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    QStringList algo = AppContext::getPWMConversionAlgorithmRegistry()->getAlgorithmIds();
    algorithmCombo->addItems(algo);

    this->resize(this->width(), this->minimumHeight());

    initFrequencySaveController();

    okButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    connect(inputButton, SIGNAL(clicked()), SLOT(sl_inFileButtonClicked()));
    connect(okButton, SIGNAL(clicked()), SLOT(sl_okButtonClicked()));
    connect(weightButton, SIGNAL(toggled(bool)), SLOT(sl_matrixTypeChanged(bool)));
}


void PWMBuildDialogController::sl_inFileButtonClicked() {
    LastUsedDirHelper lod;
    lod.url = U2FileDialog::getOpenFileName(this, tr("Select file with alignment"), lod,
        DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT, true).append("\n").append(
        DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, false)));
    if (lod.url.isEmpty()) {
        return;
    }


    QString inFile = QFileInfo(lod.url).absoluteFilePath();
    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(inFile);
    if (formats.isEmpty()) {
        reportError(L10N::notSupportedFileFormat(lod.url));
        return;
    }

    DocumentFormat* format = NULL;
    foreach(const FormatDetectionResult& i, formats) {
        if (i.format->getSupportedObjectTypes().contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT)) {
            format = i.format;
            break;
        }
    }

    if (format == NULL) {
        foreach(const FormatDetectionResult& i, formats) {
            if (i.format->getSupportedObjectTypes().contains(GObjectTypes::SEQUENCE)) {
                format = i.format;
                break;
            }
        }
    }

    if(format == NULL){
        reportError(tr("Could not detect the format of the file. Files must be in supported multiple alignment or sequence formats."));
        return;
    }
    inputEdit->setText(inFile);

    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(inFile));
    TaskStateInfo ti;
    QVariantMap hints;
    Document *doc = format->loadDocument(iof, inFile, hints, ti);
    CHECK_OP_EXT(ti, reportError(ti.getError()), );

    QList<GObject*> mobjs = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
    if (!mobjs.isEmpty()) {
        MultipleSequenceAlignmentObject* mobj =  qobject_cast<MultipleSequenceAlignmentObject*>(mobjs.first());
        replaceLogo(mobj->getMultipleAlignment());
    } else {
        mobjs = doc->findGObjectByType(GObjectTypes::SEQUENCE);
        if (mobjs.isEmpty()) {
            reportError(tr("There are no sequences in the file."));
            return;
        }
        U2SequenceObject* dnaObj = qobject_cast<U2SequenceObject*>(mobjs.first());
        MultipleSequenceAlignment ma(dnaObj->getSequenceName(), dnaObj->getAlphabet());
        foreach (GObject* obj, mobjs) {
            U2SequenceObject* dnaObj = qobject_cast<U2SequenceObject*>(obj);
            if (dnaObj->getAlphabet()->getType() != DNAAlphabet_NUCL) {
                ti.setError(  tr("Wrong sequence alphabet") );
            }
            U2OpStatus2Log os;
            QByteArray seqData = dnaObj->getWholeSequenceData(os);
            CHECK_OP_EXT(os, reportError(os.getError()), );
            ma->addRow(dnaObj->getSequenceName(), seqData);
        }
        replaceLogo(ma);
    }
}

void PWMBuildDialogController::sl_formatChanged(const QString &newFormatId) {
    if (WeightMatrixIO::FREQUENCY_MATRIX_ID == newFormatId) {
        frequencyButton->setChecked(true);
    } else {
        weightButton->setChecked(true);
    }
}

void PWMBuildDialogController::reportError(const QString &message) {
    QMessageBox::warning(this, L10N::errorTitle(), message);
}

void PWMBuildDialogController::replaceLogo(const MultipleSequenceAlignment& ma) {
    if (ma->getLength() < 50) {
        static const int LOGO_HEIGHT = 150;
        AlignmentLogoSettings logoSettings(ma);
        logoWidget->resize(logoWidget->width(), LOGO_HEIGHT);
        logoWidget->setMinimumHeight(LOGO_HEIGHT);
        logoWidget->show();

        if (logoArea != NULL) {
            logoArea->replaceSettings(logoSettings);
        } else {
            logoArea = new AlignmentLogoRenderArea(logoSettings, logoWidget);
        }
        logoArea->repaint();
    }
}

void PWMBuildDialogController::sl_matrixTypeChanged(bool matrixType) {
    delete saveController;
    if (matrixType) {
        initWeightSaveController();
    } else {
        initFrequencySaveController();
    }
}

void PWMBuildDialogController::sl_okButtonClicked() {
    if (task != NULL) {
        accept(); //go to background
        return;
    }

    // try prepare model

    PMBuildSettings s;

    QString inFile = inputEdit->text();
    if (inFile.isEmpty() || !QFile::exists(inFile)) {
        statusLabel->setText(tr("Illegal input file name"));
        inputEdit->setFocus();
        return;
    }

    QString outFile = saveController->getSaveFileName();
    if (outFile.isEmpty()) {
        statusLabel->setText(tr("Illegal output file name"));
        outputEdit->setFocus();
        return;
    }

    if (frequencyButton->isChecked()) {
        s.target = FREQUENCY_MATRIX;
    } else {
        s.target = WEIGHT_MATRIX;
    }

    //save settings
    //AppContext::getSettings()->setValue(SETTINGS_ROOT + WEIGHT_ALG, weightAlgCombo->currentIndex());

    if (mononucleicButton->isChecked()) {
        s.type = PM_MONONUCLEOTIDE;
    } else {
        s.type = PM_DINUCLEOTIDE;
    }

    // run task
    if (frequencyButton->isChecked()) {
        task = new PFMatrixBuildToFileTask(inFile, outFile, s);
    } else {
        s.algo = algorithmCombo->currentText();
        task = new PWMatrixBuildToFileTask(inFile, outFile, s);
    }
    connect(task, SIGNAL(si_stateChanged()), SLOT(sl_onStateChanged()));
    connect(task, SIGNAL(si_progressChanged()), SLOT(sl_onProgressChanged()));
    AppContext::getTaskScheduler()->registerTopLevelTask(task);
    statusLabel->setText(tr("Counting frequency statistics"));

    //update buttons
    okButton->setText(tr("Hide"));
    cancelButton->setText(tr("Cancel"));
}

void PWMBuildDialogController::sl_onStateChanged() {
    Task* t = qobject_cast<Task*>(sender());
    assert(task!=NULL);
    if (task != t || t->getState() != Task::State_Finished) {
        return;
    }
    task->disconnect(this);
    const TaskStateInfo& si = task->getStateInfo();
    if (si.hasError()) {
        statusLabel->setText(tr("Build finished with errors: %1").arg(si.getError()));
        lastURL = "";
    } else if (task->isCanceled()) {
        statusLabel->setText(tr("Build canceled"));
        lastURL = "";
    } else {
        statusLabel->setText(tr("Build finished successfuly"));
        lastURL = saveController->getSaveFileName();
    }
    okButton->setText(tr("Start"));
    cancelButton->setText(tr("Close"));
    task = NULL;
}

void PWMBuildDialogController::sl_onProgressChanged() {
    assert(task==sender());
    statusLabel->setText(tr("Running state %1 progress %2%").arg(task->getStateInfo().getDescription()).arg(task->getProgress()));
}

void PWMBuildDialogController::initFrequencySaveController() {
    SaveDocumentControllerConfig config;
    config.defaultDomain = SETTINGS_ROOT + WeightMatrixIO::FREQUENCY_MATRIX_ID;
    config.defaultFormatId = WeightMatrixIO::FREQUENCY_MATRIX_ID;
    config.fileDialogButton = outputButton;
    config.fileNameEdit = outputEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Select file to save frequency matrix to...");

    SaveDocumentController::SimpleFormatsInfo formats;
    formats.addFormat(WeightMatrixIO::FREQUENCY_MATRIX_ID, tr("Frequency matrices"), QStringList() << WeightMatrixIO::FREQUENCY_MATRIX_EXT);
    formats.addFormat(WeightMatrixIO::WEIGHT_MATRIX_ID, tr("Weight matrices"), QStringList() << WeightMatrixIO::WEIGHT_MATRIX_EXT);

    saveController = new SaveDocumentController(config, formats, this);
    connect(saveController, SIGNAL(si_formatChanged(const QString &)), SLOT(sl_formatChanged(const QString &)));
    saveController->setFormat(WeightMatrixIO::FREQUENCY_MATRIX_ID);
}

void PWMBuildDialogController::initWeightSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultDomain = SETTINGS_ROOT + WeightMatrixIO::WEIGHT_MATRIX_ID;
    config.defaultFormatId = WeightMatrixIO::WEIGHT_MATRIX_ID;
    config.fileDialogButton = outputButton;
    config.fileNameEdit = outputEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Select file to save weight matrix to...");

    SaveDocumentController::SimpleFormatsInfo formats;
    formats.addFormat(WeightMatrixIO::FREQUENCY_MATRIX_ID, tr("Frequency matrices"), QStringList() << WeightMatrixIO::FREQUENCY_MATRIX_EXT);
    formats.addFormat(WeightMatrixIO::WEIGHT_MATRIX_ID, tr("Weight matrices"), QStringList() << WeightMatrixIO::WEIGHT_MATRIX_EXT);

    saveController = new SaveDocumentController(config, formats, this);
    connect(saveController, SIGNAL(si_formatChanged(const QString &)), SLOT(sl_formatChanged(const QString &)));
    saveController->setFormat(WeightMatrixIO::WEIGHT_MATRIX_ID);
}

void PWMBuildDialogController::reject() {
    if (task!=NULL) {
        task->cancel();
    }
    if (lastURL != "") {
        QDialog::accept();
    } else {
        QDialog::reject();
    }
}


//////////////////////////////////////////////////////////////////////////
// tasks

PFMatrixBuildTask::PFMatrixBuildTask(const PMBuildSettings& s, const MultipleSequenceAlignment& ma)
: Task (tr("Build Frequency Matrix"), TaskFlag_None), settings(s), ma(ma->getCopy())
{
    GCOUNTER( cvar, tvar, "PFMatrixBuildTask" );
    tpm = Task::Progress_Manual;
}

void PFMatrixBuildTask::run() {
    if (!ma->hasEqualLength()) {
        stateInfo.setError(  tr("Sequences in alignment have various lengths") );
        return;
    }
    if (ma->isEmpty()) {
        stateInfo.setError(  tr("Alignment is empty") );
        return;
    }
    if (!ma->getAlphabet()->isNucleic()) {
        stateInfo.setError(  tr("Alignment is not nucleic") );
        return;
    }
    stateInfo.setDescription( tr("Calculating frequencies of nucleotids") );
    if (settings.type == PM_MONONUCLEOTIDE) {
        m = PFMatrix(ma, PFM_MONONUCLEOTIDE);
    } else {
        m = PFMatrix(ma, PFM_DINUCLEOTIDE);
    }
    stateInfo.progress+=50;
    if (stateInfo.hasError() || isCanceled()) {
        return;
    }
    return;
}

PFMatrixBuildToFileTask::PFMatrixBuildToFileTask(const QString& inFile, const QString& _outFile, const PMBuildSettings& s)
: Task (tr("Build Weight Matrix"), TaskFlag_NoRun), loadTask(NULL), buildTask(NULL), outFile(_outFile), settings(s)
{
    tpm = Task::Progress_SubTasksBased;

    DocumentFormatConstraints c;
    c.checkRawData = true;
    c.supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    c.supportedObjectTypes += GObjectTypes::SEQUENCE;
    c.rawData = IOAdapterUtils::readFileHeader(inFile);
    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(inFile);
    if (formats.isEmpty()) {
        stateInfo.setError(  tr("Input format error") );
        return;
    }

    DocumentFormatId format = "";
    foreach(const FormatDetectionResult& i, formats) {
        if (i.format->getSupportedObjectTypes().contains(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT)) {
            format = i.format->getFormatId();
            break;
        }
    }
    if(format.isEmpty()) {
        foreach(const FormatDetectionResult& i, formats) {
            if (i.format->getSupportedObjectTypes().contains(GObjectTypes::SEQUENCE)) {
                format = i.format->getFormatId();
                break;
            }
        }
    }

    //DocumentFormatId format = formats.first()->getFormatId();
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(inFile));
    loadTask = new LoadDocumentTask(format, inFile, iof);
    loadTask->setSubtaskProgressWeight(0.03F);
    stateInfo.progress = 0;
    stateInfo.setDescription(tr("Loading alignment"));
    addSubTask(loadTask);
}

QList<Task*> PFMatrixBuildToFileTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (isCanceled()) {
        return res;
    }
    if (subTask->getStateInfo().hasError()) {
        stateInfo.setError(  subTask->getStateInfo().getError() );
        return res;
    }
    if (subTask == loadTask) {
        setUseDescriptionFromSubtask(true);
        Document* d = loadTask->getDocument();
        assert(d != NULL);
        QList<GObject*> mobjs = d->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
        if (!mobjs.isEmpty()) {
            MultipleSequenceAlignmentObject* mobj =  qobject_cast<MultipleSequenceAlignmentObject*>(mobjs.first());
            buildTask = new PFMatrixBuildTask(settings, mobj->getMultipleAlignment());
            res.append(buildTask);
        } else {
            mobjs = d->findGObjectByType(GObjectTypes::SEQUENCE);
            if (!mobjs.isEmpty()) {
                U2SequenceObject* dnaObj = qobject_cast<U2SequenceObject*>(mobjs.first());
                QString baseName = d->getURL().baseFileName();
                MultipleSequenceAlignment ma(baseName, dnaObj->getAlphabet());
                foreach (GObject* obj, mobjs) {
                    U2SequenceObject* dnaObj = qobject_cast<U2SequenceObject*>(obj);
                    if (dnaObj->getAlphabet()->getType() != DNAAlphabet_NUCL) {
                        stateInfo.setError(  tr("Wrong sequence alphabet") );
                    }
                    QByteArray seqData = dnaObj->getWholeSequenceData(stateInfo);
                    CHECK_OP(stateInfo, res);
                    ma->addRow(dnaObj->getSequenceName(), seqData);
                }
                buildTask = new PFMatrixBuildTask(settings, ma);
                res.append(buildTask);
            } else {
                stateInfo.setError(  tr("No alignments or sequences found") );
            }
        }
    } else if (subTask == buildTask) {
        Task* t = new PFMatrixWriteTask(outFile, buildTask->getResult());
        t->setSubtaskProgressWeight(0);
        res.append(t);
    }
    return res;
}

PWMatrixBuildTask::PWMatrixBuildTask(const PMBuildSettings& s, const MultipleSequenceAlignment& ma)
: Task (tr("Build Weight Matrix"), TaskFlag_None), settings(s), ma(ma->getCopy())
{
    GCOUNTER( cvar, tvar, "PWMatrixBuildTask" );
    tpm = Task::Progress_Manual;
}

PWMatrixBuildTask::PWMatrixBuildTask(const PMBuildSettings& s, const PFMatrix& ma)
: Task (tr("Build Weight Matrix"), TaskFlag_None), settings(s), tempMatrix(ma)
{
    GCOUNTER( cvar, tvar, "PWMatrixBuildTask" );
    tpm = Task::Progress_Manual;
}

void PWMatrixBuildTask::run() {
    if (tempMatrix.getLength() > 0) {
        if (settings.type == PM_DINUCLEOTIDE && tempMatrix.getType() == PFM_MONONUCLEOTIDE) {
            stateInfo.setError( tr("Can't convert mononucleotide matrix to dinucleotide one"));
            return;
        }
        if (settings.type == PM_MONONUCLEOTIDE && tempMatrix.getType() == PFM_DINUCLEOTIDE) {
            tempMatrix = PFMatrix::convertDi2Mono(tempMatrix);
        }
        stateInfo.progress+=40;
        if (stateInfo.hasError() || isCanceled()) {
            return;
        }
    } else {
        if (!ma->hasEqualLength()) {
            stateInfo.setError(  tr("Sequences in alignment have various lengths") );
            return;
        }
        if (ma->isEmpty()) {
            stateInfo.setError(  tr("Alignment is empty") );
            return;
        }
        if (!ma->getAlphabet()->isNucleic()) {
            stateInfo.setError(  tr("Alignment is not nucleic") );
            return;
        }

        if (settings.type == PM_MONONUCLEOTIDE) {
            tempMatrix = PFMatrix(ma, PFM_MONONUCLEOTIDE);
        } else {
            tempMatrix = PFMatrix(ma, PFM_DINUCLEOTIDE);
        }
        stateInfo.progress+=40;
        if (stateInfo.hasError() || isCanceled()) {
            return;
        }
    }
    PWMConversionAlgorithmFactory* factory = AppContext::getPWMConversionAlgorithmRegistry()->getAlgorithmFactory(settings.algo);
    PWMConversionAlgorithm* algo = factory->createAlgorithm();

    m = algo->convert(tempMatrix);
    stateInfo.progress+=40;
    return;
}

PWMatrixBuildToFileTask::PWMatrixBuildToFileTask(const QString& inFile, const QString& _outFile, const PMBuildSettings& s)
: Task (tr("Build Weight Matrix"), TaskFlag_NoRun), loadTask(NULL), buildTask(NULL), outFile(_outFile), settings(s)
{
    tpm = Task::Progress_SubTasksBased;

    DocumentFormatConstraints c;
    c.checkRawData = true;
    c.supportedObjectTypes += GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT;
    c.supportedObjectTypes += GObjectTypes::SEQUENCE;
    c.rawData = IOAdapterUtils::readFileHeader(inFile);
    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(inFile);
    if (formats.isEmpty()) {
        stateInfo.setError(  tr("Input format error") );
        return;
    }
    DocumentFormatId format = formats.first().format->getFormatId();
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(inFile));
    loadTask = new LoadDocumentTask(format, inFile, iof);
    loadTask->setSubtaskProgressWeight(0.03F);
    stateInfo.progress = 0;
    stateInfo.setDescription(tr("Loading alignment"));
    addSubTask(loadTask);
}

QList<Task*> PWMatrixBuildToFileTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    if (isCanceled()) {
        return res;
    }
    if (subTask->getStateInfo().hasError()) {
        stateInfo.setError(  subTask->getStateInfo().getError() );
        return res;
    }
    if (subTask == loadTask) {
        setUseDescriptionFromSubtask(true);
        Document* d = loadTask->getDocument();
        assert(d != NULL);
        QList<GObject*> mobjs = d->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
        if (!mobjs.isEmpty()) {
            MultipleSequenceAlignmentObject* mobj =  qobject_cast<MultipleSequenceAlignmentObject*>(mobjs.first());
            buildTask = new PWMatrixBuildTask(settings, mobj->getMultipleAlignment());
            res.append(buildTask);
        } else {
            mobjs = d->findGObjectByType(GObjectTypes::SEQUENCE);
            if (!mobjs.isEmpty()) {
                U2SequenceObject* dnaObj = qobject_cast<U2SequenceObject*>(mobjs.first());
                QString baseName = d->getURL().baseFileName();
                MultipleSequenceAlignment ma(baseName, dnaObj->getAlphabet());
                foreach (GObject* obj, mobjs) {
                    U2SequenceObject* dnaObj = qobject_cast<U2SequenceObject*>(obj);
                    if (dnaObj->getAlphabet()->getType() != DNAAlphabet_NUCL) {
                        stateInfo.setError(  tr("Wrong sequence alphabet") );
                    }
                    QByteArray seqData = dnaObj->getWholeSequenceData(stateInfo);
                    CHECK_OP(stateInfo, res);
                    ma->addRow(dnaObj->getSequenceName(), seqData);
                }

                buildTask = new PWMatrixBuildTask(settings, ma);
                res.append(buildTask);
            } else {
                stateInfo.setError(  tr("No alignments or sequences found") );
            }
        }
    } else if (subTask == buildTask) {
        Task* t = new PWMatrixWriteTask(outFile, buildTask->getResult());
        t->setSubtaskProgressWeight(0);
        res.append(t);
    }
    return res;
}

}//namespace
