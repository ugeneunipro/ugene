/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "DownloadRemoteFileDialog.h"

#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPushButton>
#include <QXmlInputSource>

#include <U2Core/AppContext.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/LoadRemoteDocumentTask.h>
#include <U2Core/Log.h>
#include <U2Core/MultiTask.h>
#include <U2Core/Settings.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include "OpenViewTask.h"
#include "ui_DownloadRemoteFileDialog.h"

static const QString SAVE_DIR("downloadremotefiledialog/savedir");
static const QString HINT_STYLE_SHEET = "color:green; font:bold";
static const QString DB_EXTERNAL_LINK = QString(R"(<a href="%1"><img src=":core/images/external_link.png" width="22" height="22"></a>)");

namespace U2 {

QString DownloadRemoteFileDialog::defaultDB("");

DownloadRemoteFileDialog::DownloadRemoteFileDialog(QWidget* p)
    : QDialog(p), isQueryDB(false) {
    ui = new Ui_DownloadRemoteFileDialog;
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "65929340");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    ui->formatBox->hide();
    ui->formatLabel->hide();
    adjustSize();

    RemoteDBRegistry& registry = RemoteDBRegistry::getRemoteDBRegistry();
    connect(ui->databasesBox, &QComboBox::currentTextChanged, this, [this](const QString& text) {
        auto link = RemoteDBRegistry::EXTERNAL_LINKS.value(text);
        SAFE_POINT(!link.isEmpty(), QString("No database found: %1").arg(text), );

        auto sign = DB_EXTERNAL_LINK.arg(link);
        ui->lbExternalLink->setText(sign);
    });

    const QList<QString> dataBases = registry.getDBs();
    foreach (const QString& dbName, dataBases) {
        ui->databasesBox->addItem(dbName, dbName);
    }

    if (!defaultDB.isEmpty()) {
        int index = ui->databasesBox->findData(defaultDB);
        if (index != -1) {
            ui->databasesBox->setCurrentIndex(index);
        }
    }

    ui->hintLabel->setStyleSheet(HINT_STYLE_SHEET);

    connect(ui->databasesBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_onDbChanged()));
    connect(ui->saveFilenameToolButton, SIGNAL(clicked()), SLOT(sl_saveFilenameButtonClicked()));
    connect(ui->hintLabel, SIGNAL(linkActivated(const QString&)), SLOT(sl_linkActivated(const QString&)));

    sl_onDbChanged();

    setSaveFilename();
}

DownloadRemoteFileDialog::DownloadRemoteFileDialog(const QString& id, const QString& dbId, QWidget* p /* = NULL*/)
    : QDialog(p), isQueryDB(false) {
    ui = new Ui_DownloadRemoteFileDialog;
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "65929336");

    ui->formatBox->addItem(GENBANK_FORMAT);
    ui->formatBox->addItem(FASTA_FORMAT);
    connect(ui->formatBox, SIGNAL(currentIndexChanged(const QString&)), SLOT(sl_formatChanged(const QString&)));
    adjustSize();

    ui->databasesBox->clear();
    const QString dbName =
        dbId == EntrezUtils::NCBI_DB_PROTEIN ? RemoteDBRegistry::GENBANK_PROTEIN : RemoteDBRegistry::GENBANK_DNA;
    ui->databasesBox->addItem(dbName, dbName);

    ui->idLineEdit->setText(id);
    ui->idLineEdit->setReadOnly(true);

    delete ui->hintLabel;
    ui->hintLabel = nullptr;
    setMinimumSize(500, 0);

    connect(ui->saveFilenameToolButton, SIGNAL(clicked()), SLOT(sl_saveFilenameButtonClicked()));
    setSaveFilename();
}

const QString DOWNLOAD_REMOTE_FILE_DOMAIN = "DownloadRemoteFileDialog";

void DownloadRemoteFileDialog::sl_saveFilenameButtonClicked() {
    LastUsedDirHelper lod(DOWNLOAD_REMOTE_FILE_DOMAIN);
    QString filename = U2FileDialog::getExistingDirectory(this, tr("Select folder to save"), lod.dir);
    if (!filename.isEmpty()) {
        ui->saveFilenameLineEdit->setText(filename);
        lod.url = filename;
    }
}

static const QString DEFAULT_FILENAME = "file.format";
void DownloadRemoteFileDialog::setSaveFilename() {
    QString dir = AppContext::getSettings()->getValue(SAVE_DIR, "").value<QString>();
    if (dir.isEmpty()) {
        dir = LoadRemoteDocumentTask::getDefaultDownloadDirectory();
        assert(!dir.isEmpty());
    }
    ui->saveFilenameLineEdit->setText(QDir::toNativeSeparators(dir));
}

QString DownloadRemoteFileDialog::getResourceId() const {
    return ui->idLineEdit->text().trimmed();
}

