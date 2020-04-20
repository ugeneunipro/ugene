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

#include <QKeyEvent>
#include <QMessageBox>

#include <U2Algorithm/DnaAssemblyAlgRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/SAMFormat.h>

#include <U2Gui/AppSettingsGUI.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/SaveDocumentController.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/DnaAssemblyUtils.h>

#include "DnaAssemblyDialog.h"
#include "DnaAssemblyGUIExtension.h"

namespace U2 {

QStringList DnaAssemblyDialog::lastShortReadsUrls;
QString DnaAssemblyDialog::lastRefSeqUrl;
QString DnaAssemblyDialog::methodName;
bool DnaAssemblyDialog::prebuiltIndex = false;
bool DnaAssemblyDialog::samOutput = false;

#define LIBRARY_TYPE_PAIRED     "Paired-end"
#define LIBRARY_TYPE_SINGLE     "Single-end"
#define MATE_UPSTREAM           "Upstream"
#define MATE_DOWNSTREAM         "Downstream"

DnaAssemblyDialog::DnaAssemblyDialog(QWidget* p, const QStringList& shortReadsUrls, const QString& refSeqUrl)
    : QDialog(p),
      assemblyRegistry(AppContext::getDnaAssemblyAlgRegistry()),
      customGUI(NULL),
      saveController(NULL)
{
    setupUi(this);
    QMap<QString,QString> helpPagesMap;
    helpPagesMap.insert("BWA","24748995");
    helpPagesMap.insert("BWA-MEM","24749029");
    helpPagesMap.insert("BWA-SW","24749023");
    helpPagesMap.insert("Bowtie","24748992");
    helpPagesMap.insert("Bowtie2","24749020");
    helpPagesMap.insert("UGENE Genome Aligner","24748998");
    new ComboboxDependentHelpButton(this, buttonBox, methodNamesBox, helpPagesMap);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Start"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    QStringList names = assemblyRegistry->getRegisteredAlgorithmIds();
    methodNamesBox->addItems(names);
    // TODO: change the way default method is set
    if (names.size() > 0) {
        int res = -1;
        if (!methodName.isEmpty()) {
            res = methodNamesBox->findText(methodName);
        }
        if (-1 == res) {
            methodNamesBox->setCurrentIndex(names.size() - 1);
        } else {
            methodNamesBox->setCurrentIndex(res);
        }
    }

    shortReadsTable->installEventFilter(this);
    QHeaderView* header = shortReadsTable->header();
    header->setStretchLastSection( false );
    header->setSectionsClickable( false );
    header->setSectionResizeMode( 0, QHeaderView::Stretch );

    initSaveController();

    sl_onAlgorithmChanged(methodNamesBox->currentText());
    connect(addShortreadsButton, SIGNAL(clicked()), SLOT(sl_onAddShortReadsButtonClicked()) );
    connect(removeShortReadsButton, SIGNAL(clicked()), SLOT(sl_onRemoveShortReadsButtonClicked()));
    connect(addRefButton, SIGNAL(clicked()), SLOT(sl_onAddRefButtonClicked()) );
    connect(methodNamesBox, SIGNAL(currentIndexChanged(const QString &)), SLOT(sl_onAlgorithmChanged(const QString &)));
    connect(samBox, SIGNAL(clicked()), SLOT(sl_onSamBoxClicked()));
    connect(libraryComboBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_onLibraryTypeChanged()));

    QString activeRefSeqUrl = refSeqUrl.isEmpty() ? lastRefSeqUrl : refSeqUrl;
    if (!activeRefSeqUrl.isEmpty()) {
        refSeqEdit->setText(activeRefSeqUrl);
        buildResultUrl(activeRefSeqUrl);
        if (NULL != customGUI) {
            QString error;
            customGUI->buildIndexUrl(lastRefSeqUrl, prebuiltIndex, error);
            customGUI->validateReferenceSequence( activeRefSeqUrl );
            //todo: process error!
        }
    }

    QStringList activeShortReadsUrl = shortReadsUrls.isEmpty() ? lastShortReadsUrls : shortReadsUrls;
    foreach(const QString& read, activeShortReadsUrl) {
        ShortReadsTableItem* item = new ShortReadsTableItem(shortReadsTable, read);
        ShortReadsTableItem::addItemToTable(item, shortReadsTable);
    }

}

void DnaAssemblyDialog::updateState() {
    addGuiExtension();
}

