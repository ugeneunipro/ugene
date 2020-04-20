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

#include <U2Core/AddDocumentTask.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TaskSignalMapper.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/QDScheduler.h>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/OpenViewTask.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/AnnotatedDNAViewTasks.h>

#include "QDRunDialog.h"
#include "QDSceneIOTasks.h"
#include "QueryViewController.h"

//TODO: there are issues with 'docWithSequence' here
// Issue 1: if docWithSequence removed from the project during calc -> crash
// Issue 2: if docWithSequence is loaded and task is failed -> memleak

namespace U2 {

/************************************************************************/
/*  Dialog for run from designer window                                 */
/************************************************************************/

const QString QDRunDialog::OUTPUT_FILE_DIR_DOMAIN = "qd_run_dialog/output_file";

QDRunDialog::QDRunDialog(QDScheme* _scheme, QWidget* parent, const QString& defaultIn, const QString& defaultOut)
    : QDialog(parent),
      scheme(_scheme),
      saveController(NULL)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "28967714");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Run"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    inFileEdit->setText(defaultIn);
    initSaveController(defaultOut);

    connect(tbInFile, SIGNAL(clicked()), SLOT(sl_selectInputFile()));
    connect(outFileEdit, SIGNAL(textChanged(const QString &)), SLOT(sl_outputFileChanged()));
    connect(outFileEdit, SIGNAL(textEdited(const QString &)), SLOT(sl_outputFileChanged()));

    QPushButton* runBtn = buttonBox->button(QDialogButtonBox::Ok);
    connect(runBtn, SIGNAL(clicked()), SLOT(sl_run()));
}

void QDRunDialog::sl_selectInputFile() {
    LastUsedDirHelper dir;
    if (!inFileEdit->text().isEmpty()) {
        QFileInfo fi(inFileEdit->text());
        dir.url = fi.absoluteFilePath();
        dir.dir = fi.absolutePath();
    }

    QString fileFilter = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true);
    dir.url = U2FileDialog::getOpenFileName(this, tr("Select input file"), dir, fileFilter);

    if (!dir.url.isEmpty()) {
        inFileEdit->setText(dir.url);
        QueryViewController* view = qobject_cast<QueryViewController*>(parentWidget());
        SAFE_POINT(NULL != view, "View is NULL", );
        view->setDefaultInFile(dir.url);
    }
}

void QDRunDialog::sl_outputFileChanged() {
    QueryViewController* view = qobject_cast<QueryViewController*>(parentWidget());
    SAFE_POINT(NULL != view, "View is NULL", );
    view->setDefaultOutFile(saveController->getSaveFileName());
}

void QDRunDialog::initSaveController(const QString &defaultOut) {
    SaveDocumentControllerConfig config;
    config.defaultDomain = OUTPUT_FILE_DIR_DOMAIN;
    config.defaultFileName = defaultOut;
    config.defaultFormatId = BaseDocumentFormats::PLAIN_GENBANK;
    config.fileDialogButton = tbOutFile;
    config.fileNameEdit = outFileEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Select output file");

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::PLAIN_GENBANK;

    saveController = new SaveDocumentController(config, formats, this);
}

void QDRunDialog::sl_run() {
    const QString inUri = inFileEdit->text();
    const QString outUri = saveController->getSaveFileName();

    if (inUri.isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("The sequence is not specified!"));
        return;
    }
    if (outUri.isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("The output file is not selected!"));
        return;
    }

    QDRunDialogTask* t = new QDRunDialogTask(scheme, inUri, outUri, cbAddToProj->isChecked());
    AppContext::getTaskScheduler()->registerTopLevelTask(t);
    QDialog::accept();
}

/************************************************************************/
/* Task                                                                 */
/************************************************************************/

QDRunDialogTask::QDRunDialogTask(QDScheme* _scheme, const QString& _inUri, const QString& outUri, bool addToProject)
: Task(tr("Query Designer"), TaskFlags_NR_FOSCOE), scheme(_scheme), inUri(_inUri), output(outUri),
addToProject(addToProject), openProjTask(NULL), loadTask(NULL), scheduler(NULL),
docWithSequence(NULL), annObj(NULL)
{
    tpm = Progress_Manual;
    stateInfo.progress = 0;
    if (addToProject && !AppContext::getProject()) {
        openProjTask = AppContext::getProjectLoader()->createNewProjectTask();
        addSubTask(openProjTask);
    } else {
        const QList<Task*>& tasks = init();
        foreach(Task* t, tasks) {
            addSubTask(t);
        }
    }
}

void QDRunDialogTask::sl_updateProgress() {
    assert(scheduler);
    stateInfo.progress = scheduler->getProgress();
}

