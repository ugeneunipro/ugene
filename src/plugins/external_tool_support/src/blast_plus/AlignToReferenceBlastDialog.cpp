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

#include <QDomDocument>
#include <QMessageBox>
#include <QShortcut>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/CmdlineInOutTaskRunner.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/UserApplicationsSettings.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/OpenViewTask.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include "AlignToReferenceBlastDialog.h"
#include "AlignToReferenceBlastWorker.h"


#define DEFAULT_OUTPUT_FILE_NAME QString("sanger_reads_alignment.ugenedb")

namespace U2 {

AlignToReferenceBlastCmdlineTask::Settings::Settings()
    : minIdentity(60),
      minLength(0),
      qualityThreshold(30),
      rowNaming(SequenceName),
      addResultToProject(true)
{

}

QString AlignToReferenceBlastCmdlineTask::Settings::getRowNamingPolicyString() const {
    switch (rowNaming) {
    case SequenceName:
        return LocalWorkflow::AlignToReferenceBlastWorkerFactory::ROW_NAMING_SEQUENCE_NAME_VALUE;
        break;
    case FileName:
        return LocalWorkflow::AlignToReferenceBlastWorkerFactory::ROW_NAMING_FILE_NAME_VALUE;
        break;
    default:
        FAIL("An unknown row naming policy", LocalWorkflow::AlignToReferenceBlastWorkerFactory::ROW_NAMING_SEQUENCE_NAME_VALUE);
    }
}

const QString AlignToReferenceBlastCmdlineTask::ALIGN_TO_REF_CMDLINE = "align-to-reference";

const QString AlignToReferenceBlastCmdlineTask::TRIM_ARG = "trim-both-ends";
const QString AlignToReferenceBlastCmdlineTask::MIN_LEN_ARG = "min-length";
const QString AlignToReferenceBlastCmdlineTask::THRESHOLD_ARG = "threshold";

const QString AlignToReferenceBlastCmdlineTask::READS_ARG = "reads";

const QString AlignToReferenceBlastCmdlineTask::MIN_IDENTITY_ARG = "min-identity";
const QString AlignToReferenceBlastCmdlineTask::ROW_NAMING_ARG = "row-naming-policy";
const QString AlignToReferenceBlastCmdlineTask::REF_ARG = "reference";
const QString AlignToReferenceBlastCmdlineTask::RESULT_ALIGNMENT_ARG = "result-url";

AlignToReferenceBlastCmdlineTask::AlignToReferenceBlastCmdlineTask(const Settings &settings)
    : Task(tr("Map Sanger reads to reference"), TaskFlags_FOSE_COSC | TaskFlag_MinimizeSubtaskErrorText | TaskFlag_ReportingIsEnabled | TaskFlag_ReportingIsSupported),
      settings(settings),
      cmdlineTask(NULL),
      loadRef(NULL),
      reportFile(AppContext::getAppSettings()->getUserAppsSettings()->getCurrentProcessTemporaryDirPath() + "/align_to_ref_XXXXXX.txt")
{
    GCOUNTER(cvar, tvar, "AlignToReferenceBlastCmdlineTask");
}

void AlignToReferenceBlastCmdlineTask::prepare() {
    AppContext::getAppSettings()->getUserAppsSettings()->createCurrentProcessTemporarySubDir(stateInfo);
    const bool opened = reportFile.open();
    SAFE_POINT_EXT(opened, setError(L10N::errorOpeningFileWrite(reportFile.fileName())), );
    reportFile.close();

    GUrl referenceUrl(settings.referenceUrl);
    if (referenceUrl.isLocalFile()) {
        CHECK_EXT(QFileInfo(referenceUrl.getURLString()).exists(), setError(tr("The '%1' reference file doesn't exist.").arg(settings.referenceUrl)),);
    }

    FormatDetectionConfig config;
    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(referenceUrl, config);
    CHECK_EXT(!formats.isEmpty() && (NULL != formats.first().format), setError(tr("wrong reference format")), );

    DocumentFormat *format = formats.first().format;
    CHECK_EXT(format->getSupportedObjectTypes().contains(GObjectTypes::SEQUENCE), setError(tr("wrong reference format")), );

    loadRef = new LoadDocumentTask(format->getFormatId(),
        settings.referenceUrl, AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(settings.referenceUrl)));
    addSubTask(loadRef);
}