void DnaAssemblyDialog::sl_onAddShortReadsButtonClicked() {
    LastUsedDirHelper lod;
    QStringList fileNames = U2FileDialog::getOpenFileNames(this, tr("Add short reads"), lod.dir);
    if (fileNames.isEmpty()) {
        return;
    }
    lod.url = fileNames.at(fileNames.count() - 1);

    foreach(const QString& f, fileNames) {
        ShortReadsTableItem* item = new ShortReadsTableItem(shortReadsTable, f);
        item->setLibraryType( libraryComboBox->currentIndex() == 0 ? LIBRARY_TYPE_SINGLE : LIBRARY_TYPE_PAIRED);
        ShortReadsTableItem::addItemToTable(item, shortReadsTable);
    }
}

void DnaAssemblyDialog::sl_onAddRefButtonClicked() {
    LastUsedDirHelper lod;
    QString filter;

    lod.url = U2FileDialog::getOpenFileName(this, tr("Open reference sequence"), lod.dir, filter);
    if (lod.url.isEmpty()) {
        return;
    }

    refSeqEdit->setText(lod.url);
    buildResultUrl(lod.url);

    if (NULL != customGUI) {
        QString error;
        if (!customGUI->buildIndexUrl(lod.url, prebuiltIndex, error)) {
            QMessageBox::information(this, "DNA Assembly", error);
        }
        customGUI->validateReferenceSequence( GUrl( lod.url ) );
    }
}

void DnaAssemblyDialog::accept() {
    if (NULL != customGUI) {
        QString error;
        if (!customGUI->isParametersOk(error)) {
            if (!error.isEmpty()) {
                QMessageBox::information(this, tr("Align short reads"), error);
            }
            return;
        }
        if (!customGUI->isIndexOk(refSeqEdit->text(), error)) {
            if (!prebuiltIndex) {
                QMessageBox::StandardButton res = QMessageBox::warning(this, tr("DNA Assembly"), error, QMessageBox::Ok | QMessageBox::Cancel);
                if (QMessageBox::Cancel == res) {
                    return;
                }
            } else {
                QMessageBox::information(this, tr("DNA Assembly"), error);
                return;
            }
        }
    }

    if (refSeqEdit->text().isEmpty()) {
        QMessageBox::information(this, tr("DNA Assembly"),
            tr("Reference sequence url is not set!") );
    } else if (saveController->getSaveFileName().isEmpty() ) {
        QMessageBox::information(this, tr("DNA Assembly"),
            tr("Result alignment file name is not set!") );
    } else if (shortReadsTable->topLevelItemCount() == 0 ) {
        QMessageBox::information(this, tr("DNA Assembly"),
            tr("Short reads list is empty!") );
    } else {

        lastRefSeqUrl.clear();
        lastShortReadsUrls.clear();

        lastRefSeqUrl = refSeqEdit->text();
        int numItems = shortReadsTable->topLevelItemCount();
        for( int i =0; i < numItems; ++i) {
            lastShortReadsUrls.append(shortReadsTable->topLevelItem(i)->data(0,0).toString());
        }

        DnaAssemblyToRefTaskSettings settings = DnaAssemblyGUIUtils::getSettings(this);
        if (customGUI->isIndex(refSeqEdit->text())) {
            if (customGUI->isValidIndex(refSeqEdit->text())) {
                settings.prebuiltIndex = true;
            } else {
                QMessageBox::warning(this,
                    tr("DNA Assembly"),
                    tr("You set the index as a reference and the index files are corrupted.\n\nTry to build it again or choose a reference sequence."));
                return;
            }
        }

        QList<GUrl> unknownFormatFiles;
        QMap<QString, QString> toConvert = DnaAssemblySupport::toConvert(settings, unknownFormatFiles);
        if (!unknownFormatFiles.isEmpty()) {
            QString filesText = DnaAssemblySupport::unknownText(unknownFormatFiles);
            QMessageBox::warning(this,
                tr("DNA Assembly"),
                tr("These files have the unknown format:\n\n") + filesText);
            return;
        }
        if (!toConvert.isEmpty()) {
            QString filesText = DnaAssemblySupport::toConvertText(toConvert);
            QMessageBox::StandardButton res = QMessageBox::information(this,
                tr("DNA Assembly"),
                tr("These files have the incompatible format:\n\n") + filesText +
                tr("\n\nDo you want to convert the files and run the aligner?"),
                QMessageBox::Yes | QMessageBox::No);
            if (QMessageBox::No == res) {
                return;
            }
        }
        QDialog::accept();
    }
}