QList<Task*> QDRunDialogTask::init() {
    QList<Task*> res;
    if (AppContext::getProject() != NULL) {
        docWithSequence = AppContext::getProject()->findDocumentByURL(inUri);
    }

    if (docWithSequence != NULL) {
        if (!docWithSequence->isLoaded()) {
            loadTask = new LoadUnloadedDocumentTask(docWithSequence);
            res.append(loadTask);
        } else {
            setupQuery();
            res.append(scheduler);
        }
    } else {
        IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(inUri));
        assert(iof);
        QList<FormatDetectionResult> dfs = DocumentUtils::detectFormat(inUri);
        if (dfs.isEmpty()) {
            setError(tr(""));
        } else {
            foreach(const FormatDetectionResult& i, dfs){
                if (i.format->getSupportedObjectTypes().contains(GObjectTypes::SEQUENCE)) {
                    loadTask = new LoadDocumentTask(i.format->getFormatId(), inUri, iof);
                    res.append(loadTask);
                    break;
                }
            }
            if (!loadTask) {
                setError(tr("Sequence not found!"));
            }
        }
    }
    return res;
}

void QDRunDialogTask::setupQuery( ) {
    const QList<GObject*>& objs = docWithSequence->findGObjectByType(GObjectTypes::SEQUENCE);
    CHECK_EXT(!objs.isEmpty(), setError(tr("Sequence not found, document: %1").arg(docWithSequence->getURLString())), );

    U2SequenceObject* seqObj = qobject_cast<U2SequenceObject*>(objs.first());
    DNASequence sequence = seqObj->getWholeSequence(stateInfo);
    CHECK_OP(stateInfo, );
    scheme->setSequence(sequence);
    scheme->setEntityRef(seqObj->getEntityRef());
    QDRunSettings settings;
    settings.region = U2Region(0, seqObj->getSequenceLength());
    settings.scheme = scheme;
    settings.dnaSequence = sequence;
    settings.annotationsObj = new AnnotationTableObject(
        GObjectTypes::getTypeInfo( GObjectTypes::ANNOTATION_TABLE ).name, docWithSequence->getDbiRef( ) );
    settings.annotationsObj->addObjectRelation(seqObj, ObjectRole_Sequence);
    scheduler = new QDScheduler(settings);
    connect(scheduler, SIGNAL(si_progressChanged()), SLOT(sl_updateProgress()));
}

QList<Task*> QDRunDialogTask::onSubTaskFinished(Task* subTask) {
    QList<Task*> res;
    CHECK_OP(stateInfo, res);

    if (subTask == openProjTask) {
        res << init();
    } else if (subTask == loadTask) {
        if (docWithSequence == NULL) {
            docWithSequence = loadTask->takeDocument();
        }
        setupQuery();
        res.append(scheduler);
    } else if (subTask == scheduler) {
        DocumentFormatRegistry* dfr = AppContext::getDocumentFormatRegistry();
        DocumentFormat* df = dfr->getFormatById(BaseDocumentFormats::PLAIN_GENBANK);

        IOAdapterRegistry* ior = AppContext::getIOAdapterRegistry();
        IOAdapterFactory* io = ior->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);

        GUrl url(output, GUrl_File);
        Document* docWithAnnotations = df->createNewLoadedDocument(io, url, stateInfo);
        CHECK_OP(stateInfo, res);
        docWithAnnotations->addObject(scheduler->getSettings().annotationsObj);

        Project* proj = AppContext::getProject();
        if (!addToProject) {
            scheme->setSequence(DNASequence());
            scheme->setEntityRef(U2EntityRef());
            SaveDocumentTask* saveTask = new SaveDocumentTask(docWithAnnotations, SaveDoc_DestroyAfter, QSet<QString>());
            res.append(saveTask);
        } else {
            SAFE_POINT(proj != NULL, "Project is null", res);
            Document* sameUrlDoc = proj->findDocumentByURL(url);
            if (sameUrlDoc) {
                proj->removeDocument(sameUrlDoc);
            }
            res.append(new SaveDocumentTask(docWithAnnotations));
            res.append(new AddDocumentTask(docWithAnnotations));
            SAFE_POINT(docWithSequence , "Document is NULL", res);
            SAFE_POINT(docWithSequence->isLoaded(), "Document is not loaded", res);
            if (proj->getDocuments().contains(docWithSequence)) {
                res.append(new OpenViewTask(docWithSequence));
            } else {
                res.append(new AddDocumentTask(docWithSequence));
                res.append(new OpenViewTask(docWithSequence));
            }
        }
    }
    return res;
}

/************************************************************************/
/* Dialog for dna view context menu                                     */
/************************************************************************/

QDDialog::QDDialog(ADVSequenceObjectContext* _ctx)
: QDialog(_ctx->getAnnotatedDNAView()->getWidget()), ctx(_ctx), scheme(NULL), txtDoc(NULL) {
    setupUi(this);
    new HelpButton(this, buttonBox, "28967717");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Search"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    rs=new RegionSelector(this, ctx->getSequenceLength(), false, ctx->getSequenceSelection());
    rangeSelectorLayout->addWidget(rs);

    addAnnotationsWidget();
    connectGUI();

}

