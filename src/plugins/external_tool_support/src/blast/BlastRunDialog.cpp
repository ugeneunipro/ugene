/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "BlastRunDialog.h"

#include <QMainWindow>
#include <QMessageBox>
#include <QToolButton>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/AppResources.h>
#include <U2Core/AppSettings.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/ProjectService.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/AppSettingsGUI.h>
#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/DialogUtils.h>
#include <U2Gui/GUIUtils.h>
#include <U2Gui/OpenViewTask.h>
#include <U2Gui/RegionSelector.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "BlastSupport.h"
#include "ExternalToolSupportSettingsController.h"

namespace U2 {

namespace {
QStringList getCompValues() {
    QStringList result;
    result << "blastp";
    result << "blastx";
    result << "tblastn";
    return result;
}
}  // namespace

////////////////////////////////////////
// BlastAllSupportRunDialog
BlastRunDialog::BlastRunDialog(ADVSequenceObjectContext *seqCtx, QString &lastDBPath, QString &lastDBName, QWidget *parent)
    : BlastRunCommonDialog(parent, true, getCompValues()), lastDBPath(lastDBPath), lastDBName(lastDBName), seqCtx(seqCtx), regionSelector(nullptr) {
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

    settings.isNucleotideSeq = dnaso->getAlphabet()->getType() != DNAAlphabet_AMINO;
    QStringList programsToKeep = settings.isNucleotideSeq
                                     ? QStringList({"blastn", "blastx", "tblastx"})
                                     : QStringList({"blastp", "tblastn"});
    for (int i = programName->count(); --i >= 0;) {
        if (!programsToKeep.contains(programName->itemText(i))) {
            programName->removeItem(i);
        }
    }
    dbSelector->databasePathLineEdit->setText(lastDBPath);
    dbSelector->baseNameLineEdit->setText(lastDBName);
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
}

U2Region BlastRunDialog::getSelectedRegion() const {
    return regionSelector->isWholeSequenceSelected() ? U2Region(0, seqCtx->getSequenceLength()) : regionSelector->getRegion();
}

void BlastRunDialog::sl_lineEditChanged() {
    okButton->setEnabled(dbSelector->isInputDataValid());
}

bool BlastRunDialog::checkToolPath() {
    QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
    msgBox->setWindowTitle(tr("BLAST Search"));
    msgBox->setInformativeText(tr("Do you want to select it now?"));
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::Yes);
    QString programText = programName->currentText();
    QString toolName;
    QString toolId;
    if (programText == "blastn") {
        toolName = BlastSupport::ET_BLASTN;
        toolId = BlastSupport::ET_BLASTN_ID;
    } else if (programText == "blastp") {
        toolName = BlastSupport::ET_BLASTP;
        toolId = BlastSupport::ET_BLASTP_ID;
    } else if (programText == "blastx") {
        toolName = BlastSupport::ET_BLASTX;
        toolId = BlastSupport::ET_BLASTX_ID;
    } else if (programText == "tblastn") {
        toolName = BlastSupport::ET_TBLASTN;
        toolId = BlastSupport::ET_TBLASTN_ID;
    } else if (programText == "tblastx") {
        toolName = BlastSupport::ET_TBLASTX;
        toolId = BlastSupport::ET_TBLASTX_ID;
    } else {
        FAIL("Unexpected blast tool name: " + programText, false);
    }

    ExternalToolRegistry *toolsRegistry = AppContext::getExternalToolRegistry();
    if (!toolsRegistry->getById(toolId)->getPath().isEmpty()) {
        return true;
    }

    msgBox->setText(tr("Path for <i>BLAST %1</i> tool not selected.").arg(toolName));
    int ret = msgBox->exec();
    CHECK(!msgBox.isNull(), false);
    CHECK(ret == QMessageBox::Yes, false);
    AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
    return !toolsRegistry->getById(toolId)->getPath().isEmpty();
}