void DnaAssemblyDialog::initSaveController() {
    SaveDocumentControllerConfig config;
    config.defaultFormatId = samOutput ? BaseDocumentFormats::SAM : BaseDocumentFormats::UGENEDB;
    config.fileDialogButton = setResultFileNameButton;
    config.fileNameEdit = resultFileNameEdit;
    config.parentWidget = this;
    config.saveTitle = tr("Set result alignment file name");

    const QList<DocumentFormatId> formats = QList<DocumentFormatId>() << BaseDocumentFormats::SAM
                                                                      << BaseDocumentFormats::UGENEDB;

    saveController = new SaveDocumentController(config, formats, this);

    connect(saveController, SIGNAL(si_formatChanged(const QString &)), SLOT(sl_formatChanged(const QString &)));
}

void DnaAssemblyDialog::buildResultUrl(const QString &refUrl) {
    if (saveController->getSaveFileName().isEmpty()) {
        const QString formatId = saveController->getFormatIdToSave();
        const QFileInfo refinfo(refUrl);
        const QString path = refinfo.path() + "/" + refinfo.completeBaseName();
        saveController->setPath(path);
        saveController->setFormat(formatId);
    }
}

const GUrl DnaAssemblyDialog::getRefSeqUrl() {
    return refSeqEdit->text();
}

const QList<ShortReadSet> DnaAssemblyDialog::getShortReadSets()
{
    QList<ShortReadSet> sets;

    int numItems = shortReadsTable->topLevelItemCount();

    for( int i =0; i < numItems; ++i) {
        ShortReadsTableItem* item = static_cast<ShortReadsTableItem*> ( shortReadsTable->topLevelItem(i) );
        sets.append( ShortReadSet(item->getUrl(), item->getType(), item->getOrder()));

    }
    return sets;

}

const QString DnaAssemblyDialog::getAlgorithmName() {
    return methodNamesBox->currentText();
}

void DnaAssemblyDialog::sl_onRemoveShortReadsButtonClicked() {
    int currentRow = shortReadsTable->currentIndex().row();
    shortReadsTable->takeTopLevelItem(currentRow);

}

void DnaAssemblyDialog::sl_onAlgorithmChanged(const QString &text) {
    methodName = text;
    updateState();
}

void DnaAssemblyDialog::sl_onSamBoxClicked() {
    samOutput = samBox->isChecked();
    saveController->setFormat(samOutput ? BaseDocumentFormats::SAM : BaseDocumentFormats::UGENEDB);
}

const QString DnaAssemblyDialog::getResultFileName() {
    return saveController->getSaveFileName();
}

bool DnaAssemblyDialog::isPaired() const {
    return libraryComboBox->currentIndex() == 0 ? false : true;
}

bool DnaAssemblyDialog::isSamOutput() const {
    return samBox->isChecked();
}

bool DnaAssemblyDialog::isPrebuiltIndex() const {
    CHECK(NULL != customGUI, false);
    return customGUI->isIndex(refSeqEdit->text());
}

QMap<QString, QVariant> DnaAssemblyDialog::getCustomSettings() {
    if (customGUI != NULL) {
        return customGUI->getDnaAssemblyCustomSettings();
    } else {
        return QMap<QString, QVariant>();
    }
}

void DnaAssemblyDialog::addGuiExtension() {
    static const int insertPos = verticalLayout->count() - 2;

    int macFixDelta = 50;

    // cleanup previous extension
    if (customGUI != NULL) {
        layout()->removeWidget(customGUI);
        setMinimumHeight(minimumHeight() - customGUI->minimumHeight());
        delete customGUI;
        customGUI = NULL;
        macFixDelta = 0;
    }

    // insert new extension widget
    DnaAssemblyAlgorithmEnv* env = assemblyRegistry->getAlgorithm(methodNamesBox->currentText());

    if (NULL == env) {
        adjustSize();
        return;
    }

    if (!env->supportsPairedEndLibrary()) {
        libraryComboBox->setEnabled(false);
        libraryComboBox->setCurrentIndex(0);
    } else {
        libraryComboBox->setEnabled(true);
    }

    if (!env->isDbiSupported()) {
        samBox->setChecked(true);
        samBox->setEnabled(false);
        if (!samOutput) {
            samOutput = true;
            sl_onSamBoxClicked();
        }
    } else {
        samBox->setEnabled(true);
    }
    DnaAssemblyGUIExtensionsFactory* gui = env->getGUIExtFactory();
    if (gui!=NULL && gui->hasMainWidget()) {
        customGUI = gui->createMainWidget(this);
        int extensionMinWidth = customGUI->sizeHint().width();
        int extensionMinHeight = customGUI->sizeHint().height();
        customGUI->setMinimumWidth(extensionMinWidth);
        customGUI->setMinimumHeight(extensionMinHeight);
        verticalLayout->insertWidget(insertPos, customGUI);
        // adjust sizes
        // HACK: add 50 to min height when dialog first shown, 50 to width always (fix for Mac OS)
        // TODO: handle margins in proper way so this hack not needed
        setMinimumHeight(customGUI->minimumHeight() + minimumHeight() + macFixDelta);
        if (minimumWidth() < customGUI->minimumWidth() + 50) {
            setMinimumWidth(customGUI->minimumWidth() + 50);
        };
        if (!refSeqEdit->text().isEmpty()) {
            QString error;
            customGUI->buildIndexUrl(refSeqEdit->text(), prebuiltIndex, error);
            customGUI->validateReferenceSequence( GUrl( refSeqEdit->text( ) ) );
        }
        customGUI->show();
        adjustSize();
    } else {
        adjustSize();
    }
}

