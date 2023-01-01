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

#include "MakeBlastDbDialog.h"

#include <QToolButton>

#include <U2Core/DNAAlphabet.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>

namespace U2 {

////////////////////////////////////////
// MakeBlastDbDialog
MakeBlastDbDialog::MakeBlastDbDialog(QWidget* parent, const MakeBlastDbSettings& _settings)
    : QDialog(parent), settings(_settings) {
    setupUi(this);
    new HelpButton(this, buttonBox, "65930721");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Build"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    makeButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(inputFilesToolButton, SIGNAL(clicked()), SLOT(sl_onBrowseInputFiles()));
    connect(inputDirToolButton, SIGNAL(clicked()), SLOT(sl_onBrowseInputDir()));
    connect(databasePathToolButton, SIGNAL(clicked()), SLOT(sl_onBrowseDatabasePath()));
    connect(inputFilesLineEdit, SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));
    connect(inputDirLineEdit, SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));
    connect(inputFilesRadioButton, SIGNAL(toggled(bool)), SLOT(sl_lineEditChanged()));
    connect(inputDirRadioButton, SIGNAL(toggled(bool)), SLOT(sl_lineEditChanged()));
    connect(databasePathLineEdit, SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));
    connect(databaseTitleLineEdit, SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));
    connect(baseNamelineEdit, SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));

    if (!settings.inputFilesPath.isEmpty()) {
        QString names = settings.inputFilesPath.join(";");
        inputFilesLineEdit->setText(names);
    }
    if (!settings.isInputAmino) {
        nucleotideTypeRadioButton->setChecked(true);
    }
    makeButton->setEnabled(false);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(makeButton, SIGNAL(clicked()), this, SLOT(sl_makeBlastDb()));
}

void MakeBlastDbDialog::sl_onBrowseInputFiles() {
    LastUsedDirHelper lod("");
    QString name;
    QStringList lst = U2FileDialog::getOpenFileNames(nullptr, tr("Select file(s)"), lod.dir, "");
    name = lst.join(";");
    if (!lst.isEmpty()) {
        lod.url = lst.first();
    }
    if (!name.isEmpty()) {
        inputFilesLineEdit->setText(name);
    }
    inputFilesLineEdit->setFocus();
    if (lst.length() == 1) {
        QFileInfo fi(lst.first());
        if (databaseTitleLineEdit->text().isEmpty()) {
            databaseTitleLineEdit->setText(fi.baseName());
        }
        if (baseNamelineEdit->text().isEmpty()) {
            baseNamelineEdit->setText(fi.baseName());
        }
    }
}
void MakeBlastDbDialog::sl_onBrowseInputDir() {
    LastUsedDirHelper lod("");

    QString name;
    lod.url = name = U2FileDialog::getExistingDirectory(nullptr, tr("Select a folder with input files"), lod.dir);
    if (!name.isEmpty()) {
        inputDirLineEdit->setText(name);
    }
    inputDirLineEdit->setFocus();
}

void MakeBlastDbDialog::sl_onBrowseDatabasePath() {
    LastUsedDirHelper lod("Database folder");

    QString name;
    lod.url = name = U2FileDialog::getExistingDirectory(nullptr, tr("Select a folder to save database files"), lod.dir);
    if (!name.isEmpty()) {
        databasePathLineEdit->setText(name);
    }
    databasePathLineEdit->setFocus();
}
void MakeBlastDbDialog::sl_lineEditChanged() {
    bool hasSpacesInInputFiles = false;
    bool pathWarning = databasePathLineEdit->text().contains(' ');
    QString pathTooltip = pathWarning ? tr("Output database path contain space characters.") : "";
    GUIUtils::setWidgetWarningStyle(databasePathLineEdit, pathWarning);
    databasePathLineEdit->setToolTip(pathTooltip);

    bool nameWarning = baseNamelineEdit->text().contains(' ');
    QString nameTooltip = nameWarning ? tr("Output database path contain space characters.") : "";
    GUIUtils::setWidgetWarningStyle(baseNamelineEdit, nameWarning);
    baseNamelineEdit->setToolTip(nameTooltip);

    bool hasSpacesInOutputDBPath = pathWarning || nameWarning;

    bool isFilledInputFilesOrDirLineEdit =
        (!inputFilesLineEdit->text().isEmpty() && inputFilesRadioButton->isChecked()) ||
        (!inputDirLineEdit->text().isEmpty() && inputDirRadioButton->isChecked());
    bool isFilledDatabasePathLineEdit = !databasePathLineEdit->text().isEmpty();
    bool isFilledDatabaseTitleLineEdit = !databaseTitleLineEdit->text().isEmpty();
    bool isFilledBaseNameLineEdit = !baseNamelineEdit->text().isEmpty();
    makeButton->setEnabled(isFilledBaseNameLineEdit &&
                           isFilledDatabasePathLineEdit &&
                           isFilledDatabaseTitleLineEdit &&
                           isFilledInputFilesOrDirLineEdit &&
                           !hasSpacesInInputFiles &&
                           !hasSpacesInOutputDBPath);
}

QStringList getAllFiles(QDir inputDir, QString filter, bool isIncludeFilter = true);

void MakeBlastDbDialog::sl_makeBlastDb() {
    if (inputFilesRadioButton->isChecked()) {
        settings.inputFilesPath = inputFilesLineEdit->text().split(';');
    } else {
        QDir inputDir(inputDirLineEdit->text());
        settings.inputFilesPath = includeFilterRadioButton->isChecked()
                                      ? getAllFiles(inputDir, includeFFLineEdit->text())
                                      : getAllFiles(inputDir, excludeFFLineEdit->text(), false);
    }
    settings.databaseTitle = databaseTitleLineEdit->text();
    if ((!databasePathLineEdit->text().endsWith('/')) && (!databasePathLineEdit->text().endsWith('\\'))) {
        settings.outputPath = databasePathLineEdit->text() + '/' + baseNamelineEdit->text();
    } else {
        settings.outputPath = databasePathLineEdit->text() + baseNamelineEdit->text();
    }
    settings.isInputAmino = proteinTypeRadioButton->isChecked();

    accept();
}

QStringList getAllFiles(QDir inputDir, QString filter, bool isIncludeFilter) {
    QStringList includeFileList;
    QStringList excludeFilesList;
    QStringList filters = filter.split(';');
    // IsIncludeFilter == true
    // get files from input dir
    foreach (QString inputFileName, inputDir.entryList(filters, QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Dirs | QDir::Files)) {
        inputFileName = inputDir.absolutePath() + "/" + inputFileName;
        QFileInfo inputFileInfo(inputFileName);
        if (inputFileInfo.isFile()) {
            includeFileList.append(inputFileName);
        }
    }
    // get files from subdirs
    foreach (QString inputFileName, inputDir.entryList(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Dirs | QDir::Files)) {
        inputFileName = inputDir.absolutePath() + "/" + inputFileName;
        QFileInfo inputFileInfo(inputFileName);
        if (inputFileInfo.isDir()) {
            includeFileList.append(getAllFiles(QDir(inputFileName), filter, true));
            excludeFilesList.append(getAllFiles(QDir(inputFileName), filter, false));
        } else {
            if (!includeFileList.contains(inputFileName)) {
                // IsIncludeFilter == false
                excludeFilesList.append(inputFileName);
            }
        }
    }
    if (isIncludeFilter) {
        return includeFileList;
    } else {
        return excludeFilesList;
    }
}

const MakeBlastDbSettings& MakeBlastDbDialog::getTaskSettings() const {
    return settings;
}

}  // namespace U2