namespace {

static const QString DIV = "div";
static const QString CLASS = "class";
static const QString ID = "id";
static const QString CLASS_WORKER = "worker";
static const QString CLASS_TASK = "task";

QMap<QString, QMultiMap<QString, QString> > splitReports(U2OpStatus &os, const QString &reportString) {
    QMap<QString, QMultiMap<QString, QString> > result;
    QDomDocument document;
    document.setContent("<html><body>" + reportString + "</body></html>");
    const QDomNodeList workersList = document.firstChildElement("html").firstChildElement("body").childNodes();
    for (int i = 0; i < workersList.size(); i++) {
        QDomElement workerElement = workersList.item(i).toElement();

        const QString workerElementClass = workerElement.attribute(CLASS);
        SAFE_POINT_EXT(CLASS_WORKER == workerElementClass, os.setError(QString("An unknown element class: %1").arg(workerElementClass)), result);

        const QDomNodeList tasksList = workerElement.childNodes();
        for (int j = 0; j < tasksList.size(); j++) {
            QDomElement taskElement = tasksList.item(j).toElement();

            const QString taskElementClass = taskElement.attribute(CLASS);
            SAFE_POINT_EXT(CLASS_TASK == taskElementClass, os.setError(QString("An unknown element class: %1").arg(taskElementClass)), result);

            result[workerElement.attribute(ID)].insert(taskElement.attribute(ID), QByteArray::fromBase64(taskElement.text().toUtf8()));
        }
    }
    return result;
}

}

QString AlignToReferenceBlastCmdlineTask::generateReport() const {
    U2OpStatusImpl os;
    QMap<QString, QMultiMap<QString, QString> > reports = splitReports(os, reportString);

    QMultiMap<QString, QString> mapperReports = reports.value(LocalWorkflow::AlignToReferenceBlastWorkerFactory::ACTOR_ID, QMultiMap<QString, QString>());
    CHECK(mapperReports.size() > 0, "");

    QString resultReport = mapperReports.values().first();

    QMultiMap<QString, QString> trimReports = reports.value("SequenceQualityTrim", QMultiMap<QString, QString>());
    if (trimReports.values().size() > 0) {
        resultReport += tr("<u>Filtered by quality (%1):</u>").arg(trimReports.values().size());
        resultReport += "<table>";
    }

    QRegExp readNameExtractor(".*\'(.*)\'.*");
    foreach (const QString &taskReport, trimReports.values()) {
        readNameExtractor.indexIn(taskReport);
        resultReport += "<tr><td width=50></td><td width=300 nowrap>" + readNameExtractor.cap(1) + "</td></tr>";
    }

    if (trimReports.values().size() > 0) {
        resultReport += "</table>";
    }

    return resultReport;
}

