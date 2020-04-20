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

#include <U2Algorithm/GenomeAssemblyRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/FileAndDirectoryUtils.h>

#include <U2Gui/AppSettingsGUI.h>
#include <U2Gui/HelpButton.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/DnaAssemblyUtils.h>
#include <U2View/DnaAssemblyGUIExtension.h>

#include "GenomeAssemblyDialog.h"

namespace U2 {

QString GenomeAssemblyDialog::methodName;
QString GenomeAssemblyDialog::library;

GenomeAssemblyDialog::GenomeAssemblyDialog(QWidget* p)
: QDialog(p),
  assemblyRegistry(AppContext::getGenomeAssemblyAlgRegistry()),
  customGUI(NULL)
{
    setupUi(this);

    QMap<QString,QString> helpPagesMap;
    helpPagesMap.insert("SPAdes","24749031");
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

//    libraryComboBox->addItems(GenomeAssemblyUtils::getLibraryTypes());

    QHeaderView* header1 = propertiesReadsTable->header();
    QHeaderView* header2 = leftReadsTable->header();
    QHeaderView* header3 = rightReadsTable->header();

    header1->setStretchLastSection( false );
    header2->setStretchLastSection( false );
    header3->setStretchLastSection( false );
    header1->setSectionsClickable( false );
    header1->setSectionResizeMode( 0, QHeaderView::Stretch );
    header2->setSectionsClickable( false );
    header2->setSectionResizeMode( 0, QHeaderView::Stretch );
    header3->setSectionsClickable( false );
    header3->setSectionResizeMode( 0, QHeaderView::Stretch );

    sl_onLibraryTypeChanged();
    sl_onAlgorithmChanged(methodNamesBox->currentText());

    connect(addLeftButton, SIGNAL(clicked()), SLOT(sl_onAddShortReadsButtonClicked()) );
    connect(addRightButton, SIGNAL(clicked()), SLOT(sl_onAddShortReadsButtonClicked()) );
    connect(removeLeftButton, SIGNAL(clicked()), SLOT(sl_onRemoveShortReadsButtonClicked()));
    connect(removeRightButton, SIGNAL(clicked()), SLOT(sl_onRemoveShortReadsButtonClicked()));
    connect(setResultDirNameButton, SIGNAL(clicked()), SLOT(sl_onOutDirButtonClicked()));
    connect(methodNamesBox, SIGNAL(currentIndexChanged(const QString &)), SLOT(sl_onAlgorithmChanged(const QString &)));
    connect(libraryComboBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_onLibraryTypeChanged()));

    QString defaultOutputDir = GUrlUtils::getDefaultDataPath() + "/" + methodNamesBox->currentText() + "_output";
    resultDirNameEdit->setText(GUrlUtils::rollFileName(defaultOutputDir, QSet<QString>() << defaultOutputDir));

    if (!library.isEmpty()) {
        int index = libraryComboBox->findText(library);
        if (index != -1) {
            libraryComboBox->setCurrentIndex(index);
        }
    }
}

void GenomeAssemblyDialog::updateState() {
    addGuiExtension();
}

void GenomeAssemblyDialog::updateProperties()
{
    int numProperties = propertiesReadsTable->topLevelItemCount();
    int numberOfReads =  leftReadsTable->topLevelItemCount();
//    if(GenomeAssemblyUtils::hasRightReads(libraryComboBox->currentText())){
//        numberOfReads = qMax(leftReadsTable->topLevelItemCount(), rightReadsTable->topLevelItemCount());
//    }
    if(numProperties > numberOfReads){
        //remove items
        for (int i = numProperties - 1; i >= numberOfReads; --i){
            propertiesReadsTable->takeTopLevelItem(i);
        }
    }else if (numProperties < numberOfReads){
        //add items
        for (int i = numProperties; i < numberOfReads; i++){
            ReadPropertiesItem* item = new ReadPropertiesItem(propertiesReadsTable);
            item->setLibraryType( libraryComboBox->currentText());
            ReadPropertiesItem::addItemToTable(item, propertiesReadsTable);

        }
    }
    //update numbers
    numProperties = propertiesReadsTable->topLevelItemCount();
    for( int i =0; i < numProperties; ++i) {
        QTreeWidgetItem* item = propertiesReadsTable->topLevelItem(i);
        item->setData(0, 0, i+1);
    }
}

void GenomeAssemblyDialog::addReads(QStringList fileNames, QTreeWidget* readsWidget){
    foreach(const QString& f, fileNames) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setToolTip(0, f);
        item->setText(0, GUrl(f).fileName());
        item->setData(0, Qt::UserRole, f);
        readsWidget->addTopLevelItem(item);
        item->setSizeHint(0, QComboBox().sizeHint());
    }

    updateProperties();
}

