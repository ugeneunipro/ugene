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

#ifndef _U2_SAVE_DOCUMENT_TASK_H_
#define _U2_SAVE_DOCUMENT_TASK_H_

#include <QPointer>

#include <U2Core/GUrl.h>
#include <U2Core/Task.h>
#include <U2Core/UnloadedObject.h>

namespace U2 {

class Document;
class StateLock;
class IOAdapterFactory;
class DocumentFormat;

enum SaveDocFlag {
    SaveDoc_Overwrite = 1 << 0,  // Deprecated. Not used by the 'SaveDocumentTask', but still is used by a non-related code.
    SaveDoc_Append = 1 << 1,
    SaveDoc_Roll = 1 << 2,
    SaveDoc_DestroyAfter = 1 << 3,
    SaveDoc_DestroyButDontUnload = 1 << 4,
    SaveDoc_OpenAfter = 1 << 5,
    SaveDoc_UnloadAfter = 1 << 6,
    SaveDoc_ReduceLoggingLevel = 1 << 7
};
Q_DECLARE_FLAGS(SaveDocFlags, SaveDocFlag)

class U2CORE_EXPORT SaveDocumentTask : public Task {
    Q_OBJECT
public:
    SaveDocumentTask(Document* doc, IOAdapterFactory* iof = nullptr, const GUrl& url = GUrl(), SaveDocFlags flags = 0);
    SaveDocumentTask(Document* doc, SaveDocFlags flags, const QSet<QString>& excludeFileNames = QSet<QString>());

    void prepare() override;

    void run() override;

    ReportResult report() override;

    const GUrl& getURL() const {
        return url;
    }

    IOAdapterFactory* getIOAdapterFactory() const {
        return iof;
    }

    const QPointer<Document>& getDocument() const {
        return doc;
    }

    // used in file-name rolling mode
    void setExcludeFileNames(const QSet<QString>& _excludeFileNames) {
        excludeFileNames = _excludeFileNames;
    }

    void addFlag(SaveDocFlag f);

    /** Returns current set of 'openDocumentWithProjectHints'. See 'openDocumentWithProjectHints' for details. */
    QVariantMap getOpenDocumentWithProjectHints() const;

    /** Sets new 'openDocumentWithProjectHints'. See 'openDocumentWithProjectHints' for details. */
    void setOpenDocumentWithProjectHints(const QVariantMap& hints);

private:
    StateLock* lock;
    QPointer<Document> doc;
    IOAdapterFactory* iof;
    GUrl url;
    SaveDocFlags flags;
    QSet<QString> excludeFileNames;

    /** Set of hints passed to openWithProjectTask when SaveDoc_OpenAfter is present.*/
    QVariantMap openDocumentWithProjectHints;
};

enum SavedNewDocFlag {
    SavedNewDoc_Open = true,
    SavedNewDoc_DoNotOpen = false
};

class U2CORE_EXPORT SaveMultipleDocuments : public Task {
    Q_OBJECT
public:
    SaveMultipleDocuments(const QList<Document*>& docs, bool askBeforeSave, SavedNewDocFlag openFlag = SavedNewDoc_DoNotOpen);

    static QList<Document*> findModifiedDocuments(const QList<Document*>& docs);

private:
    GUrl chooseAnotherUrl(Document* doc);
};

class U2CORE_EXPORT SaveCopyAndAddToProjectTask : public Task {
    Q_OBJECT
public:
    SaveCopyAndAddToProjectTask(Document* doc, IOAdapterFactory* iof, const GUrl& url);
    ReportResult report();

private:
    SaveDocumentTask* saveTask;
    QList<UnloadedObjectInfo> info;
    GUrl url;
    GUrl origURL;
    DocumentFormat* df;
    QVariantMap hints;
};

class U2CORE_EXPORT RelocateDocumentTask : public Task {
    Q_OBJECT
public:
    RelocateDocumentTask(const GUrl& fromURL, const GUrl& toURL);
    ReportResult report();

public:
    GUrl fromURL;
    GUrl toURL;
};

}  // namespace U2

Q_DECLARE_OPERATORS_FOR_FLAGS(U2::SaveDocFlags)

#endif
