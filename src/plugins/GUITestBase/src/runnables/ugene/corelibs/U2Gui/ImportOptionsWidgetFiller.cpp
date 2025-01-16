/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "ImportOptionsWidgetFiller.h"
#include <primitives/GTCheckBox.h>
#include <primitives/GTLineEdit.h>
#include <primitives/GTRadioButton.h>
#include <primitives/GTSpinBox.h>
#include <primitives/GTWidget.h>

#include <QCheckBox>
#include <QRadioButton>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/U2SafePoints.h>

#include <U2Formats/AceImporter.h>

#include <U2Gui/ImportOptionsWidget.h>

namespace U2 {

#define GT_CLASS_NAME "GTUtilsDialog::ImportOptionsWidgetFiller"

void ImportOptionsWidgetFiller::fill(ImportOptionsWidget* optionsWidget, const QVariantMap& data) {
    GT_CHECK(optionsWidget != nullptr, "optionsWidget is NULL");

    setDestinationFolder(optionsWidget, data);
    setKeepFoldersStructure(optionsWidget, data);
    setProcessFoldersRecursively(optionsWidget, data);
    setCreateSubfolderForTopLevelFolder(optionsWidget, data);
    setCreateSubfolderForEachFile(optionsWidget, data);
    setImportUnknownAsUdr(optionsWidget, data);
    setMultiSequencePolicy(optionsWidget, data);
    setAceFormatPolicy(optionsWidget, data);
    setCreateSubfolderForDocument(optionsWidget, data);
}

void ImportOptionsWidgetFiller::setDestinationFolder(ImportOptionsWidget* optionsWidget, const QVariantMap& data) {
    CHECK(data.contains(ImportToDatabaseOptions::DESTINATION_FOLDER), );
    QString destinationPath = data.value(ImportToDatabaseOptions::DESTINATION_FOLDER).toString();
    GTLineEdit::setText("leBaseFolder", destinationPath, optionsWidget);
}

void ImportOptionsWidgetFiller::setKeepFoldersStructure(ImportOptionsWidget* optionsWidget, const QVariantMap& data) {
    CHECK(data.contains(ImportToDatabaseOptions::KEEP_FOLDERS_STRUCTURE), );
    auto keepStructure = GTWidget::findCheckBox("cbKeepStructure", optionsWidget);
    GTCheckBox::setChecked(keepStructure, data.value(ImportToDatabaseOptions::KEEP_FOLDERS_STRUCTURE).toBool());
}

void ImportOptionsWidgetFiller::setProcessFoldersRecursively(ImportOptionsWidget* optionsWidget, const QVariantMap& data) {
    CHECK(data.contains(ImportToDatabaseOptions::PROCESS_FOLDERS_RECUSIVELY), );
    auto processRecursively = GTWidget::findCheckBox("cbRecursively", optionsWidget);
    GTCheckBox::setChecked(processRecursively, data.value(ImportToDatabaseOptions::PROCESS_FOLDERS_RECUSIVELY).toBool());
}

void ImportOptionsWidgetFiller::setCreateSubfolderForTopLevelFolder(ImportOptionsWidget* optionsWidget, const QVariantMap& data) {
    CHECK(data.contains(ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_TOP_LEVEL_FOLDER), );
    auto subfolderForEachFolder = GTWidget::findCheckBox("cbCreateSubfoldersForTopLevelFolder", optionsWidget);
    GTCheckBox::setChecked(subfolderForEachFolder, data.value(ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_TOP_LEVEL_FOLDER).toBool());
}

void ImportOptionsWidgetFiller::setCreateSubfolderForEachFile(ImportOptionsWidget* optionsWidget, const QVariantMap& data) {
    CHECK(data.contains(ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_EACH_FILE), );
    auto subfolderForEachFile = GTWidget::findCheckBox("cbCreateSubfoldersForFiles", optionsWidget);
    GTCheckBox::setChecked(subfolderForEachFile, data.value(ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_EACH_FILE).toBool());
}

void ImportOptionsWidgetFiller::setImportUnknownAsUdr(ImportOptionsWidget* optionsWidget, const QVariantMap& data) {
    CHECK(data.contains(ImportToDatabaseOptions::IMPORT_UNKNOWN_AS_UDR), );
    auto importUnknown = GTWidget::findCheckBox("cbImportUnrecognized", optionsWidget);
    GTCheckBox::setChecked(importUnknown, data.value(ImportToDatabaseOptions::IMPORT_UNKNOWN_AS_UDR).toBool());
}

void ImportOptionsWidgetFiller::setMultiSequencePolicy(ImportOptionsWidget* optionsWidget, const QVariantMap& data) {
    CHECK(data.contains(ImportToDatabaseOptions::MULTI_SEQUENCE_POLICY), );

    ImportToDatabaseOptions::MultiSequencePolicy policy = static_cast<ImportToDatabaseOptions::MultiSequencePolicy>(data.value(ImportToDatabaseOptions::MULTI_SEQUENCE_POLICY).toInt());
    switch (policy) {
        case ImportToDatabaseOptions::SEPARATE: {
            auto separate = GTWidget::findRadioButton("rbSeparate", optionsWidget);
            GTRadioButton::click(separate);
            break;
        }
        case ImportToDatabaseOptions::MERGE: {
            auto merge = GTWidget::findRadioButton("rbMerge", optionsWidget);
            GTRadioButton::click(merge);
            setMergeMultiSequencePolicySeparatorSize(optionsWidget, data);
            break;
        }
        case ImportToDatabaseOptions::MALIGNMENT: {
            auto malignment = GTWidget::findRadioButton("rbMalignment", optionsWidget);
            GTRadioButton::click(malignment);
            break;
        }
    }
}

void ImportOptionsWidgetFiller::setAceFormatPolicy(ImportOptionsWidget* optionsWidget, const QVariantMap& data) {
    CHECK(data.contains(ImportToDatabaseOptions::PREFERRED_FORMATS), );

    const QStringList preferredFormats = data.value(ImportToDatabaseOptions::PREFERRED_FORMATS).toStringList();

    if (preferredFormats.contains(BaseDocumentFormats::ACE)) {
        auto rbAceAsMalignment = GTWidget::findRadioButton("rbAceAsMalignment", optionsWidget);
        GTRadioButton::click(rbAceAsMalignment);
    } else if (preferredFormats.contains(AceImporter::ID)) {
        auto rbAceAsAssembly = GTWidget::findRadioButton("rbAceAsAssembly", optionsWidget);
        GTRadioButton::click(rbAceAsAssembly);
    }
}

void ImportOptionsWidgetFiller::setCreateSubfolderForDocument(ImportOptionsWidget* optionsWidget, const QVariantMap& data) {
    CHECK(data.contains(ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_DOCUMENT), );
    auto subfolderForDocument = GTWidget::findCheckBox("cbCreateSubfoldersforDocs", optionsWidget);
    GTCheckBox::setChecked(subfolderForDocument, data.value(ImportToDatabaseOptions::CREATE_SUBFOLDER_FOR_DOCUMENT).toBool());
}

void ImportOptionsWidgetFiller::setMergeMultiSequencePolicySeparatorSize(ImportOptionsWidget* optionsWidget, const QVariantMap& data) {
    CHECK(data.contains(ImportToDatabaseOptions::MERGE_MULTI_SEQUENCE_POLICY_SEPARATOR_SIZE), );
    auto separatorSize = GTWidget::findSpinBox("sbMerge", optionsWidget);
    GTSpinBox::setValue(separatorSize, data.value(ImportToDatabaseOptions::MERGE_MULTI_SEQUENCE_POLICY_SEPARATOR_SIZE).toInt());
}

#undef GT_CLASS_NAME

}  // namespace U2
