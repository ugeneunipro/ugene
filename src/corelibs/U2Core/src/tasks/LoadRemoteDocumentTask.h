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

#pragma once

#include <QAuthenticator>
#include <QNetworkReply>
#include <QXmlReader>
#include <QXmlStreamReader>

#include <U2Core/DocumentProviderTask.h>
#include <U2Core/GUrl.h>
#include <U2Core/NetworkConfiguration.h>

class QEventLoop;
class QTimer;

namespace U2 {

// Entrez tools variables
#define ENSEBL_ID "ensembl"
#define GENBANK_NUCLEOTIDE_ID "nucleotide"
#define GENBANK_PROTEIN_ID "protein"
#define GENBANK_FORMAT "gb"
#define FASTA_FORMAT "fasta"
#define GENBANK_WITH_PARTS "gbwithparts"
#define FORCE_DOWNLOAD_SEQUENCE_HINT "gbwithparts"

class Document;
class CopyDataTask;
class LoadDocumentTask;
class LoadDataFromEntrezTask;

class U2CORE_EXPORT RemoteDBRegistry {
    RemoteDBRegistry();
    QMap<QString, QString> queryDBs;
    QMap<QString, QString> httpDBs;
    QMap<QString, QString> hints;
    QMap<QString, QString> aliases;

public:
    // Links to the database (to open in browser) by the database name.
    static const QMap<QString, QString> EXTERNAL_LINKS;
    // Links to the database (to open in browser) to open a sequence page by database name.
    static const QMap<QString, QString> PAGE_LINKS;

    QString getURL(const QString& accId, const QString& dbName) const;
    QString getDbEntrezName(const QString& dbName) const;
    void convertAlias(QString& dbName) const;
    QList<QString> getDBs() const;
    bool hasDbId(const QString& dbId) const;
    QString getHint(const QString& dbName) const;
    // TODO: move this to AppContext
    static RemoteDBRegistry& getRemoteDBRegistry();

public:
    static const QString ENSEMBL;
    static const QString GENBANK_DNA;
    static const QString GENBANK_PROTEIN;
    static const QString PDB;
    static const QString SWISS_PROT;
    static const QString UNIPROTKB_SWISS_PROT;
    static const QString UNIPROTKB_TREMBL;
};

class U2CORE_EXPORT RecentlyDownloadedCache : public QObject {
    Q_OBJECT
    QMap<QString, QString> urlMap;

public:
    RecentlyDownloadedCache();
    ~RecentlyDownloadedCache() override;

    bool contains(const QString& fileName) const;

    void append(const QString& fullPath);

    void remove(const QString& fullPath);

    QString getFullPath(const QString& fileName) const;

    void clear() {
        urlMap.clear();
    }
};

class U2CORE_EXPORT EntrezUtils {
public:
    static const QString NCBI_ESEARCH_URL;
    static const QString NCBI_ESUMMARY_URL;
    static const QString NCBI_EFETCH_URL;
    static const QString NCBI_DB_NUCLEOTIDE;
    static const QString NCBI_DB_PROTEIN;
};

// Base class for loading documents
class U2CORE_EXPORT BaseLoadRemoteDocumentTask : public DocumentProviderTask {
    Q_OBJECT
public:
    BaseLoadRemoteDocumentTask(const QString& downloadPath = QString(), const QVariantMap& hints = QVariantMap(), TaskFlags flags = TaskFlags(TaskFlags_NR_FOSCOE | TaskFlag_MinimizeSubtaskErrorText));
    void prepare() override;
    ReportResult report() override;

    QString getLocalUrl() const;
    static bool prepareDownloadDirectory(const QString& path);
    static QString getDefaultDownloadDirectory();

protected:
    virtual QString getFileFormat(const QString& dbid) = 0;
    virtual GUrl getSourceUrl() = 0;
    virtual QString getFileName() = 0;

    bool isCached();
    bool initLoadDocumentTask();
    void createLoadedDocument();

    GUrl sourceUrl;
    QString fileName;
    QString fullPath;
    QString downloadPath;
    QString format;
    QVariantMap hints;

