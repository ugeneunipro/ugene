/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/ProjectService.h>
#include <U2Core/Settings.h>
#include <U2Core/U2AlphabetUtils.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Core/U2OpStatusUtils.h>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/OpenViewTask.h>
#include <U2Gui/RegionSelector.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "ExternalToolSupportSettingsController.h"

namespace U2 {

////////////////////////////////////////
// BlastRunDialog
BlastRunDialog::BlastRunDialog(ADVSequenceObjectContext* _seqCtx, QWidget* parent)
    : BlastRunCommonDialog(parent, _seqCtx->getAlphabet()), seqCtx(_seqCtx) {
    sequenceObject = seqCtx->getSequenceObject();
    CreateAnnotationModel ca_m;
    ca_m.hideAnnotationType = true;
    ca_m.hideAnnotationName = true;
    ca_m.hideLocation = true;
    ca_m.sequenceObjectRef = GObjectReference(sequenceObject);
    ca_m.sequenceLen = sequenceObject->getSequenceLength();
    ca_c = new CreateAnnotationWidgetController(ca_m, this);
    annotationWidgetLayout->addWidget(ca_c->getWidget());

    int lastRow = settingsGridLayout->rowCount();
    regionSelector = new RegionSelector(this, seqCtx->getSequenceLength(), false, seqCtx->getSequenceSelection());
    settingsGridLayout->addWidget(regionSelector, lastRow, 0, 1, 3);

    settings.isNucleotideSeq = sequenceObject->getAlphabet()->getType() != DNAAlphabet_AMINO;
    connect(cancelButton, SIGNAL(clicked()), SLOT(reject()));
}

U2Region BlastRunDialog::getSelectedRegion() const {
    return regionSelector->isWholeSequenceSelected() ? U2Region(0, seqCtx->getSequenceLength()) : regionSelector->getRegion();
}

void BlastRunDialog::sl_lineEditChanged() {
    okButton->setEnabled(dbSelector->isInputDataValid());
}

void BlastRunDialog::sl_runQuery() {
    CHECK(checkSelectedToolPath(), );

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
            QMessageBox::warning(this, L10N::errorTitle(), tr("Cannot create an annotation object. Please check settings"));
            return;
        }
        settings.aobj = ca_c->getModel().getAnnotationObject();
    }
    CHECK(dbSelector->validateDatabaseDir(), );
    settings.groupName = ca_c->getModel().groupName;
    settings.annDescription = ca_c->getModel().description;

    getSettings(settings);
    settings.alphabet = sequenceObject->getAlphabet();
    settings.outputType = 5;  // By default set output file format to xml
    if (seqCtx != nullptr) {
        seqCtx->getAnnotatedDNAView()->tryAddObject(settings.aobj);
    }
    accept();
}

////////////////////////////////////////
// BlastWithExtFileRunDialog
BlastWithExtFileRunDialog::BlastWithExtFileRunDialog(QWidget* parent)
    : BlastRunCommonDialog(parent, nullptr) {
    AppContext::getSettings()->setValue(OpenViewTask::IGNORE_MODAL_WIDGET, true);
    // Create input file widget.
    auto widget = new QWidget(parent);
    inputFileLineEdit = new FileLineEdit("", "", false, widget);
    inputFileLineEdit->setReadOnly(true);
    inputFileLineEdit->setText("");
    inputFileLineEdit->setObjectName("inputFileLineEdit");

    auto selectToolPathButton = new QToolButton(widget);
    selectToolPathButton->setObjectName("browseInput");
    selectToolPathButton->setVisible(true);
    selectToolPathButton->setText("...");
    connect(selectToolPathButton, SIGNAL(clicked()), inputFileLineEdit, SLOT(sl_onBrowse()));
    connect(inputFileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(sl_inputFileLineEditChanged(QString)));

    auto layout = new QHBoxLayout(widget);
    layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    layout->addWidget(inputFileLineEdit);
    layout->addWidget(selectToolPathButton);

    auto inputFileGroupBox = new QGroupBox(tr("Select input file"), widget);
    inputFileGroupBox->setLayout(layout);

    auto parentLayout = qobject_cast<QBoxLayout*>(this->layout());
    SAFE_POINT(parentLayout != nullptr, "Not a QBoxLayout!", );
    parentLayout->insertWidget(0, inputFileGroupBox);

    connect(cancelButton, SIGNAL(clicked()), SLOT(sl_cancel()));
    connect(this, SIGNAL(rejected()), SLOT(sl_cancel()));
}

 BlastWithExtFileRunDialog::~BlastWithExtFileRunDialog() {
    AppContext::getSettings()->setValue(OpenViewTask::IGNORE_MODAL_WIDGET, false);
}

const QList<BlastTaskSettings>& BlastWithExtFileRunDialog::getSettingsList() const {
    return settingsList;
}

void BlastWithExtFileRunDialog::sl_lineEditChanged() {
    okButton->setEnabled(dbSelector->isInputDataValid() && hasValidInput);
}

static const char* INPUT_URL_PROP = "input_url";