bool DnaAssemblyDialog::eventFilter( QObject * obj, QEvent * event ) {
    if (obj == shortReadsTable) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Delete) {
                sl_onRemoveShortReadsButtonClicked();
            }
            return true;
        } else {
            return false;
        }
    } else {
        // pass the event on to the parent class
        return QDialog::eventFilter(obj, event);
    }
}

void DnaAssemblyDialog::sl_onLibraryTypeChanged()
{
    int count = shortReadsTable->topLevelItemCount();
    for (int i = 0; i < count; ++i) {
        ShortReadsTableItem* item = static_cast<ShortReadsTableItem*> (shortReadsTable->topLevelItem(i));
        item->setLibraryType( libraryComboBox->currentIndex() == 0 ? LIBRARY_TYPE_SINGLE : LIBRARY_TYPE_PAIRED );
    }
}

void DnaAssemblyDialog::sl_formatChanged(const QString &newFormat) {
    samBox->setChecked(BaseDocumentFormats::SAM == newFormat);
}

DnaAssemblyToRefTaskSettings DnaAssemblyGUIUtils::getSettings(DnaAssemblyDialog *dialog) {
    DnaAssemblyToRefTaskSettings s;
    SAFE_POINT(NULL != dialog, "NULL dialog", s);
    s.samOutput = dialog->isSamOutput();
    s.refSeqUrl = dialog->getRefSeqUrl();
    s.algName = dialog->getAlgorithmName();
    s.resultFileName = dialog->getResultFileName();
    s.setCustomSettings(dialog->getCustomSettings());
    s.shortReadSets = dialog->getShortReadSets();
    s.pairedReads = dialog->isPaired();

    return s;
}

void DnaAssemblyGUIUtils::runAssembly2ReferenceDialog(const QStringList& shortReadUrls, const QString& refSeqUrl) {
    QObjectScopedPointer<DnaAssemblyDialog> dlg = new DnaAssemblyDialog(QApplication::activeWindow(), shortReadUrls, refSeqUrl);
    dlg->exec();
    CHECK(!dlg.isNull(), );

    if (QDialog::Accepted == dlg->result()) {
        DnaAssemblyToRefTaskSettings s = getSettings(dlg.data());
        s.openView = true;
        Task* assemblyTask = new DnaAssemblyTaskWithConversions(s, true);
        AppContext::getTaskScheduler()->registerTopLevelTask(assemblyTask);
    }
}

ShortReadsTableItem::ShortReadsTableItem(QTreeWidget *treeWidget, const QString& url) : QTreeWidgetItem(treeWidget)
{

    mateTypeBox = new QComboBox(treeWidget);
    mateTypeBox->addItem(MATE_UPSTREAM);
    mateTypeBox->addItem(MATE_DOWNSTREAM);

    setData(0, 0, url);
    setData(1, 0, LIBRARY_TYPE_SINGLE);
}

GUrl ShortReadsTableItem::getUrl() const
{
    return data(0,0).toString();
}

ShortReadSet::LibraryType ShortReadsTableItem::getType() const
{
    return data(1,0).toString() == LIBRARY_TYPE_PAIRED ? ShortReadSet::PairedEndReads : ShortReadSet::SingleEndReads;
}

ShortReadSet::MateOrder ShortReadsTableItem::getOrder() const
{
    return mateTypeBox->currentText() == MATE_DOWNSTREAM ? ShortReadSet::DownstreamMate : ShortReadSet::UpstreamMate;
}

void ShortReadsTableItem::addItemToTable(ShortReadsTableItem *item, QTreeWidget *treeWidget)
{
    treeWidget->addTopLevelItem(item);
    treeWidget->setItemWidget(item,2,item->mateTypeBox);
}

void ShortReadsTableItem::setLibraryType( const QString& libraryType )
{
    setData(1,0, libraryType);
    if (libraryType == LIBRARY_TYPE_SINGLE) {
        mateTypeBox->setCurrentIndex(0);
        mateTypeBox->setEnabled(false);
    } else {
        mateTypeBox->setEnabled(true);
    }
}



} // U2
