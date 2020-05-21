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

#include "ExtractAnnotationsBySequenceDialogController.h"

#include <U2Core/AppContext.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/SaveDocumentController.h>

namespace U2 {

ExtractAnnotationsBySequenceDialogController::ExtractAnnotationsBySequenceDialogController(const QString &fileName, int fileSizeMb, 
    const DocumentFormatId &formatId, const QStringList &entries, QWidget *parent /* = 0 */)
    : QDialog(parent) 
{
    //"Help", "Open file", "Extract by sequences", "Cancel"
    setupUi(this);
    new HelpButton(this, buttonBox, HelpButton::INVALID_VALUE);

    messageLabel->setText(tr(   "The size of the \"%1\" is %2 Mb. It may take some time to parse the data.\n"
                                "Probably, the file contains annotations that belong to different sequences (chromosomes).\n"
                                "In case only some sequences are investigated, it is recommended to extract annotations that"
                                "belong to these sequences into a separate file.\n\n"
    
                                "Usage of the extracted annotations file of a smaller size will help to decrease the time needed for loading the data.")
        .arg(fileName)
        .arg(QString::number(fileSizeMb)));

    SaveDocumentControllerConfig conf;
    conf.defaultFileName = "choosen_annotations." + AppContext::getDocumentFormatRegistry()->getFormatById(formatId)->getSupportedDocumentFileExtensions().first();
    conf.defaultFormatId = formatId;
    conf.fileDialogButton = browseButton;
    conf.fileNameEdit = filepathLineEdit;
    conf.parentWidget = this;
    conf.saveTitle = tr("Save File");

    saveController = new SaveDocumentController(conf, QList<DocumentFormatId>() << formatId, this);

    annotationsSelectorTable->setColumnCount(2);
    //annotationsSelectorTable->setRowCount(bamInfo.getHeader().getReferences().count());
    QStringList header;
    header << tr("Export") << tr("Annotation");
    annotationsSelectorTable->setHorizontalHeaderLabels(header);
    annotationsSelectorTable->horizontalHeader()->setStretchLastSection(true);
    int i = 0;
    foreach (const QString &entry, entries) {
        QTableWidgetItem *checkbox = new QTableWidgetItem();
        checkbox->setCheckState(Qt::Unchecked);
        checkbox->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        annotationsSelectorTable->setItem(i, 0, checkbox);
        QTableWidgetItem *item = new QTableWidgetItem(entry);
        item->setFlags(Qt::ItemIsEnabled);
        annotationsSelectorTable->setItem(i, 1, item);
        i++;
    }
    //annotationsSelectorTable->verticalHeader()->setDefaultSectionSize(QFontMetrics(QFont()).height() + 5);
}

}