void GenomeAssemblyDialog::sl_onAddShortReadsButtonClicked() {
    QTreeWidget* readsWidget = NULL;
    QObject* obj = sender();
    if( obj == addLeftButton ){
        readsWidget = leftReadsTable;
    }else if (obj == addRightButton){
        readsWidget = rightReadsTable;
    }else{
        return;
    }

    LastUsedDirHelper lod("AssemblyReads");
    QStringList fileNames;
#ifdef Q_OS_MAC
    if (qgetenv(ENV_GUI_TEST).toInt() == 1 && qgetenv(ENV_USE_NATIVE_DIALOGS).toInt() == 0) {
        fileNames = U2FileDialog::getOpenFileNames(this, tr("Add short reads"), lod.dir, QString(), 0, QFileDialog::DontUseNativeDialog );
    } else
#endif
    fileNames = U2FileDialog::getOpenFileNames(this, tr("Add short reads"), lod.dir);
    if (fileNames.isEmpty()) {
        return;
    }
    lod.url = fileNames.at(fileNames.count() - 1);

    addReads(fileNames, readsWidget);
}

void GenomeAssemblyDialog::accept() {
    bool validated = true;
    if (NULL != customGUI) {
        QString error;
        if (!customGUI->isParametersOk(error)) {
            if (!error.isEmpty()) {
                QMessageBox::information(this, tr("Genome Assembly"), error);
            }
            validated = false;
        }
    }

    if (resultDirNameEdit->text().isEmpty() ) {
        QMessageBox::information(this, tr("Genome Assembly"),
            tr("Result assembly folder is not set!") );
        validated = false;
    } else {
//        if(GenomeAssemblyUtils::hasRightReads(libraryComboBox->currentText())){
//            if(leftReadsTable->topLevelItemCount() == 0 && rightReadsTable->topLevelItemCount() == 0){
//                QMessageBox::information(this, tr("Genome Assembly"),
//                    tr("No reads. Please, add file(s) with short reads.") );
//                validated = false;
//            }

//            if(leftReadsTable->topLevelItemCount() != rightReadsTable->topLevelItemCount()){
//                QMessageBox::information(this, tr("Genome Assembly"),
//                    tr("In the paired-end mode a number of lift and right reads must be equal.") );
//                validated = false;
//            }

//        }else{
//            if(leftReadsTable->topLevelItemCount() == 0){
//                QMessageBox::information(this, tr("Genome Assembly"),
//                    tr("No reads. Please, add file(s) with short reads.") );
//                validated = false;
//            }
//        }

        if(validated){
            library = libraryComboBox->currentText();
            library = libraryComboBox->currentText();

            //check formats
            QStringList reads;
            int numItems = leftReadsTable->topLevelItemCount();
            for (int i = 0; i < numItems; ++i) {
                reads.append(leftReadsTable->topLevelItem(i)->data(0, Qt::UserRole).toString());
            }

            numItems = rightReadsTable->topLevelItemCount();
            for (int i = 0; i < numItems; ++i) {
                reads.append(rightReadsTable->topLevelItem(i)->data(0, Qt::UserRole).toString());
            }

            GenomeAssemblyAlgorithmEnv *env = AppContext::getGenomeAssemblyAlgRegistry()->getAlgorithm(methodNamesBox->currentText());
            SAFE_POINT(NULL != env, "Unknown algorithm: " + methodNamesBox->currentText(), );
            QStringList formats = env->getReadsFormats();

            foreach(const QString& r, reads) {
                const QString detectedFormat = FileAndDirectoryUtils::detectFormat(r);
                if (detectedFormat.isEmpty()) {
                    QMessageBox::information(this, tr("Genome Assembly"),
                        tr("Unknown file format of %1.").arg(r));
                    return;
                }

                if (!formats.contains(detectedFormat)) {
                    QMessageBox::information(this, tr("Genome Assembly"),
                        tr("File format of %1 is %2. Supported file formats of reads: %3.").arg(r).arg(detectedFormat).arg(formats.join(", ")));
                    return;
                }
            }
            QString outputDirUrl = resultDirNameEdit->text();
            QDir d(outputDirUrl);
            if (!d.exists()) {
                if (!d.mkdir(outputDirUrl)) {
                    QMessageBox::information(this, tr("Genome Assembly"),
                        tr("Unable to create output folder for result assembly.\r\nDirectory Path: %1").arg(outputDirUrl));
                }
            }
            QDialog::accept();
        }
    }
}

const QString GenomeAssemblyDialog::getAlgorithmName() {
    return methodNamesBox->currentText();
}

const QString GenomeAssemblyDialog::getOutDir() {
    return resultDirNameEdit->text();
}