QList<Task*> AlignToReferenceBlastCmdlineTask::onSubTaskFinished(Task *subTask) {
    QList<Task*> result;
    CHECK(subTask != NULL, result);
    CHECK(!subTask->isCanceled() && !subTask->hasError(), result);
    if (loadRef == subTask) {
        QFileInfo resultFile(settings.resultAlignmentFile);
        QDir resultDir = resultFile.dir();
        if (!resultDir.exists()) {
            bool mkDirResult = QDir().mkpath(resultDir.absolutePath());
            if (!mkDirResult) {
                setError(tr("Failed to create output folder: %1.").arg(resultDir.absolutePath()));
                return result;
            }
        }
        CmdlineInOutTaskConfig config;

        config.command = "--task=" + ALIGN_TO_REF_CMDLINE;
        QString argString = "--%1=\"%2\"";
        config.arguments << argString.arg(REF_ARG).arg(QFileInfo(settings.referenceUrl).absoluteFilePath());
        config.arguments << argString.arg(READS_ARG).arg(settings.readUrls.join(";"));
        config.arguments << argString.arg(MIN_IDENTITY_ARG).arg(settings.minIdentity);
        config.arguments << argString.arg(ROW_NAMING_ARG).arg(settings.getRowNamingPolicyString());
        config.arguments << argString.arg(MIN_LEN_ARG).arg(settings.minLength);
        config.arguments << argString.arg(THRESHOLD_ARG).arg(settings.qualityThreshold);
        config.arguments << argString.arg(TRIM_ARG).arg(true);
        config.arguments << argString.arg(RESULT_ALIGNMENT_ARG).arg(QFileInfo(settings.resultAlignmentFile).absoluteFilePath());

        config.reportFile = reportFile.fileName();
        config.emptyOutputPossible = true;

        config.logLevel = LogLevel_TRACE;

        cmdlineTask = new CmdlineInOutTaskRunner(config);
        result.append(cmdlineTask);
    } else if (subTask == cmdlineTask && settings.addResultToProject) {
        // add load document task
        FormatDetectionConfig config;
        QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(settings.resultAlignmentFile, config);
        CHECK_EXT(!formats.isEmpty() && (NULL != formats.first().format), setError(tr("wrong output format")), result);

        DocumentFormat *format = formats.first().format;
        CHECK_EXT(format->getSupportedObjectTypes().contains(GObjectTypes::MULTIPLE_CHROMATOGRAM_ALIGNMENT), setError(tr("wrong output format")), result);

        Task *loadTask = AppContext::getProjectLoader()->openWithProjectTask(settings.resultAlignmentFile);
        AppContext::getTaskScheduler()->registerTopLevelTask(loadTask);
    }

    return result;
}

void AlignToReferenceBlastCmdlineTask::run() {
    reportFile.open();
    reportString = reportFile.readAll();
}

Task::ReportResult AlignToReferenceBlastCmdlineTask::report() {
    if (loadRef != NULL) {
        loadRef->cleanup();
    }
    return ReportResult_Finished;
}


AlignToReferenceBlastDialog::AlignToReferenceBlastDialog(QWidget *parent)
    : QDialog(parent),
      saveController(NULL),
      savableWidget(this)
{
    setupUi(this);
    GCOUNTER(cvar, tvar, "'Map reads to reference' dialog opening");

    new HelpButton(this, buttonBox, "24749114");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Map"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    connectSlots();
    initSaveController();
    readsListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    cbRowNaming->addItem(tr("File name"), AlignToReferenceBlastCmdlineTask::Settings::FileName);
    cbRowNaming->addItem(tr("Sequence name from file"), AlignToReferenceBlastCmdlineTask::Settings::SequenceName);
    cbRowNaming->setCurrentIndex(cbRowNaming->findData(AlignToReferenceBlastCmdlineTask::Settings::SequenceName));

    U2WidgetStateStorage::restoreWidgetState(savableWidget);
    saveController->setPath(outputLineEdit->text());

    new QShortcut(QKeySequence(Qt::Key_Delete), this, SLOT(sl_removeRead()));

    defaultOutputUrl = outputLineEdit->text();
}

void AlignToReferenceBlastDialog::initSaveController() {
    SaveDocumentControllerConfig conf;
    conf.defaultFormatId = BaseDocumentFormats::UGENEDB;
    conf.fileDialogButton = setOutputButton;
    conf.fileNameEdit = outputLineEdit;
    conf.formatCombo = NULL;
    conf.parentWidget = this;
    conf.saveTitle = tr("Select Output File...");
    conf.defaultFileName = GUrlUtils::getDefaultDataPath() + "/" + DEFAULT_OUTPUT_FILE_NAME;

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::UGENEDB;
    saveController = new SaveDocumentController(conf, formats, this);
}

AlignToReferenceBlastCmdlineTask::Settings AlignToReferenceBlastDialog::getSettings() const {
    return settings;
}

