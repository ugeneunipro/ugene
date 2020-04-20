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

#include "FormatDBSupportRunDialog.h"
#include "FormatDBSupport.h"

#include <U2Core/DNAAlphabet.h>

#include <U2Gui/GUIUtils.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/HelpButton.h>

#include <QPushButton>
#include <QToolButton>

namespace U2 {

////////////////////////////////////////
//FormatDBWithExtFileSpecifySupportRunDialog
FormatDBSupportRunDialog::FormatDBSupportRunDialog(const QString &_name, FormatDBSupportTaskSettings &_settings, QWidget *_parent) :
        QDialog(_parent), name(_name), settings(_settings)
{
    setupUi(this);
    new HelpButton(this, buttonBox, "24749011");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Format"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    formatButton = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(inputFilesToolButton,SIGNAL(clicked()),SLOT(sl_onBrowseInputFiles()));
    connect(inputDirToolButton,SIGNAL(clicked()),SLOT(sl_onBrowseInputDir()));
    connect(databasePathToolButton,SIGNAL(clicked()),SLOT(sl_onBrowseDatabasePath()));
    connect(inputFilesLineEdit,SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));
    connect(inputDirLineEdit,SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));
    connect(inputFilesRadioButton,SIGNAL(toggled(bool)), SLOT(sl_lineEditChanged()));
    connect(inputDirRadioButton,SIGNAL(toggled(bool)), SLOT(sl_lineEditChanged()));
    connect(databasePathLineEdit,SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));
    connect(databaseTitleLineEdit,SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));
    connect(baseNamelineEdit,SIGNAL(textChanged(QString)), SLOT(sl_lineEditChanged()));

    if(!settings.inputFilesPath.isEmpty()){
        QString names=settings.inputFilesPath.join(";");
        inputFilesLineEdit->setText(names);
    }
    if (!settings.isInputAmino) {
        nucleotideTypeRadioButton->setChecked(true);
    }
    formatButton->setEnabled(false);
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
    connect(formatButton,SIGNAL(clicked()),this,SLOT(sl_formatDB()));

}

void FormatDBSupportRunDialog::sl_onBrowseInputFiles(){
    LastUsedDirHelper lod("");
    QString name;
    QStringList lst = U2FileDialog::getOpenFileNames(NULL, tr("Select file(s)"), lod.dir, "");
    name = lst.join(";");
    if (!lst.isEmpty()) {
        lod.url = lst.first();
    }
    if (!name.isEmpty()) {
        inputFilesLineEdit->setText(name);
    }
    inputFilesLineEdit->setFocus();
    if(lst.length()==1){
        QFileInfo fi(lst.first());
        if(databaseTitleLineEdit->text().isEmpty()){
            databaseTitleLineEdit->setText(fi.baseName());
        }
        if(baseNamelineEdit->text().isEmpty()){
            baseNamelineEdit->setText(fi.baseName());
        }
    }
}
void FormatDBSupportRunDialog::sl_onBrowseInputDir(){
    LastUsedDirHelper lod("");

    QString name;
    lod.url = name = U2FileDialog::getExistingDirectory(NULL, tr("Select a folder with input files"), lod.dir);
    if (!name.isEmpty()) {
        inputDirLineEdit->setText(name);
    }
    inputDirLineEdit->setFocus();
}

