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

#ifndef _U2_HTTP_FILE_ADAPTER_H_
#define _U2_HTTP_FILE_ADAPTER_H_

#include <QAuthenticator>
#include <QEventLoop>
#include <QLinkedList>
#include <QMutex>
#include <QNetworkProxy>

#include <U2Core/IOAdapter.h>

class QNetworkAccessManager;
class QNetworkReply;
class QMutex;

namespace U2 {

class U2CORE_EXPORT HttpFileAdapterFactory: public IOAdapterFactory {
    Q_OBJECT
public:
    HttpFileAdapterFactory(QObject* p = NULL);

    virtual IOAdapter* createIOAdapter();

    virtual IOAdapterId getAdapterId() const {return BaseIOAdapters::HTTP_FILE;}

    virtual const QString& getAdapterName() const {return name;}

    virtual bool isIOModeSupported(IOAdapterMode m)  const {return m == IOAdapterMode_Read;} //read-only

    QNetworkProxy getProxyByUrl( const QUrl & url ) const;

    virtual TriState isResourceAvailable(const GUrl& ) const  {return TriState_Unknown;}

protected:
    QString name;
};

class U2CORE_EXPORT GzippedHttpFileAdapterFactory : public HttpFileAdapterFactory {
    Q_OBJECT
public:
    GzippedHttpFileAdapterFactory( QObject * obj = 0 );
    virtual IOAdapter * createIOAdapter();
    virtual IOAdapterId getAdapterId() const { return BaseIOAdapters::GZIPPED_HTTP_FILE; }
};

class U2CORE_EXPORT HttpFileAdapter : public IOAdapter {
    Q_OBJECT
public:
    HttpFileAdapter(HttpFileAdapterFactory* f, QObject* o = NULL);
    ~HttpFileAdapter();

    virtual bool open(const GUrl& url, IOAdapterMode m);

    bool open(const QUrl& url, const QNetworkProxy & p);

    virtual bool isOpen() const {return (bool)reply;}

    virtual void close();

    virtual qint64 readBlock(char* data, qint64 maxSize);

    virtual qint64 writeBlock(const char* data, qint64 size);

    virtual bool skip(qint64 nBytes);

    virtual qint64 left() const;

    virtual int getProgress() const;

    virtual GUrl getURL() const {return gurl;}

    virtual QString errorString() const;

private:
    void        init();
    qint64      stored() const;
    inline bool singleChunk() const { return chunk_list.size() == 1; }
    inline int  firstChunkContains() const {return (singleChunk() ? (isEmpty() ? 0 : end_ptr - begin_ptr) :
                                           CHUNKSIZE - begin_ptr);}
    inline void Empty() { assert( singleChunk() ); begin_ptr = -1; end_ptr = 0; }
    inline bool isEmpty() const { return -1 == begin_ptr && 0 == end_ptr; }
    void popFront();
    void readFromChunk( char * data, int size );
    void skipFromChunk( qint64 size );
    qint64 skipAhead( qint64 nBytes );
    qint64 waitData( qint64 until );

    static const int CHUNKSIZE = 32 * 1024;
    QLinkedList<QByteArray> chunk_list;
    QByteArray cache;
    bool is_cached;
    int begin_ptr; //pointer to the first byte of data in first chunk
    int end_ptr; //pointer to the first free byte in last chunk

    QNetworkAccessManager * netManager;
    QNetworkReply * reply;
    bool badstate;
    bool is_downloaded;
    int downloaded;
    int total;

    QMutex rwmut;
    QEventLoop loop;
    GUrl gurl;
    QByteArray postData;
private slots:
    void add_data();
    void done();
    void progress( qint64 done, qint64 total );
    void onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*);

};


}//namespace

#endif