void QDDialog::addAnnotationsWidget() {
    U2SequenceObject *dnaso = qobject_cast<U2SequenceObject*>(ctx->getSequenceGObject());
    CreateAnnotationModel acm;
    acm.sequenceObjectRef = GObjectReference(dnaso);
    acm.hideAnnotationType = true;
    acm.hideAnnotationName = true;
    acm.hideLocation = true;
    acm.data->name = "Query_results";
    acm.useUnloadedObjects = true;
    acm.sequenceLen = dnaso->getSequenceLength();
    cawc = new CreateAnnotationWidgetController(acm, this);
    QWidget* caw = cawc->getWidget();
    QVBoxLayout* l = new QVBoxLayout();
    l->setMargin(0);
    l->addWidget(caw);
    l->addStretch(1);
    annotationsWidget->setLayout(l);
    annotationsWidget->setMinimumSize(caw->layout()->minimumSize());
}

void QDDialog::connectGUI() {
    connect(tbSelectQuery, SIGNAL(clicked()), SLOT(sl_selectScheme()));

    QPushButton* okBtn = buttonBox->button(QDialogButtonBox::Ok);
    connect(okBtn, SIGNAL(clicked()), SLOT(sl_okBtnClicked()));
}

void QDDialog::sl_selectScheme() {
    delete scheme;
    scheme = NULL;
    LastUsedDirHelper dir(QUERY_DESIGNER_ID);
    dir.url = U2FileDialog::getOpenFileName(this, tr("Select query"), dir, QString("*.%1").arg(QUERY_SCHEME_EXTENSION));
    if (dir.url.isEmpty()) {
        return;
    }
    queryFileEdit->setText(dir.url);

    QDDocument doc;
    QFile f(dir.url);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    QByteArray data = f.readAll();
    const QString& content = QString::fromUtf8(data);
    f.close();
    bool res = doc.setContent(content);
    if (!res) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Can not load %1").arg(dir.url));
        return;
    }

    QueryScene scene;
    QList<QDDocument*> docs = (QList<QDDocument*>() << &doc);
    if(!QDSceneSerializer::doc2scene(&scene, docs)) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Can not load %1").arg(dir.url));
        return;
    }

    scheme = new QDScheme;
    QDSceneSerializer::doc2scheme(docs, scheme);

    QPixmap pixmap = QDUtils::generateSnapShot(&doc, QRect());
    QIcon icon(pixmap);
    hintEdit->clear();

    txtDoc = new QTextDocument(hintEdit);
    QString html =  "<html>"
        "<div align=\"center\">"
        "<img src=\"%1\"/>"
        "</div>"
        "</html>";
    QString img("img://img");
    html = html.arg(img);
    qreal h = pixmap.height()*hintEdit->width()/pixmap.width();
    qreal w = hintEdit->width();
    txtDoc->addResource(QTextDocument::ImageResource, QUrl(img), icon.pixmap(w, h));

    txtDoc->setHtml(html);

    hintEdit->setDocument(txtDoc);
}

void QDDialog::sl_okBtnClicked() {
    if (!scheme) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("File with query is not selected!"));
        return;
    }

    if (!scheme->isValid()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Error in schema!"));
        return;
    }

    QString err = cawc->validate();
    if (!err.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), err);
        return;
    }

    bool isRegionOk=false;
    rs->getRegion(&isRegionOk);
    if (!isRegionOk){
        rs->showErrorMessage();
        return;
    }
    bool objectPrepared = cawc->prepareAnnotationObject();
    if (!objectPrepared){
        QMessageBox::warning(this, tr("Error"), tr("Cannot create an annotation object. Please check settings"));
        return;
    }
    const CreateAnnotationModel& m = cawc->getModel();

    U2SequenceObject *seqObj = ctx->getSequenceObject();
    SAFE_POINT(NULL != seqObj, "NULL sequence object", );
    U2OpStatusImpl os;
    DNASequence sequence = seqObj->getWholeSequence(os);
    CHECK_OP_EXT(os, QMessageBox::critical(this, L10N::errorTitle(), os.getError()), );
    scheme->setSequence(sequence);
    scheme->setEntityRef(seqObj->getSequenceRef());
    QDRunSettings settings;
    GObject* ao = GObjectUtils::selectObjectByReference(m.annotationObjectRef, UOF_LoadedOnly);
    settings.annotationsObj = qobject_cast<AnnotationTableObject *>(ao);
    settings.annotationsObjRef = m.annotationObjectRef;
    settings.groupName = m.groupName;
    settings.annDescription = m.description;
    settings.scheme = scheme;
    settings.dnaSequence = sequence;
    settings.viewName = ctx->getAnnotatedDNAView()->getName();
    settings.region = rs->getRegion();

    QDScheduler* t = new QDScheduler(settings);
    AppContext::getTaskScheduler()->registerTopLevelTask(t);

    QDDialog::accept();
}

}//namespace
