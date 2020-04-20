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

#include <QDir>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/LocalFileAdapter.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

#include "MultipleDocumentsReadingModeSelectorController.h"

namespace U2{

struct GUrlLess {
    bool operator()(GUrl a, GUrl b) const {
        return a.getURLString() < b.getURLString();
    }
}; 

bool MultipleDocumentsReadingModeSelectorController::adjustReadingMode(QVariantMap& props, QList<GUrl>& urls, const QMap<QString, qint64>& headerSequenceLengths){	
    qSort(urls.begin(), urls.end(), GUrlLess());

    MultipleDocumentsReadingModeDialog d(urls, QApplication::activeWindow());
    return d.setupGUI(urls, props, headerSequenceLengths);    
}

bool MultipleDocumentsReadingModeSelectorController::mergeDocumentOption(const FormatDetectionResult& formatResult, QMap<QString, qint64>* headerSequenceLengths){
    QVariantMap docHints = formatResult.rawDataCheckResult.properties;
    if(formatResult.format == NULL){
        return false;
    }
    if(formatResult.format->getFormatId() == BaseDocumentFormats::PLAIN_GENBANK){
        if(docHints.value(RawDataCheckResult_Sequence) == false){
            static const int MAX_LINE = 8192;
            char buff[MAX_LINE + 1] = {0};

            QScopedPointer<LocalFileAdapterFactory> factory( new LocalFileAdapterFactory());
            QScopedPointer<IOAdapter> io(factory->createIOAdapter());
            if(!io->open(formatResult.url, IOAdapterMode_Read)){
                return false;
            }
            bool terminatorFound = false;
            io->readLine(buff, MAX_LINE, &terminatorFound);
            if(!terminatorFound){
                return false;
            }
            QString line = QString(QByteArray(buff));
            QStringList words = line.split(QRegExp("\\s"), QString::SkipEmptyParts);
            if(words.size() < 3){ // origin len not defined
                return false;
            }
            bool isLenDefined = false;
            qint64 seqLen = words[2].toLongLong(&isLenDefined); 
            
            if(!isLenDefined || seqLen <= 0){
                return false;
            }
            (*headerSequenceLengths)[formatResult.url.getURLString()] = seqLen;
            return true;
        }
    }
    return docHints.value(RawDataCheckResult_Sequence).toBool();
}


MultipleDocumentsReadingModeDialog::MultipleDocumentsReadingModeDialog(const QList<GUrl>& _urls,QWidget* parent)
    : QDialog(parent),
      saveController(NULL),
      urls(_urls) {

}

QString MultipleDocumentsReadingModeDialog::setupNewUrl() {
    QString urlStr = newDocUrl->text();
    if (urlStr.isEmpty()) {
        GUrl url = urls.at(0);
        urlStr = url.getURLString();
    }
    QFileInfo fi(urlStr);
    urlStr = fi.dir().path();

    QString extension4MergedDocument;
    if (mergeMode->isChecked()) {
        extension4MergedDocument = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::PLAIN_GENBANK)->getSupportedDocumentFileExtensions().first();
    } else if (join2alignmentMode->isChecked()) {
        extension4MergedDocument = AppContext::getDocumentFormatRegistry()->getFormatById(BaseDocumentFormats::CLUSTAL_ALN)->getSupportedDocumentFileExtensions().first();
    }

    return urlStr + "/merged_document." + extension4MergedDocument;
}

bool MultipleDocumentsReadingModeDialog::setupGUI(QList<GUrl>& _urls, QVariantMap& props, const QMap<QString, qint64>& headerSequenceLengths){
    setModal(true);
    setupUi(this);
    new HelpButton(this, buttonBox, "24748773");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    // doesn't matter from what position, because excluded fileName all path of documents are the same
    CHECK(!urls.isEmpty(), false);

    connect(separateMode, SIGNAL(toggled(bool)), SLOT(sl_optionChanged()));
    connect(mergeMode, SIGNAL(toggled(bool)), SLOT(sl_optionChanged()));
    connect(join2alignmentMode, SIGNAL(toggled(bool)), SLOT(sl_optionChanged()));
    connect(upperButton, SIGNAL(clicked()), SLOT(sl_onMoveUp()));
    connect(bottomButton, SIGNAL(clicked()), SLOT(sl_onMoveDown()));

    upperButton->setIcon(QIcon(":ugene/images/up.png"));
    bottomButton->setIcon(QIcon(":ugene/images/down.png"));

    for(int i = 0; i<urls.size(); ++i){
        listDocuments->addItem(new QListWidgetItem(QString("%1. ").arg(i + 1) + urls.at(i).fileName(), listDocuments));           
    }

    int rc = exec();
    
    if (rc == QDialog::Rejected) {
        return false;
    }

    if(separateMode->isChecked() || saveController->getSaveFileName().isEmpty()){
        return true;
    }

    deleteAllNumPrefix();
    _urls.clear();

    for(int i = 0; i < listDocuments->count(); ++i){
        _urls << findUrlByFileName(listDocuments->item(i)->text());
    }

    listDocuments->clear();

    if(mergeMode->isChecked()){
        props[DocumentReadingMode_SequenceMergeGapSize] = fileGap->value();
        props[ProjectLoaderHint_MultipleFilesMode_RealDocumentFormat] = BaseDocumentFormats::PLAIN_GENBANK;

        foreach(const GUrl& url, _urls){
            props[RawDataCheckResult_HeaderSequenceLength + url.getURLString()] = headerSequenceLengths.value(url.getURLString(), -1);
        }
    }
    else if(join2alignmentMode->isChecked()){
        props[ProjectLoaderHint_MultipleFilesMode_RealDocumentFormat] = BaseDocumentFormats::CLUSTAL_ALN;
        props[DocumentReadingMode_SequenceAsAlignmentHint] = true;
    }
    props[ProjectLoaderHint_MultipleFilesMode_URLDocument] = saveController->getSaveFileName();
    props[ProjectLoaderHint_MultipleFilesMode_SaveDocumentFlag] = saveBox->isChecked();
    props[ProjectLoaderHint_MultipleFilesMode_Flag] = true;
    

    QStringList urlsStr;
    foreach(GUrl url, _urls){
        urlsStr << url.getURLString();
    }

    props[ProjectLoaderHint_MultipleFilesMode_URLsDocumentConsistOf] =  urlsStr;

    _urls.clear();
    _urls << GUrl(saveController->getSaveFileName());
    
    return true;
}

