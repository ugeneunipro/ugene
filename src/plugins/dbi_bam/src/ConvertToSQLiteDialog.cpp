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

#include "ConvertToSQLiteDialog.h"

#include <QDesktopWidget>
#include <QMessageBox>
#include <QTextEdit>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/FormatUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Task.h>
#include <U2Core/Theme.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include "BAMDbiPlugin.h"
#include "BaiReader.h"
#include "LoadBamInfoTask.h"

namespace U2 {
namespace BAM {

static const QString DIR_HELPER_DOMAIN("ConvertToSQLiteDialog");

ConvertToSQLiteDialog::ConvertToSQLiteDialog(const GUrl &_sourceUrl, BAMInfo &_bamInfo, bool sam)
    : QDialog(QApplication::activeWindow()),
      saveController(NULL),
      sourceUrl(_sourceUrl),
      bamInfo(_bamInfo) {
    ui.setupUi(this);
    new HelpButton(this, ui.buttonBox, "46500141");
    ui.buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Import"));
    ui.buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    if (sam) {
        setWindowTitle(tr("Import SAM File"));
    } else {
        setWindowTitle(tr("Import BAM File"));
    }
    this->setObjectName("Import BAM File");

    const QString warningMessageStyleSheet("color: " + Theme::successColorLabelStr() + "; font: bold;");
    ui.indexNotAvailableLabel->setStyleSheet(warningMessageStyleSheet);
    ui.referenceWarningLabel->setStyleSheet(warningMessageStyleSheet);

    initSaveController();

    connect(ui.bamInfoButton, SIGNAL(clicked()), SLOT(sl_bamInfoButtonClicked()));
    connect(ui.refUrlButton, SIGNAL(clicked()), SLOT(sl_refUrlButtonClicked()));
    connect(ui.selectAllToolButton, SIGNAL(clicked()), SLOT(sl_selectAll()));
    connect(ui.selectNoneToolButton, SIGNAL(clicked()), SLOT(sl_unselectAll()));
    connect(ui.inverseSelectionToolButton, SIGNAL(clicked()), SLOT(sl_inverseSelection()));
    ui.indexNotAvailableLabel->setVisible(sam ? false : !bamInfo.hasIndex());

    if (sam && bamInfo.getHeader().getReferences().isEmpty()) {
        hideReferencesTable();
    } else {
        hideReferenceUrl();
        hideReferenceMessage();
        ui.tableWidget->setColumnCount(3);
        ui.tableWidget->setRowCount(bamInfo.getHeader().getReferences().count());
        QStringList header;
        header << BAMDbiPlugin::tr("Assembly name") << BAMDbiPlugin::tr("Length") << BAMDbiPlugin::tr("URI");
        ui.tableWidget->setHorizontalHeaderLabels(header);
        ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
        {
            int i = 0;
            foreach (const Header::Reference &ref, bamInfo.getHeader().getReferences()) {
                QTableWidgetItem *checkbox = new QTableWidgetItem();
                checkbox->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                checkbox->setText(ref.getName());
                ui.tableWidget->setItem(i, 0, checkbox);
                QTableWidgetItem *item = new QTableWidgetItem(FormatUtils::formatNumberWithSeparators(ref.getLength()));
                item->setFlags(Qt::ItemIsEnabled);
                ui.tableWidget->setItem(i, 1, item);
                ui.tableWidget->setCellWidget(i, 2, new QLabel("<a href=\"" + ref.getUri() + "\">" + ref.getUri() + "</a>"));
                checkbox->setCheckState(Qt::Checked);
                i++;
            }
        }
        ui.tableWidget->verticalHeader()->setDefaultSectionSize(QFontMetrics(QFont()).height() + 5);
    }
    QPushButton *okButton = ui.buttonBox->button(QDialogButtonBox::Ok);
    ui.importUnmappedBox->setCheckState(bamInfo.isUnmappedSelected() ? Qt::Checked : Qt::Unchecked);
    ui.sourceUrlView->setText(QDir::cleanPath(sourceUrl.getURLString()));
    okButton->setFocus();
    connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem *)), SLOT(sl_assemblyCheckChanged(QTableWidgetItem *)));

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    adjustSize();
    if (ui.tableWidget->isHidden()) {
        setFixedHeight(height());
    }
    setMinimumWidth(600);
}