void AlignToReferenceBlastDialog::accept() {
    if (referenceLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Reference sequence is not set."));
        return;
    }
    settings.referenceUrl = referenceLineEdit->text();

    if (readsListWidget->count() == 0) {
        QMessageBox::warning(this, tr("Error"),
                             tr("No reads provided."));
        return;
    }
    QStringList readUrls;
    for (int i = 0; i < readsListWidget->count(); i++) {
        QListWidgetItem* item = readsListWidget->item(i);
        SAFE_POINT(item != NULL, "Item is NULL", );
        QString s = item->text();
        readUrls.append(s);
    }
    settings.readUrls = readUrls;

    settings.minIdentity = minIdentitySpinBox->value();
    settings.minLength = 0;
    settings.qualityThreshold = qualitySpinBox->value();
    settings.rowNaming = static_cast<AlignToReferenceBlastCmdlineTask::Settings::RowNaming>(cbRowNaming->currentData().toInt());

    if (outputLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Output file is not set."));
        return;
    }
    settings.resultAlignmentFile = outputLineEdit->text();
    settings.addResultToProject = addToProjectCheckbox->isChecked();

    QString outUrl = saveController->getSaveFileName();
    QFile outFile(outUrl);
    if (outFile.exists()) {
        QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::Cancel;
        QObjectScopedPointer<QMessageBox> messageBox = new QMessageBox(QMessageBox::Question,
            tr("Overwrite the file?"),
            tr("The result file already exists. Would you like to overwrite it?"),
            buttons,
            this);
        messageBox->setIcon(QMessageBox::Question);
        messageBox->exec();
        CHECK(!messageBox.isNull(), )
        if (messageBox->result() == QMessageBox::Cancel) {
            return;
        }
        if (!outFile.remove()) {
            QMessageBox::critical(this, tr("Error"), tr("Unable to delete the file."));
            return;
        }
    }

    QDialog::accept();
}

void AlignToReferenceBlastDialog::sl_setReference() {
    LastUsedDirHelper lod;
    QString filter = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true);

    lod.url = U2FileDialog::getOpenFileName(this, tr("Open Reference Sequence"), lod.dir, filter);
    if (lod.url.isEmpty()) {
        return;
    }
    referenceLineEdit->setText(lod.url);
}

void AlignToReferenceBlastDialog::sl_addRead() {
    LastUsedDirHelper lod;
    QString filter = DialogUtils::prepareDocumentsFileFilterByObjType(GObjectTypes::SEQUENCE, true);

    QStringList readFiles = U2FileDialog::getOpenFileNames(this, tr("Select File(s) with Read(s)"), lod.dir, filter);
    if (readFiles.isEmpty()) {
        return;
    }

    foreach (const QString& read, readFiles) {
        if (readsListWidget->findItems(read, Qt::MatchExactly).isEmpty()) {
            readsListWidget->addItem(read);
        }
    }
}

void AlignToReferenceBlastDialog::sl_removeRead() {
    QList<QListWidgetItem*> selection = readsListWidget->selectedItems();
    CHECK(!selection.isEmpty(), );

    foreach (QListWidgetItem* item, selection) {
        readsListWidget->takeItem(readsListWidget->row(item));
    }
    qDeleteAll(selection);
}

void AlignToReferenceBlastDialog::sl_referenceChanged(const QString &newRef) {
    if (outputLineEdit->text() != defaultOutputUrl) { // file name is set by user -> do not change
        return;
    }
    // set default file name based on reference file name
    QString outDir = QFileInfo(outputLineEdit->text()).dir().absolutePath();
    QString outFile = QFileInfo(newRef).baseName() + "_" + DEFAULT_OUTPUT_FILE_NAME;
    saveController->setPath(outDir + "/" + outFile);
    defaultOutputUrl = saveController->getSaveFileName();
}

void AlignToReferenceBlastDialog::connectSlots() {
    connect(setReferenceButton, SIGNAL(clicked(bool)), SLOT(sl_setReference()));
    connect(addReadButton, SIGNAL(clicked(bool)), SLOT(sl_addRead()));
    connect(removeReadButton, SIGNAL(clicked(bool)), SLOT(sl_removeRead()));
    connect(referenceLineEdit, SIGNAL(textChanged(const QString &)), SLOT(sl_referenceChanged(const QString &)));
}

} // namespace