void BlastWithExtFileRunDialog::sl_inputFileLineEditChanged(const QString& url) {
    hasValidInput = false;
    sl_lineEditChanged();
    CHECK(!url.isEmpty(), );

    Project* proj = AppContext::getProject();
    if (proj == nullptr) {
        wasNoOpenProject = true;
    } else {
        Document* doc = proj->findDocumentByURL(url);
        if (doc != nullptr) {
            if (doc->isLoaded()) {
                tryApplyDoc(doc);
            } else {
                LoadUnloadedDocumentAndOpenViewTask* loadTask = new LoadUnloadedDocumentAndOpenViewTask(doc);
                loadTask->setProperty(INPUT_URL_PROP, url);
                connect(loadTask, SIGNAL(si_stateChanged()), SLOT(sl_inputFileOpened()));
                AppContext::getTaskScheduler()->registerTopLevelTask(loadTask);
            }
            return;
        }
    }

    loadDoc(url);
}

void BlastWithExtFileRunDialog::onFormatError() {
    QMessageBox::critical(this, tr("Wrong input file"), tr("This file has the incompatible format for the BLAST search."));
    inputFileLineEdit->setText("");
}

void BlastWithExtFileRunDialog::loadDoc(const QString& url) {
    FormatDetectionConfig config;
    config.useExtensionBonus = true;
    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(url, config);
    CHECK_EXT(!formats.isEmpty() && (nullptr != formats.first().format), onFormatError(), );

    DocumentFormat* format = formats.first().format;
    CHECK_EXT(format->getSupportedObjectTypes().contains(GObjectTypes::SEQUENCE), onFormatError(), );

    auto loadTask = new LoadDocumentTask(format->getFormatId(), url, AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url)));
    auto openTask = new AddDocumentAndOpenViewTask(loadTask);
    openTask->setProperty(INPUT_URL_PROP, url);

    connect(openTask, SIGNAL(si_stateChanged()), SLOT(sl_inputFileOpened()));
    AppContext::getTaskScheduler()->registerTopLevelTask(openTask);
}

void BlastWithExtFileRunDialog::sl_inputFileOpened() {
    auto task = qobject_cast<Task*>(sender());
    CHECK(task != nullptr, );
    CHECK(task->isFinished() && !task->hasError(), );

    Project* proj = AppContext::getProject();
    SAFE_POINT(proj != nullptr, "No opened project", );

    QString url = task->property(INPUT_URL_PROP).toString();
    Document* doc = proj->findDocumentByURL(url);
    SAFE_POINT(doc != nullptr, "No loaded document", );

    tryApplyDoc(doc);
}

void BlastWithExtFileRunDialog::tryApplyDoc(Document* doc) {
    int numOfSequences = 0;
    foreach (GObject* obj, doc->getObjects()) {
        if (obj->getGObjectType() == GObjectTypes::SEQUENCE) {
            numOfSequences++;
        }
    }
    settingsList.clear();
    sequencesRefList.clear();
    if (numOfSequences == 0) {
        QMessageBox::critical(this, tr("Wrong input file"), tr("This file does not contain sequences."));
        inputFileLineEdit->setText("");
        return;
    }

    hasValidInput = true;
    const DNAAlphabet* commonAlphabet = nullptr;
    foreach (GObject* obj, doc->getObjects()) {
        if (obj->getGObjectType() != GObjectTypes::SEQUENCE) {
            continue;
        }
        U2SequenceObject* seq = dynamic_cast<U2SequenceObject*>(obj);
        SAFE_POINT(seq != nullptr, "NULL sequence object", );

        BlastTaskSettings localSettings;
        U2OpStatusImpl os;
        localSettings.querySequences = {seq->getWholeSequenceData(os)};
        CHECK_OP_EXT(os, QMessageBox::critical(this, L10N::errorTitle(), os.getError()), );
        localSettings.alphabet = seq->getAlphabet();
        commonAlphabet = commonAlphabet == nullptr ? localSettings.alphabet
                                                   : U2AlphabetUtils::deriveCommonAlphabet(localSettings.alphabet, commonAlphabet);
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
    if (ca_c == nullptr) {
        ca_c = new CreateAnnotationWidgetController(ca_m, this);
        annotationWidgetLayout->addWidget(ca_c->getWidget());
    } else {
        ca_c->updateWidgetForAnnotationModel(ca_m);
    }
    updateAvailableProgramsList(commonAlphabet);

    sl_lineEditChanged();
}

void BlastWithExtFileRunDialog::sl_runQuery() {
    if (!checkSelectedToolPath()) {
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
    Project* proj = AppContext::getProject();
    foreach (Document* doc, proj->getDocuments()) {
        if (doc->getURL() == inputFileLineEdit->text()) {
            docAlreadyInProject = true;
        }
    }
    if (!docAlreadyInProject) {
        QString url = inputFileLineEdit->text();
        Task* t = AppContext::getProjectLoader()->openWithProjectTask(url);
        if (t != nullptr) {
            AppContext::getTaskScheduler()->registerTopLevelTask(t);
        }
    }
    if (!dbSelector->validateDatabaseDir()) {
        return;
    }
    accept();
}

void BlastWithExtFileRunDialog::sl_cancel() {
    if (qobject_cast<BlastWithExtFileRunDialog*>(sender()) == nullptr) {
        reject();
        return;
    }
    if (wasNoOpenProject) {
        ProjectService* projService = AppContext::getProjectService();
        CHECK(projService != nullptr, );
        AppContext::getTaskScheduler()->registerTopLevelTask(projService->closeProjectTask());
    }
}
}  // namespace U2