void ConvertToSQLiteDialog::hideReferenceUrl() {
    ui.refUrlLabel->hide();
    ui.refUrlEdit->hide();
    ui.refUrlButton->hide();
}

void ConvertToSQLiteDialog::hideReferencesTable() {
    ui.selectionButtons->hide();
    ui.tableWidget->hide();
}

void ConvertToSQLiteDialog::hideReferenceMessage() {
    ui.referenceWarningLabel->setVisible(false);
}

void ConvertToSQLiteDialog::sl_selectAll() {
    for (int i = 0; i < bamInfo.getSelected().count(); i++) {
        ui.tableWidget->item(i, 0)->setCheckState(Qt::Checked);
    }
}

void ConvertToSQLiteDialog::sl_unselectAll() {
    for (int i = 0; i < bamInfo.getSelected().count(); i++) {
        ui.tableWidget->item(i, 0)->setCheckState(Qt::Unchecked);
    }
}
void ConvertToSQLiteDialog::sl_inverseSelection() {
    for (int i = 0; i < bamInfo.getSelected().count(); i++) {
        QTableWidgetItem *item = ui.tableWidget->item(i, 0);
        item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
    }
}

void ConvertToSQLiteDialog::sl_bamInfoButtonClicked() {
    const Header &header = bamInfo.getHeader();
    QObjectScopedPointer<QDialog> dialog = new QDialog(this);
    dialog->setWindowTitle(BAMDbiPlugin::tr("%1 file info").arg(sourceUrl.getURLString()));
    dialog->setLayout(new QVBoxLayout());

    {
        QTableWidget *table = new QTableWidget();
        table->setColumnCount(2);
        table->setHorizontalHeaderLabels(QStringList() << BAMDbiPlugin::tr("Property name") << BAMDbiPlugin::tr("Value"));
        table->horizontalHeader()->setStretchLastSection(true);
        table->verticalHeader()->setVisible(false);

        QList<QPair<QString, QString>> list;
        QString sort;
        switch (header.getSortingOrder()) {
        case Header::Unsorted:
            sort = BAMDbiPlugin::tr("Unsorted");
            break;
        case Header::Unknown:
            sort = BAMDbiPlugin::tr("Unknown");
            break;
        case Header::Coordinate:
            sort = BAMDbiPlugin::tr("Coordinate");
            break;
        case Header::QueryName:
            sort = BAMDbiPlugin::tr("Query name");
            break;
        }

        list << QPair<QString, QString>(BAMDbiPlugin::tr("URL"), sourceUrl.getURLString())
             << QPair<QString, QString>(BAMDbiPlugin::tr("Format version"), header.getFormatVersion().text)
             << QPair<QString, QString>(BAMDbiPlugin::tr("Sorting order"), sort);

        table->setRowCount(list.count());
        {
            for (int i = 0; i < list.count(); i++) {
                const QPair<QString, QString> &pair = list.at(i);
                QTableWidgetItem *item = new QTableWidgetItem(pair.first);
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                table->setItem(i, 0, item);
                item = new QTableWidgetItem(pair.second);
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                table->setItem(i, 1, item);
            }
        }
        dialog->layout()->addWidget(table);
    }

    {
        QTableWidget *table = new QTableWidget();
        table->setColumnCount(9);
        table->setHorizontalHeaderLabels(QStringList() << BAMDbiPlugin::tr("Sequencing center") << BAMDbiPlugin::tr("Description") << BAMDbiPlugin::tr("Date")
                                                       << BAMDbiPlugin::tr("Library") << BAMDbiPlugin::tr("Programs") << BAMDbiPlugin::tr("Predicted median insert size") << BAMDbiPlugin::tr("Platform/technology")
                                                       << BAMDbiPlugin::tr("Platform unit") << BAMDbiPlugin::tr("Sample"));
        table->horizontalHeader()->setStretchLastSection(true);

        int i = 0;
        foreach (const Header::ReadGroup &rg, header.getReadGroups()) {
            QStringList rgList;
            rgList << QString(rg.getSequencingCenter()) << QString(rg.getDescription()) << QString(rg.getDate().toString()) << QString(rg.getLibrary())
                   << QString(rg.getPlatform()) << QString(rg.getPredictedInsertSize()) << QString(rg.getPlatform()) << QString(rg.getPlatformUnit()) << QString(rg.getSample());
            int j = 0;
            foreach (const QString &s, rgList) {
                QTableWidgetItem *item = new QTableWidgetItem(s);
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                table->setItem(i, j, item);
                j++;
            }
            i++;
        }
        dialog->layout()->addWidget(new QLabel(BAMDbiPlugin::tr("Read groups:")));
        dialog->layout()->addWidget(table);
    }

    {
        QTableWidget *table = new QTableWidget();
        table->setColumnCount(4);
        table->setHorizontalHeaderLabels(QStringList() << BAMDbiPlugin::tr("Name") << BAMDbiPlugin::tr("Version") << BAMDbiPlugin::tr("Command") << BAMDbiPlugin::tr("Previous ID"));
        table->horizontalHeader()->setStretchLastSection(true);

        int i = 0;
        foreach (const Header::Program &pg, header.getPrograms()) {
            QStringList pgList;
            pgList << QString(pg.getName()) << QString(pg.getVersion()) << QString(pg.getCommandLine()) << QString(pg.getPreviousId());
            int j = 0;
            foreach (const QString &s, pgList) {
                QTableWidgetItem *item = new QTableWidgetItem(s);
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                table->setItem(i, j, item);
                j++;
            }
            i++;
        }
        dialog->layout()->addWidget(new QLabel(BAMDbiPlugin::tr("Programs:")));
        dialog->layout()->addWidget(table);
    }
    dialog->resize(qMin(600, QApplication::desktop()->screenGeometry().width()), dialog->sizeHint().height());
    dialog->exec();
}

