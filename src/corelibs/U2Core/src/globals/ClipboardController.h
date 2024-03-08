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

#include <QClipboard>

#include <U2Core/DocumentUtils.h>
#include <U2Core/Task.h>

namespace U2 {

/** Clipboard related helper methods and constants used by UGENE. */
class U2CORE_EXPORT U2Clipboard {
public:
    /**
     * MIME type used internally by UGENE. Marks the plain text data of a known document format
     * that can be auto-detected by UGENE. Example: FASTA, CLUSTALW, Genbank...
     */
    static const QString UGENE_MIME_TYPE;

    /** Maximum text data size UGENE can put into clipboard safely: 100mb. */
    static constexpr qint64 MAX_SAFE_COPY_TO_CLIPBOARD_SIZE = 100 * 1000 * 1000;

    /*
     * Maximum text data size UGENE can put into clipboard safely for GUI test mode.
     * Using lower value then MAX_SAFE_COPY_TO_CLIPBOARD_SIZE helps to speed up GUI tests.
    **/
    static constexpr qint64 UGENE_GUI_TEST_MAX_SAFE_COPY_TO_CLIPBOARD_SIZE = MAX_SAFE_COPY_TO_CLIPBOARD_SIZE / 100;

    /*
     * Check if text of .@clipboardSize could be copied to clipboard or not.
     * Set error to @os if not.
     **/
    static void checkCopyToClipboardSize(qint64 clipboardSize, U2OpStatus& os) {
        bool isGuiMode = qgetenv("UGENE_GUI_TEST") == "1";
        if (clipboardSize > (isGuiMode ? U2Clipboard::UGENE_GUI_TEST_MAX_SAFE_COPY_TO_CLIPBOARD_SIZE
                                      : U2Clipboard::MAX_SAFE_COPY_TO_CLIPBOARD_SIZE)) {
            os.setError(QObject::tr("Block size is too big and can't be copied into the clipboard"));
        }
    }
};

class U2CORE_EXPORT PasteTask : public Task {
    Q_OBJECT
public:
    PasteTask(TaskFlags flags = TaskFlag_None);

    virtual QList<GUrl> getUrls() const = 0;
    virtual QList<Document*> getDocuments() const = 0;

protected:
    virtual void processDocument(Document* doc);
};

class U2CORE_EXPORT PasteFactory : public QObject {
    Q_OBJECT
public:
    PasteFactory(QObject* parent = 0);

    /** Creates task using current clipboard state. May return NULL if clipboard state is invalid/unsupported. */
    virtual PasteTask* createPasteTask(bool isAddToProject) = 0;
};

class U2CORE_EXPORT PasteUtils : public QObject {
    Q_OBJECT
public:
    static QList<DNASequence> getSequences(const QList<Document*>& docs, U2OpStatus& os);
};

}  // namespace U2
