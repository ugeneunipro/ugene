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

#ifndef _U2_PROJECT_UTILS_H_
#define _U2_PROJECT_UTILS_H_

#include <QMessageBox>

#include <U2Core/GUrl.h>

#include <GTGlobals.h>
#include <utils/GTUtilsApp.h>

#include "GTUtilsDocument.h"

namespace U2 {

class Document;
class ADVSingleSequenceWidget;

class GTUtilsProject {
public:
    class OpenFileSettings {
    public:
        enum OpenMethod {Dialog, DragDrop};

        OpenFileSettings();

        OpenMethod openMethod;
    };

    /*
        opens files using settings, checks if the document is loaded
    */
    static void openFiles(HI::GUITestOpStatus &os, const QList<QUrl> &urls, const OpenFileSettings& s = OpenFileSettings());
    static void openFiles(HI::GUITestOpStatus &os, const GUrl &path, const OpenFileSettings& s = OpenFileSettings());

    enum CheckType {Exists, Empty, NotExists};
    static void checkProject(HI::GUITestOpStatus &os, CheckType checkType = Exists);


    /**
     * Opens file @path\@fileName.
     * Verifies that a Sequence View was opened with one sequence.
     * Verifies that the sequence name is @seqName.
     * Returns the sequence widget.
     */
    static ADVSingleSequenceWidget *openFileExpectSequence(HI::GUITestOpStatus &os,
                                                            const QString &dirPath,
                                                            const QString &fileName,
                                                            const QString &seqName);
    static ADVSingleSequenceWidget *openFileExpectSequence(HI::GUITestOpStatus &os,
                                                            const QString &filePath,
                                                            const QString &seqName);

     /**
     * Opens file @path\@fileName and expects a sequence with raw alphabet to be opened.
     * Verifies that a Sequence View was opened with one sequence.
     * Verifies that the sequence name is @seqName.
     * Returns the sequence widget.
     */
    static ADVSingleSequenceWidget *openFileExpectRawSequence(HI::GUITestOpStatus &os,
                                                              const QString &dirPath,
                                                              const QString &fileName,
                                                              const QString &seqName);

    static ADVSingleSequenceWidget *openFileExpectRawSequence(HI::GUITestOpStatus &os,
                                                              const QString &filePath,
                                                              const QString &seqName);

    /**
     * Opens file @path\@fileName.
     * Verifies that a Sequence View was opened and contains sequences with the specified name @seqNames (in
     * the order specified in the list).
     * Returns the sequences widgets (in the same order).
     */
    static QList<ADVSingleSequenceWidget *> openFileExpectSequences(HI::GUITestOpStatus &os, const QString &path, const QString &fileName,
        const QList<QString> &seqNames);

    static void openMultiSequenceFileAsSequences(HI::GUITestOpStatus &os, const QString &path, const QString &fileName);
    static void openMultiSequenceFileAsSequences(HI::GUITestOpStatus &os, const QString &filePath);
    static void openMultiSequenceFileAsMergedSequence(HI::GUITestOpStatus &os, const QString &filePath);
    static void openMultiSequenceFileAsMalignment(HI::GUITestOpStatus &os, const QString &dirPath, const QString &fileName);
    static void openMultiSequenceFileAsMalignment(HI::GUITestOpStatus &os, const QString &filePath);

    static void saveProjectAs(HI::GUITestOpStatus &os, const QString &path);
    static void closeProject(HI::GUITestOpStatus &os);

protected:
    static void openFilesDrop(HI::GUITestOpStatus &os, const QList<QUrl>& urls);
    static void openFilesWithDialog(HI::GUITestOpStatus &os, const QList<QUrl> &filePaths);
};

} // U2

#endif