void ConvertToSQLiteDialog::sl_refUrlButtonClicked() {
    GUrl currentUrl = ui.refUrlEdit->text();
    if (ui.refUrlEdit->text().isEmpty()) {
        currentUrl = sourceUrl;
    }
    QString dir = currentUrl.dirPath() + "/" + currentUrl.baseFileName();
    QString value;
#ifdef Q_OS_MAC
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        value = U2FileDialog::getOpenFileName(this, QObject::tr("Reference File"), dir, "", 0, QFileDialog::DontUseNativeDialog);
    } else
#endif
        value = U2FileDialog::getOpenFileName(this, QObject::tr("Reference File"), dir);
    if (!value.isEmpty()) {
        ui.refUrlEdit->setText(value);
        hideReferenceMessage();
    }
}

void ConvertToSQLiteDialog::sl_assemblyCheckChanged(QTableWidgetItem *item) {
    bamInfo.getSelected()[item->row()] = (item->checkState() == Qt::Checked);
}

const GUrl &ConvertToSQLiteDialog::getDestinationUrl() const {
    return destinationUrl;
}

QString ConvertToSQLiteDialog::getReferenceUrl() const {
    return ui.refUrlEdit->text();
}

bool ConvertToSQLiteDialog::addToProject() const {
    return ui.addToProjectBox->isChecked();
}

void ConvertToSQLiteDialog::hideAddToProjectOption() {
    ui.addToProjectBox->hide();
}

bool ConvertToSQLiteDialog::referenceFromFile() {
    return ui.refUrlEdit->isVisible();
}

bool ConvertToSQLiteDialog::checkReferencesState() {
    if (referenceFromFile()) {
        return true;
    } else {
        bool selected = false;
        foreach (const bool &i, bamInfo.getSelected()) {
            if (i) {
                selected = true;
                break;
            }
        }
        if (!selected && !bamInfo.isUnmappedSelected()) {
            QMessageBox::critical(this, windowTitle(), BAMDbiPlugin::tr("Please select assemblies to be imported"));
            return false;
        }
    }
    return true;
}