    DocumentFormatId formatId;
    CopyDataTask* copyDataTask = nullptr;
    LoadDocumentTask* loadDocumentTask = nullptr;
};

class U2CORE_EXPORT LoadRemoteDocumentTask : public BaseLoadRemoteDocumentTask {
    Q_OBJECT
public:
    LoadRemoteDocumentTask(const GUrl& url);
    LoadRemoteDocumentTask(const QString& accId, const QString& dbName, const QString& fullPathDir = "", const QString& fileFormat = "", const QVariantMap& hints = QVariantMap());
    void prepare() override;
    QString generateReport() const override;

    QString getAccNumber() const;
    QString getDBName() const;

protected:
    QString getFileFormat(const QString& dbid) override;
    GUrl getSourceUrl() override;
    QString getFileName() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    QString getRetType() const;

    GUrl fileUrl;
    LoadDataFromEntrezTask* loadDataFromEntrezTask = nullptr;
    QString accNumber;
    QString dbName;
};

class U2CORE_EXPORT BaseEntrezRequestTask : public Task {
    Q_OBJECT
public:
    BaseEntrezRequestTask(const QString& taskName);
    ~BaseEntrezRequestTask() override;

protected slots:
    virtual void sl_replyFinished(QNetworkReply* reply) = 0;
    void sl_onError();
    void sl_uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    virtual void onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*);

protected:
    // method should be called from the thread where @networkManager is actually used
    void createLoopAndNetworkManager(const QString& queryString);

    QEventLoop* loop;
    QNetworkAccessManager* networkManager;
};

// This task makes queries to NCBI Entrez search engine, using eTools
// First step: query eSearch to get global Entrez index
// Second step: query eFetch to download file by index
// About eTools: http://www.ncbi.nlm.nih.gov/bookshelf/br.fcgi?book=coursework&part=eutils
class U2CORE_EXPORT LoadDataFromEntrezTask : public BaseEntrezRequestTask {
    Q_OBJECT
public:
    LoadDataFromEntrezTask(const QString& dbId,
                           const QString& accNumber,
                           const QString& retType,
                           const QString& fullPath);

    void run() override;

private slots:
    void sl_replyFinished(QNetworkReply* reply) override;
    void sl_cancelCheck() const;

private:
    void runRequest(const QUrl& requestUrl);

    QNetworkReply* downloadReply = nullptr;
    QXmlStreamReader xmlReader;
    QString db;
    QString accNumber;
    QString resultIndex;
    QString fullPath;
    QString format;
};

class U2CORE_EXPORT XmlStreamReaderHandler {
public:
    virtual ~XmlStreamReaderHandler() = default;
    virtual QString startElement(const QString& qName, const QXmlStreamAttributes& attributes) = 0;
    virtual QString endElement(const QString& qName) = 0;
    virtual QString characters(const QString& str) = 0;
};

class U2CORE_EXPORT EntrezQueryTask : public BaseEntrezRequestTask {
    Q_OBJECT
public:
    EntrezQueryTask(XmlStreamReaderHandler* resultHandler, const QString& query);

    void run() override;
    const XmlStreamReaderHandler* getResultHandler() const;

private slots:
    void sl_replyFinished(QNetworkReply* reply) override;

private:
    void runRequest(const QUrl& requestUrl);

    QNetworkReply* queryReply = nullptr;
    XmlStreamReaderHandler* resultHandler;
    QXmlStreamReader xmlReader;
    QString query;
};

// Helper class to parse NCBI Entrez eSearch results
class U2CORE_EXPORT ESearchResultHandler : public XmlStreamReaderHandler {
public:
    ESearchResultHandler();
    QString startElement(const QString& qName, const QXmlStreamAttributes& attributes) override;
    QString endElement(const QString& qName) override;
    QString characters(const QString& str) override;

    const QList<QString>& getIdList() const {
        return idList;
    }

private:
    bool metESearchResult;
    QString curText;
    QList<QString> idList;
};

struct EntrezSummary {
    QString id;
    QString name;
    QString title;
    int size = 0;
};

// Helper class to parse NCBI Entre ESummary results.
class U2CORE_EXPORT ESummaryResultHandler : public XmlStreamReaderHandler {
public:
    ESummaryResultHandler();
    QString startElement(const QString& qName, const QXmlStreamAttributes& attributes) override;
    QString endElement(const QString& qName) override;
    QString characters(const QString& str) override;
    const QList<EntrezSummary>& getResults() const;

private:
    bool metESummaryResult;
    QString curText;
    EntrezSummary currentSummary;
    QXmlStreamAttributes curAttributes;
    QList<EntrezSummary> results;
};

}  // namespace U2
