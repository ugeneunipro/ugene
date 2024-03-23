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

#include <QPointer>
#include <QVariantMap>

#include <U2Core/DocumentProviderTask.h>
#include <U2Core/GObjectReference.h>
#include <U2Core/GUrl.h>

namespace U2 {

class Document;
class DocumentFormat;
class IOAdapterFactory;
class StateLock;
class LoadDocumentTask;

// creates object using name and type info from ref
// NOTE: the default impl can create only limited set of objects
class U2CORE_EXPORT LDTObjectFactory : public QObject {
public:
    LDTObjectFactory(QObject* p)
        : QObject(p) {
    }

    virtual GObject* create(const GObjectReference& ref);
};

class U2CORE_EXPORT LoadDocumentTaskConfig {
public:
    LoadDocumentTaskConfig(bool _createDoc = false, const GObjectReference& _ref = GObjectReference(), LDTObjectFactory* _f = nullptr)
        : createDoc(_createDoc), checkObjRef(_ref), objFactory(_f) {
    }

    bool createDoc;  // if document is failed to load and 'createDoc' is true -> it will be created
    GObjectReference checkObjRef;  // if is valid task checks that ref is found in doc, fails if not
    LDTObjectFactory* objFactory;  // if not NULL and 'checkObjRef' is not found -> 'objFactory' is used to create obj
};

class U2CORE_EXPORT LoadUnloadedDocumentTask : public DocumentProviderTask {
    Q_OBJECT
public:
    LoadUnloadedDocumentTask(Document* d, const LoadDocumentTaskConfig& config = LoadDocumentTaskConfig());

    void prepare() override;
    ReportResult report() override;
    Document* getDocument(bool mainThread = true) override;

    static QString getResourceName(Document* d);

    const LoadDocumentTaskConfig& getConfig() const {
        return config;
    }

    static LoadUnloadedDocumentTask* findActiveLoadingTask(Document* d);
    static bool addLoadingSubtask(Task* t, const LoadDocumentTaskConfig& config);

private:
    void clearResourceUse();

    LoadDocumentTask* loadTask;
    QPointer<Document> unloadedDoc;
    QString resName;
    LoadDocumentTaskConfig config;
};

class U2CORE_EXPORT LoadDocumentTask : public DocumentProviderTask {
    Q_OBJECT
public:
    static LoadDocumentTask* getDefaultLoadDocTask(const GUrl& url, const QVariantMap& hints = QVariantMap());
    static LoadDocumentTask* getDefaultLoadDocTask(U2OpStatus& os, const GUrl& url, const QVariantMap& hints = QVariantMap());
    static DocumentProviderTask* getCommonLoadDocTask(const GUrl& url);

public:
    LoadDocumentTask(DocumentFormatId format, const GUrl& url, IOAdapterFactory* iof, const QVariantMap& hints = QVariantMap(), const LoadDocumentTaskConfig& config = LoadDocumentTaskConfig());

    LoadDocumentTask(DocumentFormat* format, const GUrl& url, IOAdapterFactory* iof, const QVariantMap& hints = QVariantMap(), const LoadDocumentTaskConfig& config = LoadDocumentTaskConfig());

    void run() override;
    void prepare() override;
    ReportResult report() override;

    const GUrl& getURL() const;

    QString getURLString() const;

    /** When set to 'true' the result document will be moved to the main thread right after loading (run). */
    bool moveDocumentToMainThread = false;

private:
    void loadDocument();

    void init();

    /**
     * Processes config.checkObjRef and config.checkObjRef.objFactory related settings.
     * See 'LoadDocumentTaskConfig' for details.
     */
    void processObjRef(Document* loadedDocument);

    int calculateMemory() const;

    static void renameObjects(Document* doc, const QStringList& names);

    DocumentFormat* format;
    const GUrl url;
    IOAdapterFactory* iof;
    QVariantMap hints;
    LoadDocumentTaskConfig config;
};

}  // namespace U2
