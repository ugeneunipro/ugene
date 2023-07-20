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

#pragma once

#include <GTGlobals.h>

#include <QMessageBox>

#include <U2Core/GUrl.h>

#include "GTUtilsDocument.h"

namespace U2 {

class Document;
class ADVSingleSequenceWidget;

class GTUtilsProject {
public:
    class OpenFileSettings {
    public:
        enum OpenMethod {
            Dialog = 1,
            DragDrop = 2,
        };
        OpenFileSettings();
        OpenMethod openMethod = Dialog;
    };

    enum ProjectCheckType {
        Exists = 1,
        ExistsAndEmpty = 2,
        NotExists = 3,
    };

    /* Initiates file open dialog for a single file and waits until all tasks finished before the return. Asserts that the project exists. */
    static void openFile(const GUrl& path, const OpenFileSettings& settings = {}, const ProjectCheckType& checkType = Exists);

    /* Initiates file open dialog for multiple files and waits until all tasks finished before the return. Asserts that the project exists. */
    static void openFiles(const QList<QUrl>& urls, const OpenFileSettings& settings = {}, const ProjectCheckType& checkType = Exists);

    /* Initiates file open dialog for a single file and waits until all tasks finished before the return. Asserts that the project does not exist. */
    static void openFileExpectNoProject(const GUrl& path, const OpenFileSettings& s = {});

    static void checkProject(const ProjectCheckType& checkType = Exists);

    /**
     * Opens file @path\@fileName.
     * Verifies that a Sequence View was opened with one sequence.
     * Verifies that the sequence name is @seqName.
     * Returns the sequence widget.
     */
    static ADVSingleSequenceWidget* openFileExpectSequence(
        const QString& dirPath,
        const QString& fileName,
        const QString& seqName);
    static ADVSingleSequenceWidget* openFileExpectSequence(
        const QString& filePath,
        const QString& seqName);

    /**
     * Opens file @path\@fileName and expects a sequence with raw alphabet to be opened.
     * Verifies that a Sequence View was opened with one sequence.
     * Verifies that the sequence name is @seqName.
     * Returns the sequence widget.
     */
    static ADVSingleSequenceWidget* openFileExpectRawSequence(
        const QString& dirPath,
        const QString& fileName,
        const QString& seqName);

    static ADVSingleSequenceWidget* openFileExpectRawSequence(
        const QString& filePath,
        const QString& seqName);

    /**
     * Opens file @path\@fileName.
     * Verifies that a Sequence View was opened and contains sequences with the specified name @seqNames (in
     * the order specified in the list).
     * Returns the sequences widgets (in the same order).
     */
    static QList<ADVSingleSequenceWidget*> openFileExpectSequences(const QString& path, const QString& fileName, const QList<QString>& seqNames);

    static void openMultiSequenceFileAsSequences(const QString& path, const QString& fileName);

    static void openMultiSequenceFileAsSequences(const QString& filePath);

    static void openMultiSequenceFileAsMergedSequence(const QString& filePath);

    static void openMultiSequenceFileAsMalignment(const QString& dirPath, const QString& fileName);

    static void openMultiSequenceFileAsMalignment(const QString& filePath);

    static void saveProjectAs(const QString& path);

    static void closeProject(bool isExpectSaveProjectDialog, bool isExpectAppMessageBox = false);

protected:
    static void openFilesDrop(const QList<QUrl>& urls);
    static void openFilesWithDialog(const QList<QUrl>& filePaths);
};

}  // namespace U2