QString DownloadRemoteFileDialog::getDBId() const {
    int curIdx = ui->databasesBox->currentIndex();
    if (curIdx == -1) {
        return QString("");
    }
    return ui->databasesBox->itemData(curIdx).toString();
}

QString DownloadRemoteFileDialog::getFullpath() const {
    return ui->saveFilenameLineEdit->text();
}

void DownloadRemoteFileDialog::accept() {
    defaultDB = getDBId();

    QString resourceId = getResourceId();
    if (resourceId.isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("Resource id is empty!"));
        ui->idLineEdit->setFocus();
        return;
    }
    QString fullPath = getFullpath();
    if (ui->saveFilenameLineEdit->text().isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), tr("No folder selected for saving file!"));
        ui->saveFilenameLineEdit->setFocus();
        return;
    }

    U2OpStatus2Log os;
    fullPath = GUrlUtils::prepareDirLocation(fullPath, os);

    if (fullPath.isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), os.getError());
        ui->saveFilenameLineEdit->setFocus();
        return;
    }

    QString dbId = getDBId();
    QStringList resIds = resourceId.split(QRegExp("[\\s,;]+"));
    QList<Task*> tasks;

    QString fileFormat;
    if (ui->formatBox->count() > 0) {
        fileFormat = ui->formatBox->currentText();
    }

    QVariantMap hints;
    hints.insert(FORCE_DOWNLOAD_SEQUENCE_HINT, ui->chbForceDownloadSequence->isVisible() && ui->chbForceDownloadSequence->isChecked());

    bool addToProject = ui->chbAddToProjectCheck->isChecked();
    if (addToProject && resIds.size() >= 100) {
        QString message = tr("There are more than 100 files found for download.\nAre you sure you want to open all of them?");
        int button = QMessageBox::question(QApplication::activeWindow(), tr("Warning"), message, tr("Cancel"), tr("Open anyway"), tr("Don't open"));
        if (button == 0) {
            return;  // return to dialog
        } else if (button == 2) {
            addToProject = false;
        }
    }
    for (int i = 0; i < resIds.size(); i++) {
        if (!addToProject) {
            auto task = new LoadRemoteDocumentTask(resIds[i], dbId, fullPath, fileFormat, hints);
            task->setReportingSupported(true);
            task->setReportingEnabled(true);
            tasks << task;
        } else {
            bool openView = i < OpenViewTask::MAX_DOC_NUMBER_TO_OPEN_VIEWS ? true : false;
            tasks << new LoadRemoteDocumentAndAddToProjectTask(resIds[i], dbId, fullPath, fileFormat, hints, openView);
        }
    }

    TaskFlags flags = TaskFlag_NoRun | TaskFlag_ReportingIsSupported | TaskFlag_ReportingIsEnabled;
    Task* topLevelTask = new MultiTask(tr("Download remote documents"), tasks, false, flags);
    AppContext::getTaskScheduler()->registerTopLevelTask(topLevelTask);

    QDialog::accept();
}

DownloadRemoteFileDialog::~DownloadRemoteFileDialog() {
    AppContext::getSettings()->setValue(SAVE_DIR, ui->saveFilenameLineEdit->text());
    delete ui;
}

bool DownloadRemoteFileDialog::isNcbiDb(const QString& dbId) const {
    return dbId == RemoteDBRegistry::GENBANK_DNA || dbId == RemoteDBRegistry::GENBANK_PROTEIN;
}

void DownloadRemoteFileDialog::sl_onDbChanged() {
    QString dbId = getDBId();
    QString hint;
    QString description;

    ui->chbForceDownloadSequence->setVisible(isNcbiDb(dbId));

    RemoteDBRegistry& registry = RemoteDBRegistry::getRemoteDBRegistry();
    hint = description = registry.getHint(dbId);

    setupHintText(hint);
    ui->idLineEdit->setToolTip(description);
}

void DownloadRemoteFileDialog::sl_formatChanged(const QString& format) {
    ui->chbForceDownloadSequence->setVisible(GENBANK_FORMAT == format);
}

void DownloadRemoteFileDialog::sl_linkActivated(const QString& link) {
    if (!link.isEmpty()) {
        ui->idLineEdit->setText(link);
    }
}

void DownloadRemoteFileDialog::setupHintText(const QString& text) {
    SAFE_POINT(ui != nullptr && ui->hintLabel != nullptr, "Invalid dialog content!", );
    const QString hintStart(tr("Hint: "));
    const QString hintSample = (text.isEmpty() ? tr("Use database unique identifier.") : text) + "<br>";
    const QString hintEnd(tr("You can download multiple items by separating IDs with space "
                             "or semicolon."));
    ui->hintLabel->setText(hintStart + hintSample + hintEnd);
}

}  // namespace U2