QList<AssemblyReads> GenomeAssemblyDialog::getReads() {
    QList<AssemblyReads> result;

    int numProperties = propertiesReadsTable->topLevelItemCount();
    int numLeftReads =  leftReadsTable->topLevelItemCount();
    int numRightReads =  rightReadsTable->topLevelItemCount();

    for (int i = 0; i < numProperties; i++){
        AssemblyReads read;
        QTreeWidgetItem* item = propertiesReadsTable->topLevelItem(i);
        ReadPropertiesItem* pitem = dynamic_cast<ReadPropertiesItem*>(item);
        if (pitem){
//            read.libNumber = pitem->getNumber();
//            read.libType = pitem->getType();
            read.orientation = pitem->getOrientation();
            read.libName = libraryComboBox->currentText();

            if (i < numLeftReads){
                read.left << leftReadsTable->topLevelItem(i)->data(0, Qt::UserRole).toString();
                if(i < numRightReads){
                    read.right << rightReadsTable->topLevelItem(i)->data(0, Qt::UserRole).toString();
                }
                result.append(read);
            }
        }
    }
    return result;
}

void GenomeAssemblyDialog::sl_onRemoveShortReadsButtonClicked() {
    QTreeWidget* readsWidget = NULL;
    QObject* obj = sender();
    if( obj == removeLeftButton ){
        readsWidget = leftReadsTable;
    }else if (obj == removeRightButton){
        readsWidget = rightReadsTable;
    }else{
        return;
    }

    int currentRow = readsWidget->currentIndex().row();
    readsWidget->takeTopLevelItem(currentRow);

    updateProperties();
}

void GenomeAssemblyDialog::sl_onOutDirButtonClicked(){
    LastUsedDirHelper lod("assemblyRes");

    lod.url = U2FileDialog::getExistingDirectory(this, tr("Select output folder"), lod.dir);
    if (lod.url.isEmpty()) {
        return;
    }

    resultDirNameEdit->setText(lod.url);
}


void GenomeAssemblyDialog::sl_onAlgorithmChanged(const QString &text) {
    methodName = text;
    updateState();
}

QMap<QString, QVariant> GenomeAssemblyDialog::getCustomSettings() {
    if (customGUI != NULL) {
        return customGUI->getGenomeAssemblyCustomSettings();
    } else {
        return QMap<QString, QVariant>();
    }
}

void GenomeAssemblyDialog::addGuiExtension() {
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
    GenomeAssemblyAlgorithmEnv* env = assemblyRegistry->getAlgorithm(methodNamesBox->currentText());

    if (NULL == env) {
        adjustSize();
        return;
    }

    GenomeAssemblyGUIExtensionsFactory* gui = env->getGUIExtFactory();
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

        customGUI->show();
        adjustSize();
    } else {
        adjustSize();
    }

}

void GenomeAssemblyDialog::sl_onLibraryTypeChanged(){
    QString libraryType = libraryComboBox->currentText();
//    if(GenomeAssemblyUtils::hasRightReads(libraryType)){
//        rightReadsTable->setEnabled(true);
//        addRightButton->setEnabled(true);
//        removeRightButton->setEnabled(true);
//    }else{
//        rightReadsTable->setEnabled(false);
//        addRightButton->setEnabled(false);
//        removeRightButton->setEnabled(false);
//    }

    int size = propertiesReadsTable->topLevelItemCount();
    for (int i = 0; i < size; i++){
        QTreeWidgetItem* item = propertiesReadsTable->topLevelItem(i);
        ReadPropertiesItem* pitem = dynamic_cast<ReadPropertiesItem*>(item);
        if (pitem){
            pitem->setLibraryType(libraryType);
        }
    }

    updateProperties();
}

ReadPropertiesItem::ReadPropertiesItem(QTreeWidget *widget) : QTreeWidgetItem(widget){
    typeBox = new QComboBox(widget);
//    typeBox->addItems(GenomeAssemblyUtils::getPairTypes());

    orientationBox = new QComboBox(widget);
    orientationBox->addItems(GenomeAssemblyUtils::getOrientationTypes());
}

QString ReadPropertiesItem::getNumber() const{
    return data(0,0).toString();
}

QString ReadPropertiesItem::getType() const{
    return typeBox->currentText();
}

QString ReadPropertiesItem::getOrientation() const{
    return orientationBox->currentText();
}

void ReadPropertiesItem::setLibraryType( const QString& libraryType )
{
    if (GenomeAssemblyUtils::isLibraryPaired(libraryType)) {
        orientationBox->setEnabled(true);
        typeBox->setEnabled(true);
    } else {
        orientationBox->setEnabled(false);
        typeBox->setEnabled(false);
    }
}

void ReadPropertiesItem::addItemToTable(ReadPropertiesItem *item, QTreeWidget *treeWidget){
    treeWidget->addTopLevelItem(item);
    treeWidget->setItemWidget(item,1,item->typeBox);
    treeWidget->setItemWidget(item,2,item->orientationBox);
}

} // U2
