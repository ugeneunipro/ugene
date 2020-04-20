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

#ifndef _U2_BASEDOC_WRITER_H_
#define _U2_BASEDOC_WRITER_H_

#include <U2Lang/LocalDomain.h>

#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

class Document;
class DocumentFormat;
class IOAdapter;

namespace LocalWorkflow {

class BaseDocWriter : public BaseWorker {
    Q_OBJECT
public:
    enum DataStorage {
        LocalFs,
        SharedDb
    };

    BaseDocWriter(Actor* a, const DocumentFormatId& fid);
    BaseDocWriter( Actor * a );
    virtual ~BaseDocWriter(){}
    virtual void init() ;
    virtual Task* tick() ;
    virtual void cleanup() ;

    static QString getUniqueObjectName(const Document *doc, const QString &name);
    static QString generateUrl(const MessageMetadata &metadata, bool groupByDatasets, const QString &suffix, const QString &ext, const QString &defaultName);

protected:
    virtual void data2doc(Document*, const QVariantMap&) = 0;
    virtual bool hasDataToWrite(const QVariantMap &data) const = 0;
    virtual QSet<GObject *> getObjectsToWrite(const QVariantMap &data) const;
    virtual bool isStreamingSupport() const;
    virtual bool isSupportedSeveralMessages() const;
    virtual void storeEntry(IOAdapter *, const QVariantMap &, int) {}
    virtual Task * getWriteDocTask(Document *doc, const SaveDocFlags &flags);
    virtual void takeParameters(U2OpStatus &os);
    virtual QStringList takeUrlList(const QVariantMap &data, int metadataId, U2OpStatus &os);

protected:
    DocumentFormat *format;

    U2DbiRef dstDbiRef;

private:
    DataStorage dataStorage;

    CommunicationChannel *ch;
    bool append;
    uint fileMode;
    QSet<QString> usedUrls;
    QMap<QString, int> counters; // url <-> count suffix
    QMap<QString, IOAdapter*> adapters;
    QMap<IOAdapter*, Document*> docs;

    QString dstPathInDb;
    bool objectsReceived;

private slots:
    void sl_objectImported(Task *importTask);

private:
    bool ifCreateAdapter(const QString &url) const;
    /**
     * Creates an adapter for @url or returns existing one.
     * The url of the adapter could be not equal to @url.
     */
    IOAdapter * getAdapter(const QString &url, U2OpStatus &os);
    void openAdapter(IOAdapter *io, const QString &url, const SaveDocFlags &flags, U2OpStatus &os);
    /** Creates a document for @io or returns existing one. */
    Document * getDocument(IOAdapter *io, U2OpStatus &os);
    Task * processDocs();
    SaveDocFlags getDocFlags() const;
    void storeData(const QStringList &urls, const QVariantMap &data, U2OpStatus &os);
    Task * createWriteToSharedDbTask(const QVariantMap &data);
    void reportNoDataReceivedWarning();

    QString getDefaultFileName() const;
    bool ifGroupByDatasets() const;
    QString getSuffix() const;
    QString getExtension() const;
    QString generateUrl(int metadataId) const;
    static QString getBaseName(const MessageMetadata &metadata, bool groupByDatasets, const QString &defaultName);
};

}// Workflow namespace
}// U2 namespace

#endif // _U2_BASEDOC_WRITER_H_