void BlastRunDialog::sl_runQuery() {
    if (!checkToolPath()) {
        return;
    }

    QString error = ca_c->validate();
    if (!error.isEmpty()) {
        QMessageBox::critical(nullptr, tr("Wrong parameters for creating annotations"), error);
        return;
    }
    settings.outputResFile = ca_c->getModel().newDocUrl;
    if (ca_c->isNewObject()) {
        U2OpStatusImpl os;
        const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
        SAFE_POINT_OP(os, );
        settings.aobj = new AnnotationTableObject("Annotations", dbiRef);
        settings.aobj->addObjectRelation(GObjectRelation(ca_c->getModel().sequenceObjectRef, ObjectRole_Sequence));
    } else {
        bool objectPrepared = ca_c->prepareAnnotationObject();
        if (!objectPrepared) {
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

    getSettings(settings);
    settings.alphabet = dnaso->getAlphabet();
    lastDBPath = dbSelector->databasePathLineEdit->text();
    lastDBName = dbSelector->baseNameLineEdit->text();
    settings.outputType = 5;  // By default set output file format to xml
    if (seqCtx != nullptr) {
        seqCtx->getAnnotatedDNAView()->tryAddObject(settings.aobj);
    }
    accept();
}
////////////////////////////////////////
// BlastWithExtFileSpecifySupportRunDialog
BlastWithExtFileSpecifySupportRunDialog::BlastWithExtFileSpecifySupportRunDialog(QString &lastDBPath, QString &lastDBName, QWidget *parent)
    : BlastRunCommonDialog(parent, true, getCompValues()), lastDBPath(lastDBPath), lastDBName(lastDBName), hasValidInput(false) {
    ca_c = nullptr;
    wasNoOpenProject = false;
    // create input file widget
    QWidget *widget = new QWidget(parent);
    inputFileLineEdit = new FileLineEdit("", "", false, widget);
    inputFileLineEdit->setReadOnly(true);
    inputFileLineEdit->setText("");
    QToolButton *selectToolPathButton = new QToolButton(widget);
    selectToolPathButton->setObjectName("browseInput");
    selectToolPathButton->setVisible(true);
    selectToolPathButton->setText("...");
    connect(selectToolPathButton, SIGNAL(clicked()), inputFileLineEdit, SLOT(sl_onBrowse()));
    connect(inputFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(sl_inputFileLineEditChanged(QString)));

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    layout->addWidget(inputFileLineEdit);
    layout->addWidget(selectToolPathButton);

    QGroupBox *inputFileGroupBox = new QGroupBox(tr("Select input file"), widget);
    inputFileGroupBox->setLayout(layout);
    QBoxLayout *parentLayout = qobject_cast<QBoxLayout *>(this->layout());
    assert(parentLayout);
    parentLayout->insertWidget(0, inputFileGroupBox);

    programName->removeItem(3);  // cuda-blastp

    dbSelector->databasePathLineEdit->setText(lastDBPath);
    dbSelector->baseNameLineEdit->setText(lastDBName);
    connect(cancelButton, SIGNAL(clicked()), SLOT(sl_cancel()));
    connect(this, SIGNAL(rejected()), SLOT(sl_cancel()));
}

const QList<BlastTaskSettings> &BlastWithExtFileSpecifySupportRunDialog::getSettingsList() const {
    return settingsList;
}

void BlastWithExtFileSpecifySupportRunDialog::sl_lineEditChanged() {
    okButton->setEnabled(dbSelector->isInputDataValid() && hasValidInput);
}

namespace {
const char *INPUT_URL_PROP = "input_url";
}

void BlastWithExtFileSpecifySupportRunDialog::sl_inputFileLineEditChanged(const QString &url) {
    hasValidInput = false;
    sl_lineEditChanged();
    CHECK(!url.isEmpty(), );

    Project *proj = AppContext::getProject();
    if (nullptr == proj) {
        wasNoOpenProject = true;
    } else {
        Document *doc = proj->findDocumentByURL(url);
        if (doc != nullptr) {
            if (doc->isLoaded()) {
                tryApplyDoc(doc);
            } else {
                LoadUnloadedDocumentAndOpenViewTask *loadTask = new LoadUnloadedDocumentAndOpenViewTask(doc);
                loadTask->setProperty(INPUT_URL_PROP, url);
                connect(loadTask, SIGNAL(si_stateChanged()), SLOT(sl_inputFileOpened()));
                AppContext::getTaskScheduler()->registerTopLevelTask(loadTask);
            }
            return;
        }
    }

    loadDoc(url);
}

void BlastWithExtFileSpecifySupportRunDialog::onFormatError() {
    QMessageBox::critical(this, tr("Wrong input file"), tr("This file has the incompatible format for the BLAST search."));
    inputFileLineEdit->setText("");
}

void BlastWithExtFileSpecifySupportRunDialog::loadDoc(const QString &url) {
    FormatDetectionConfig config;
    config.useExtensionBonus = true;
    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(url, config);
    CHECK_EXT(!formats.isEmpty() && (nullptr != formats.first().format), onFormatError(), );

    DocumentFormat *format = formats.first().format;
    CHECK_EXT(format->getSupportedObjectTypes().contains(GObjectTypes::SEQUENCE), onFormatError(), );

    LoadDocumentTask *loadTask = new LoadDocumentTask(format->getFormatId(), url, AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url)));
    AddDocumentAndOpenViewTask *openTask = new AddDocumentAndOpenViewTask(loadTask);
    openTask->setProperty(INPUT_URL_PROP, url);

    connect(openTask, SIGNAL(si_stateChanged()), SLOT(sl_inputFileOpened()));
    AppContext::getTaskScheduler()->registerTopLevelTask(openTask);
}