void ConvertToSQLiteDialog::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultDomain = DIR_HELPER_DOMAIN;
    config.defaultFileName = sourceUrl.dirPath() + "/" + QFileInfo(sourceUrl.fileName()).completeBaseName() + ".ugenedb";
    config.defaultFormatId = BaseDocumentFormats::UGENEDB;
    config.fileDialogButton = ui.destinationUrlButton;
    config.fileNameEdit = ui.destinationUrlEdit;
    config.parentWidget = this;
    config.saveTitle = BAMDbiPlugin::tr("Destination UGENEDB File");

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::UGENEDB;

    saveController = new SaveDocumentController(config, formats, this);
}

namespace {
bool checkWritePermissions(const QString &fileUrl) {
    QDir dir = QFileInfo(fileUrl).dir();
    if (!dir.exists()) {
        bool created = dir.mkpath(dir.absolutePath());
        CHECK(created, false);
    }
    return FileAndDirectoryUtils::isDirectoryWritable(dir.absolutePath());
}
}    // namespace

void ConvertToSQLiteDialog::accept() {
    destinationUrl = GUrl(saveController->getSaveFileName());
    bamInfo.setUnmappedSelected(ui.importUnmappedBox->checkState() == Qt::Checked);
    if (destinationUrl.isEmpty()) {
        ui.destinationUrlEdit->setFocus(Qt::OtherFocusReason);
        QMessageBox::critical(this, windowTitle(), BAMDbiPlugin::tr("Destination URL is not specified"));
    } else if (!destinationUrl.isLocalFile()) {
        ui.destinationUrlEdit->setFocus(Qt::OtherFocusReason);
        QMessageBox::critical(this, windowTitle(), BAMDbiPlugin::tr("Destination URL must point to a local file"));
    } else if (!checkWritePermissions(destinationUrl.getURLString())) {
        ui.destinationUrlEdit->setFocus(Qt::OtherFocusReason);
        QMessageBox::critical(this, windowTitle(), BAMDbiPlugin::tr("Destination URL folder has not write permissions"));
    } else {
        if (!checkReferencesState()) {
            return;
        }

        Project *prj = AppContext::getProject();
        if (prj != NULL) {
            Document *destDoc = prj->findDocumentByURL(destinationUrl);
            if (destDoc != NULL && destDoc->isLoaded() && !GObjectViewUtils::findViewsWithAnyOfObjects(destDoc->getObjects()).isEmpty()) {
                QMessageBox::critical(this, windowTitle(), BAMDbiPlugin::tr("There is opened view with destination file.\n"
                                                                            "Close it or choose different file"));
                ui.destinationUrlEdit->setFocus(Qt::OtherFocusReason);
                return;
            }
        }
        QFileInfo destinationDir(QFileInfo(destinationUrl.getURLString()).path());
        if (!destinationDir.isWritable()) {
            ui.destinationUrlEdit->setFocus(Qt::OtherFocusReason);
            QMessageBox::critical(this, windowTitle(), BAMDbiPlugin::tr("Destination folder '%1' is not writable, please choose different destination URL").arg(destinationDir.absoluteFilePath()));
            return;
        }

        if (QFile::exists(destinationUrl.getURLString())) {
            int result = QMessageBox::question(this, windowTitle(), BAMDbiPlugin::tr("Destination file already exists.\n"
                                                                                     "To overwrite the file, press 'Replace'.\n"
                                                                                     "To append data to existing file press 'Append'."),
                                               BAMDbiPlugin::tr("Replace"),
                                               BAMDbiPlugin::tr("Append"),
                                               BAMDbiPlugin::tr("Cancel"),
                                               2);
            switch (result) {
            case 0: {
                bool ok = QFile::remove(destinationUrl.getURLString());
                if (!ok) {
                    QMessageBox::critical(this, windowTitle(), BAMDbiPlugin::tr("Destination file '%1' cannot be removed").arg(destinationUrl.getURLString()));
                    return;
                }
            }
                QDialog::accept();
                break;
            case 1:
                QDialog::accept();
                break;
            }
        } else {
            QDialog::accept();
        }
    }
}

}    // namespace BAM
}    // namespace U2