QString MultipleDocumentsReadingModeDialog::findUrlByFileName(const QString& fileName){
    foreach(GUrl url, urls){
        QString _fileName = url.fileName();
        if(_fileName == fileName){
            return url.getURLString();
        }
    }
    assert(false && "Error finding url by file name");
    return "";
}

void MultipleDocumentsReadingModeDialog::sl_optionChanged() {
    bool isNewDocInfoAvailable = !separateMode->isChecked();

    saveBox->setEnabled(isNewDocInfoAvailable);
    listDocuments->setEnabled(isNewDocInfoAvailable);
    upperButton->setEnabled(isNewDocInfoAvailable);
    bottomButton->setEnabled(isNewDocInfoAvailable);
    newDocLabel->setEnabled(isNewDocInfoAvailable);
    newDocUrl->setEnabled(isNewDocInfoAvailable);

    bool mergeInfoAvailable = mergeMode->isChecked();
    fileGap->setEnabled(mergeInfoAvailable);
    mergeModeLabel->setEnabled(mergeInfoAvailable);

    if (mergeMode->isChecked()) {
        delete saveController;
        initSequenceSaveController();
    } else if (join2alignmentMode->isChecked()) {
        delete saveController;
        initMsaSaveController();
    } else {
        return;
    }
}

void MultipleDocumentsReadingModeDialog::initSequenceSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultDomain = "SaveMergeDocumentsAsOneDocument";
    config.defaultFileName = setupNewUrl();
    config.defaultFormatId = BaseDocumentFormats::PLAIN_GENBANK;
    config.fileDialogButton = toolButton;
    config.fileNameEdit = newDocUrl;
    config.parentWidget = this;
    config.saveTitle = tr("Select file to save new document");
    config.rollOutProjectUrls = true;

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::PLAIN_GENBANK;

    saveController = new SaveDocumentController(config, formats, this);
}

void MultipleDocumentsReadingModeDialog::initMsaSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultDomain = "SaveMergeDocumentsAsOneDocument";
    config.defaultFileName = setupNewUrl();
    config.defaultFormatId = BaseDocumentFormats::CLUSTAL_ALN;
    config.fileDialogButton = toolButton;
    config.fileNameEdit = newDocUrl;
    config.parentWidget = this;
    config.saveTitle = tr("Select file to save new document");
    config.rollOutProjectUrls = true;

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::CLUSTAL_ALN;

    saveController = new SaveDocumentController(config, formats, this);
}

void MultipleDocumentsReadingModeDialog::sl_onMoveUp(){
    QListWidgetItem * item = listDocuments->currentItem();
    if(item == NULL){return;}
    int row = listDocuments->row(item);
    if(row == 0){return;}

    QListWidgetItem *newCurrent = new QListWidgetItem(item->text());
    listDocuments->insertItem(row - 1, newCurrent);
    listDocuments->removeItemWidget(item);
    delete item;

    changeNumPrefix();
    listDocuments->setCurrentItem(newCurrent);
}

void MultipleDocumentsReadingModeDialog::sl_onMoveDown(){
    QListWidgetItem * item = listDocuments->currentItem();
    if(item == NULL){return;}
    int row = listDocuments->row(item);
    if(row == listDocuments->count() - 1){return;}

    QListWidgetItem *newCurrent = new QListWidgetItem(item->text());
    listDocuments->insertItem(row + 2, newCurrent);
    listDocuments->removeItemWidget(item);
    delete item;
    
    changeNumPrefix();
    listDocuments->setCurrentItem(newCurrent);
}

void MultipleDocumentsReadingModeDialog::changeNumPrefix(){
    for(int i = 0; i < listDocuments->count(); ++i){
        listDocuments->item(i)->setText(QString("%1. ").arg(i+1) + deleteNumPrefix(listDocuments->item(i)->text()));
    }
}

QString MultipleDocumentsReadingModeDialog::deleteNumPrefix(QString prefixString){
    QString toDelete = prefixString.split(" ")[0]; // number with space
    prefixString.remove(0, toDelete.size() + 1);
    return prefixString;
}

void MultipleDocumentsReadingModeDialog::deleteAllNumPrefix(){
    for(int i = 0; i < listDocuments->count(); ++i){
        listDocuments->item(i)->setText(deleteNumPrefix(listDocuments->item(i)->text()));
    }
}

}
