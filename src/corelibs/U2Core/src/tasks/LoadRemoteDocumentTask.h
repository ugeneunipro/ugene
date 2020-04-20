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

#ifndef _U2_LOAD_REMOTE_DOCUMENT_TASK_H_
#define _U2_LOAD_REMOTE_DOCUMENT_TASK_H_

#include <U2Core/DocumentProviderTask.h>
#include <U2Core/GUrl.h>
#include <U2Core/NetworkConfiguration.h>

#include <QNetworkReply>
#include <QAuthenticator>
#include <QXmlReader>

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
    QString getURL(const QString& accId, const QString& dbName);
    QString getDbEntrezName(const QString& dbName);
    void convertAlias(QString& dbName);
    QList<QString> getDBs();
    bool hasDbId(const QString& dbId);
    QString getHint(const QString& dbName);
    //TODO: move this to AppContext
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
    void loadCacheFromSettings();
    void saveCacheToSettings();
public:
    RecentlyDownloadedCache();
    ~RecentlyDownloadedCache();
    bool contains(const QString& fileName);
    void append(const QString& fullPath);
    void remove(const QString& fullPath);
    QString getFullPath(const QString& fileName);
    void clear() { urlMap.clear(); }

};

class U2CORE_EXPORT EntrezUtils {
public:
    static const QString NCBI_ESEARCH_URL;
    static const QString NCBI_ESUMMARY_URL;
    static const QString NCBI_EFETCH_URL;
    static const QString NCBI_DB_NUCLEOTIDE;
    static const QString NCBI_DB_PROTEIN;

};


//Base class for loading documents
class U2CORE_EXPORT BaseLoadRemoteDocumentTask : public DocumentProviderTask {
    Q_OBJECT
public:
    BaseLoadRemoteDocumentTask(const QString& downloadPath = QString(), const QVariantMap &hints = QVariantMap(), TaskFlags flags = TaskFlags(TaskFlags_NR_FOSCOE | TaskFlag_MinimizeSubtaskErrorText));
    virtual void prepare();
    QString getLocalUrl() { return fullPath; }

    virtual ReportResult report();

    static bool prepareDownloadDirectory(QString &path);
    static QString getDefaultDownloadDirectory();

protected:
    virtual QString getFileFormat(const QString & dbid) = 0;
    virtual GUrl getSourceUrl() = 0;
    virtual QString getFileName() = 0;


protected:
    bool isCached();
    bool initLoadDocumentTask();
    void createLoadedDocument();

protected:
    GUrl        sourceUrl;
    QString     fileName;
    QString     fullPath;
    QString     downloadPath;
    QString     format;
    QVariantMap hints;

    DocumentFormatId formatId;
    CopyDataTask* copyDataTask;
    LoadDocumentTask* loadDocumentTask;

};

class U2CORE_EXPORT LoadRemoteDocumentTask : public BaseLoadRemoteDocumentTask {
    Q_OBJECT
public:
    LoadRemoteDocumentTask(const GUrl& url);
    LoadRemoteDocumentTask(const QString & accId, const QString & dbName, const QString & fullPathDir = "", const QString& fileFormat = "", const QVariantMap &hints = QVariantMap());
    virtual void prepare();

    QString getAccNumber() const { return accNumber; }
    QString getDBName() const { return dbName; }

protected:
    virtual QString getFileFormat(const QString & dbid);
    virtual GUrl getSourceUrl();
    virtual QString getFileName();
    QList<Task*> onSubTaskFinished(Task* subTask);

private:
    QString getRetType() const;

    GUrl fileUrl;
    LoadDataFromEntrezTask* loadDataFromEntrezTask;
    QString accNumber;
    QString dbName;
};

class U2CORE_EXPORT BaseEntrezRequestTask : public Task {
    Q_OBJECT
public:
    BaseEntrezRequestTask(const QString &taskName);
    virtual ~BaseEntrezRequestTask();

    protected slots:
    virtual void sl_replyFinished(QNetworkReply *reply) = 0;
    void sl_onError(QNetworkReply::NetworkError error);
    void sl_uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    virtual void onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*);

protected:
    // method should be called from the thread where @networkManager is actually used
    void createLoopAndNetworkManager(const QString &queryString);

    QEventLoop *loop;
    QNetworkAccessManager *networkManager;
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

    void run();

    private slots:
    void sl_replyFinished(QNetworkReply* reply);
    void sl_cancelCheck();

private:
    void runRequest(const QUrl& requestUrl);

    QNetworkReply* searchReply;     // TODO: I think, it is unsed variable. Check if you can remove it.
    QNetworkReply* downloadReply;
    QXmlSimpleReader xmlReader;
    QString db;
    QString accNumber;
    QString resultIndex;
    QString fullPath;
    QString format;
};

class U2CORE_EXPORT EntrezQueryTask : public BaseEntrezRequestTask {
    Q_OBJECT
public:
    EntrezQueryTask(QXmlDefaultHandler* resultHandler, const QString& query);

    void run();
    const QXmlDefaultHandler* getResultHandler() const;

    private slots:
    void sl_replyFinished(QNetworkReply* reply);

private:
    void runRequest(const QUrl& requestUrl);

    QNetworkReply* queryReply;
    QXmlDefaultHandler* resultHandler;
    QXmlSimpleReader xmlReader;
    QString query;
};

// Helper class to parse NCBI Entrez eSearch results
class U2CORE_EXPORT ESearchResultHandler : public QXmlDefaultHandler {
    bool metESearchResult;
    QString errorStr;
    QString curText;
    QList<QString> idList;
public:
    ESearchResultHandler();
    bool startElement(const QString &namespaceURI, const QString &localName,
        const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
        const QString &qName);
    bool characters(const QString &str);
    bool fatalError(const QXmlParseException &exception);
    QString errorString() const { return errorStr; }
    const QList<QString>& getIdList() const { return idList; }
};


struct EntrezSummary {
    EntrezSummary() : size(0) {}

    QString id;
    QString name;
    QString title;
    int size;
};

// Helper class to parse NCBI Entrez ESummary results
class U2CORE_EXPORT ESummaryResultHandler : public QXmlDefaultHandler {
    bool metESummaryResult;
    QString errorStr;
    QString curText;
    EntrezSummary currentSummary;
    QXmlAttributes curAttributes;
    QList<EntrezSummary> results;
public:
    ESummaryResultHandler();
    bool startElement(const QString &namespaceURI, const QString &localName,
        const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
        const QString &qName);
    bool characters(const QString &str);
    bool fatalError(const QXmlParseException &exception);
    QString errorString() const { return errorStr; }
    const QList<EntrezSummary>& getResults() const { return results; }
};



} //namespace

#endif // _U2_LOAD_REMOTE_DOCUMENT_TASK_H_