void FormatDBSupportRunDialog::sl_onBrowseDatabasePath(){
    LastUsedDirHelper lod("Database folder");

    QString name;
    lod.url = name = U2FileDialog::getExistingDirectory(NULL, tr("Select a folder to save database files"), lod.dir);
    if (!name.isEmpty()) {
        databasePathLineEdit->setText(name);
    }
    databasePathLineEdit->setFocus();
}
void FormatDBSupportRunDialog::sl_lineEditChanged(){
    bool hasSpacesInInputFiles=false;
    bool hasSpacesInOutputDBPath=false;
    bool pathWarning = databasePathLineEdit->text().contains(' ');
    QString pathTooltip = pathWarning ? tr("Output database path contain space characters.") : "";
    GUIUtils::setWidgetWarning(databasePathLineEdit, pathWarning);
    databasePathLineEdit->setToolTip(pathTooltip);

    bool nameWarning = baseNamelineEdit->text().contains(' ');
    QString nameTooltip = nameWarning ? tr("Output database path contain space characters.") : "";
    GUIUtils::setWidgetWarning(baseNamelineEdit, nameWarning);
    baseNamelineEdit->setToolTip(nameTooltip);

    hasSpacesInOutputDBPath = pathWarning || nameWarning;

    bool isFilledInputFilesOrDirLineEdit =
            (!inputFilesLineEdit->text().isEmpty() && inputFilesRadioButton->isChecked()) ||
            (!inputDirLineEdit->text().isEmpty() && inputDirRadioButton->isChecked());
    bool isFilledDatabasePathLineEdit = !databasePathLineEdit->text().isEmpty();
    bool isFilledDatabaseTitleLineEdit = !databaseTitleLineEdit->text().isEmpty();
    bool isFilledBaseNamelineEdit = !baseNamelineEdit->text().isEmpty();
    formatButton->setEnabled(isFilledBaseNamelineEdit &&
                             isFilledDatabasePathLineEdit &&
                             isFilledDatabaseTitleLineEdit &&
                             isFilledInputFilesOrDirLineEdit &&
                             !hasSpacesInInputFiles &&
                             !hasSpacesInOutputDBPath);
}

QStringList getAllFiles(QDir inputDir, QString filter, bool isIncludeFilter=true);

void FormatDBSupportRunDialog::sl_formatDB(){
    if(inputFilesRadioButton->isChecked()){
        settings.inputFilesPath=inputFilesLineEdit->text().split(';');
    }else{
        if(includeFilterRadioButton->isChecked()){
            settings.inputFilesPath=getAllFiles(QDir(inputDirLineEdit->text()), includeFFLineEdit->text());
        }else{
            settings.inputFilesPath=getAllFiles(QDir(inputDirLineEdit->text()), excludeFFLineEdit->text(), false);
        }
    }
    settings.databaseTitle=databaseTitleLineEdit->text();
    if ((!databasePathLineEdit->text().endsWith('/'))&&(!databasePathLineEdit->text().endsWith('\\'))){
        settings.outputPath=databasePathLineEdit->text()+'/'+baseNamelineEdit->text();
    }else{
        settings.outputPath=databasePathLineEdit->text()+baseNamelineEdit->text();
    }
    settings.isInputAmino=proteinTypeRadioButton->isChecked();

    accept();
}

QStringList getAllFiles(QDir inputDir, QString filter, bool isIncludeFilter){
    QStringList includeFileList;
    QStringList excludeFilesList;
    QStringList filters=filter.split(';');
    //IsIncludeFilter == true
    //get files from input dir
    foreach(QString inputFileName, inputDir.entryList(filters, QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Dirs | QDir::Files)){
        inputFileName = inputDir.absolutePath()+"/"+inputFileName;
        QFileInfo inputFileInfo(inputFileName);
        if(inputFileInfo.isFile()){
            includeFileList.append(inputFileName);
        }
    }
    //get files from subdirs
    foreach(QString inputFileName, inputDir.entryList(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Dirs | QDir::Files)){
        inputFileName = inputDir.absolutePath()+"/"+inputFileName;
        QFileInfo inputFileInfo(inputFileName);
        if(inputFileInfo.isDir()){
            includeFileList.append(getAllFiles(QDir(inputFileName), filter, true));
            excludeFilesList.append(getAllFiles(QDir(inputFileName), filter, false));
        }else{
            if(!includeFileList.contains(inputFileName)){
                //IsIncludeFilter == false
                excludeFilesList.append(inputFileName);
            }
        }
    }
    if(isIncludeFilter){
        return includeFileList;
    }else{
        return excludeFilesList;
    }
}
}//namespace