void BlastWithExtFileSpecifySupportRunDialog::sl_inputFileOpened() {
    Task *t = qobject_cast<Task *>(sender());
    CHECK(nullptr != t, );
    CHECK(t->isFinished() && !t->hasError(), );

    Project *proj = AppContext::getProject();
    SAFE_POINT(nullptr != proj, "No opened project", );

    QString url = t->property(INPUT_URL_PROP).toString();
    Document *doc = proj->findDocumentByURL(url);
    SAFE_POINT(nullptr != doc, "No loaded document", );

    tryApplyDoc(doc);
}

void BlastWithExtFileSpecifySupportRunDialog::tryApplyDoc(Document *doc) {
    int numOfSequences = 0;
    foreach (GObject *obj, doc->getObjects()) {
        if (obj->getGObjectType() == GObjectTypes::SEQUENCE) {
            numOfSequences++;
        }
    }
    settingsList.clear();
    sequencesRefList.clear();
    if (0 == numOfSequences) {
        QMessageBox::critical(this, tr("Wrong input file"), tr("This file does not contain sequences."));
        inputFileLineEdit->setText("");
        return;
    }

    hasValidInput = true;
    foreach (GObject *obj, doc->getObjects()) {
        if (obj->getGObjectType() != GObjectTypes::SEQUENCE) {
            continue;
        }
        U2SequenceObject *seq = dynamic_cast<U2SequenceObject *>(obj);
        SAFE_POINT(nullptr != seq, "NULL sequence object", );

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
    settings = settingsList.first();

    CreateAnnotationModel ca_m;
    ca_m.hideAnnotationType = true;
    ca_m.hideAnnotationName = true;
    ca_m.hideLocation = true;
    ca_m.sequenceObjectRef = sequencesRefList[0];
    ca_m.sequenceLen = 10;
    ca_m.defaultIsNewDoc = true;
    if (nullptr == ca_c) {
        ca_c = new CreateAnnotationWidgetController(ca_m, this);
        annotationWidgetLayout->addWidget(ca_c->getWidget());
    } else {
        ca_c->updateWidgetForAnnotationModel(ca_m);
    }

    sl_lineEditChanged();
}

bool BlastWithExtFileSpecifySupportRunDialog::checkToolPath() {
    bool needSetToolPath = false;
    QString toolId;
    QObjectScopedPointer<QMessageBox> msgBox = new QMessageBox;
    msgBox->setWindowTitle("BLAST Search");
    msgBox->setInformativeText(tr("Do you want to select it now?"));
    msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox->setDefaultButton(QMessageBox::Yes);
    if ((programName->currentText() == "blastn") &&
        (AppContext::getExternalToolRegistry()->getById(BlastSupport::ET_TBLASTN_ID)->getPath().isEmpty())) {
        needSetToolPath = true;
        toolId = BlastSupport::ET_BLASTN_ID;

    } else if ((programName->currentText() == "blastp") &&
               (AppContext::getExternalToolRegistry()->getById(BlastSupport::ET_BLASTP_ID)->getPath().isEmpty())) {
        needSetToolPath = true;
        toolId = BlastSupport::ET_BLASTP_ID;

    } else if ((programName->currentText() == "blastx") &&
               (AppContext::getExternalToolRegistry()->getById(BlastSupport::ET_BLASTX_ID)->getPath().isEmpty())) {
        needSetToolPath = true;
        toolId = BlastSupport::ET_BLASTX_ID;

    } else if ((programName->currentText() == "tblastn") &&
               (AppContext::getExternalToolRegistry()->getById(BlastSupport::ET_TBLASTN_ID)->getPath().isEmpty())) {
        needSetToolPath = true;
        toolId = BlastSupport::ET_TBLASTN_ID;

    } else if ((programName->currentText() == "tblastx") &&
               (AppContext::getExternalToolRegistry()->getById(BlastSupport::ET_TBLASTX_ID)->getPath().isEmpty())) {
        needSetToolPath = true;
        toolId = BlastSupport::ET_TBLASTX_ID;
    }
    if (needSetToolPath) {
        msgBox->setText(tr("Path for <i>BLAST %1</i> tool not selected.").arg(AppContext::getExternalToolRegistry()->getById(toolId)->getName()));
        const int ret = msgBox->exec();
        CHECK(!msgBox.isNull(), false);

        switch (ret) {
            case QMessageBox::Yes:
                AppContext::getAppSettingsGUI()->showSettingsDialog(ExternalToolSupportSettingsPageId);
                break;
            case QMessageBox::No:
                return false;
                break;
            default:
                assert(false);
                break;
        }
        if (!AppContext::getExternalToolRegistry()->getById(toolId)->getPath().isEmpty()) {
            return true;
        } else {
            return false;
        }

    } else {
        return true;
    }
}

void BlastWithExtFileSpecifySupportRunDialog::sl_runQuery() {
    if (!checkToolPath()) {
        return;
    }

    QString error = ca_c->validate();
    if (!error.isEmpty()) {
        QMessageBox::critical(nullptr, tr("Wrong parameters for creating annotations"), error);
        return;
    }

    for (int i = 0; i < settingsList.length(); i++) {
        settingsList[i].outputResFile = ca_c->getModel().newDocUrl;
        if (ca_c->isNewObject()) {
            U2OpStatusImpl os;
            const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(os);
            SAFE_POINT_OP(os, );
            settingsList[i].aobj = new AnnotationTableObject(sequencesRefList[i].objName + " annotations", dbiRef);
            settingsList[i].aobj->addObjectRelation(GObjectRelation(sequencesRefList[i], ObjectRole_Sequence));
        } else {
            assert(false);  // always created new document for annotations
        }
        settingsList[i].groupName = ca_c->getModel().groupName;

        getSettings(settingsList[i]);
        settingsList[i].outputType = 5;  // By default set output file format to xml
    }
    bool docAlreadyInProject = false;
    Project *proj = AppContext::getProject();
    foreach (Document *doc, proj->getDocuments()) {
        if (doc->getURL() == inputFileLineEdit->text()) {
            docAlreadyInProject = true;
        }
    }
    if (!docAlreadyInProject) {
        QString url = inputFileLineEdit->text();
        Task *t = AppContext::getProjectLoader()->openWithProjectTask(url);
        if (t != nullptr) {
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
void BlastWithExtFileSpecifySupportRunDialog::sl_cancel() {
    if (qobject_cast<BlastWithExtFileSpecifySupportRunDialog *>(sender()) == nullptr) {
        reject();
        return;
    }
    if (wasNoOpenProject) {
        ProjectService *projService = AppContext::getProjectService();
        CHECK(nullptr != projService, );
        AppContext::getTaskScheduler()->registerTopLevelTask(projService->closeProjectTask());
    }
}
}  // namespace U2
