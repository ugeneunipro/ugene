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

#include <QUrl>
#include <QMutex>
#include <QWaitCondition>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <U2Core/AppContext.h>
#include <U2Core/AppSettings.h>
#include <U2Core/Log.h>
#include <U2Core/NetworkConfiguration.h>
#include <U2Core/U2SafePoints.h>

#include "HttpFileAdapter.h"
#include "ZlibAdapter.h"

namespace U2 {

#define MAX_GET_LENGTH 3000

HttpFileAdapterFactory::HttpFileAdapterFactory(QObject* o) : IOAdapterFactory(o) {
    name = tr("HTTP file");
}

IOAdapter* HttpFileAdapterFactory::createIOAdapter() {
    return new HttpFileAdapter(this);
}

GzippedHttpFileAdapterFactory::GzippedHttpFileAdapterFactory( QObject * obj /* = 0 */ )  :
HttpFileAdapterFactory( obj ) {
    name = tr("HTTP GZIP adaptor");
}

IOAdapter * GzippedHttpFileAdapterFactory::createIOAdapter() {
    return new ZlibAdapter( new HttpFileAdapter(this) );
}

QNetworkProxy HttpFileAdapterFactory::getProxyByUrl( const QUrl & url ) const {
    NetworkConfiguration* nc = AppContext::getAppSettings()->getNetworkConfiguration();
    return nc->getProxyByUrl(url);
}

HttpFileAdapter::HttpFileAdapter(HttpFileAdapterFactory* factory, QObject* o)
: IOAdapter(factory, o), is_cached(false), begin_ptr(-1), end_ptr(0),
  reply(NULL), badstate(false), is_downloaded(false), downloaded(0), total(0)
{
    chunk_list.push_back( QByteArray(CHUNKSIZE, char(0)) );
    netManager=new QNetworkAccessManager(this);

}
HttpFileAdapter::~HttpFileAdapter(){
     if (isOpen()) close();
     delete netManager;
     netManager=NULL;
}
bool HttpFileAdapter::open(const GUrl& url_, IOAdapterMode m) {
    SAFE_POINT (m == IOAdapterMode_Read, QString("Illegal IO mode: %1").arg(m), false);

    QUrl url( url_.getURLString().trimmed() );
    if( !url.isValid() ) {
        return false;
    }
    bool https = url.scheme() == "https";
    if( !url.scheme().isEmpty() && !https && url.scheme() != "http" ) {
        return false;
    }
    gurl = url_;
    init();

    HttpFileAdapterFactory * f = qobject_cast<HttpFileAdapterFactory*> ( getFactory() );
    QNetworkProxy proxy = f->getProxyByUrl(url);
    return open( url, proxy );
}

bool HttpFileAdapter::open( const QUrl& url, const QNetworkProxy & p)
{
    SAFE_POINT(!isOpen(), "Adapter is already opened!", false);

    if( reply ) {
        close();
    }
    netManager->setProxy(p);
    connect(netManager, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), this, SLOT(onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)));
    if (url.toString().length() > MAX_GET_LENGTH) {
        QStringList splittedStrings = url.toString().split(RemoteRequestConfig::HTTP_BODY_SEPARATOR);
        if(splittedStrings.count() > 1) {
            SAFE_POINT(2 == splittedStrings.count(), tr("Incorrect url string has been passed to HttpFileAdapter::open()"), false);
            QString headerString = splittedStrings.at(0);
            postData = splittedStrings.at(1).toLatin1();
            QNetworkRequest netRequest(headerString);
            reply=netManager->post(netRequest, postData);
        }
        else {
            QNetworkRequest netRequest(url);
            reply = netManager->post(netRequest, "");
        }
    }
    else {
        QString urlString = url.toString();
        urlString = urlString.replace(RemoteRequestConfig::HTTP_BODY_SEPARATOR, "&");
        QNetworkRequest netRequest(urlString);
        reply = netManager->get(netRequest);
    }
    coreLog.details(tr("Downloading from %1").arg(reply->url().toString()));
    connect( reply, SIGNAL(readyRead()), this, SLOT(add_data()), Qt::DirectConnection );
    connect( reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(progress(qint64,qint64)), Qt::DirectConnection );//+
    connect( reply, SIGNAL(finished()), this, SLOT(done()), Qt::DirectConnection );
    return true;
}

void HttpFileAdapter::close() {
    SAFE_POINT(isOpen(), "Adapter is not opened!", );

    if (!isOpen()) {
        return;
    }
    assert( reply );
    reply->abort();
    delete reply;
    reply = NULL;
    gurl = GUrl();
    init();
}

qint64 HttpFileAdapter::readBlock(char* data, qint64 size)
{
    SAFE_POINT(isOpen(), "Adapter is not opened!", 0);
    if( badstate ) {
        return -1;
    }
    assert( data );
    assert( CHUNKSIZE != end_ptr );
    size = waitData( size );

    if( badstate ) {
        return -1;
    }

    rwmut.lock();
    qint64 write_offs = 0;
    while( write_offs < size ) {
        qint64 howmuch = qMin( size - write_offs, (qint64)firstChunkContains() );
        readFromChunk(data + write_offs, howmuch);
        if (formatMode == TextMode) {
            cutByteOrderMarks(data, errorMessage, howmuch);
            CHECK(errorMessage.isEmpty(), -1);
        }
        write_offs += howmuch;
    }

    rwmut.unlock();
    return size;
}

qint64 HttpFileAdapter::writeBlock(const char* , qint64) {
    SAFE_POINT(0, "Operation is not supported!",0);
    return 0;
}

bool HttpFileAdapter::skip(qint64 nBytes) {
    SAFE_POINT(isOpen(), "Adapter is not opened!", false);

    nBytes = waitData( nBytes );
    rwmut.lock();

    if( nBytes < 0 ) {
        assert( -nBytes <= CHUNKSIZE );
        if( begin_ptr >= -nBytes ) {
            begin_ptr += nBytes;
        } else {
            if( !is_cached ) {
                rwmut.unlock();
                return false;
            }
            chunk_list.prepend( cache );
            begin_ptr = CHUNKSIZE - (-nBytes - begin_ptr);
            is_cached = false;
        }
    } else {
        rwmut.unlock();
        return nBytes == skipAhead( nBytes );
    }
    rwmut.unlock();
    return true;
}

qint64 HttpFileAdapter::left() const {
    return (is_downloaded ? stored() : -1);
}

void HttpFileAdapter::init() {
    reply=NULL;
    badstate = false;
    is_downloaded = false;
    is_cached= false;
    end_ptr = 0;
    chunk_list.clear();
    chunk_list.append( QByteArray(CHUNKSIZE, 0) );
    loop.exit();
}

void HttpFileAdapter::add_data()
{
    rwmut.lock();
    if( isEmpty() ) {
        begin_ptr = 0;
    }
    int howmuch = CHUNKSIZE - end_ptr;
    int got = reply->read( chunk_list.last().data() + end_ptr, howmuch );

    if( got < 0 ) {
        badstate = true;
    }
    else if( got < howmuch ) {
        end_ptr += got;
    }
    else {
        do {
            QByteArray newchunck( CHUNKSIZE, (char)0 );
            got = reply->read( newchunck.data(), CHUNKSIZE );

            if( got < 0  ) {
                badstate = true;
                break;
            }
            chunk_list.append( newchunck );
        } while( got == CHUNKSIZE );
        end_ptr = got;
    }
//    cond.wakeOne();
    loop.exit();
    rwmut.unlock();
}

qint64 HttpFileAdapter::stored() const
{
    return (singleChunk() ? firstChunkContains() :
           firstChunkContains() + end_ptr + (chunk_list.size()-2) * CHUNKSIZE );
}

void HttpFileAdapter::readFromChunk( char * data, int size )
{
    assert( size <= firstChunkContains() );
    assert( !isEmpty() );

    memcpy( data, chunk_list.first().data() + begin_ptr, size );
    if( begin_ptr + size == CHUNKSIZE) {
        popFront();
    } else {
        begin_ptr += size;
    }
}

void HttpFileAdapter::skipFromChunk( qint64 nbytes )
{
    assert( nbytes <= firstChunkContains() );
    assert( !isEmpty() );

    if( begin_ptr + nbytes == CHUNKSIZE) {
        popFront();
    } else {
        begin_ptr += nbytes;
    }
}

void HttpFileAdapter::popFront()
{
    if( singleChunk() ) {
        cache = chunk_list.first();
        Empty();
    } else {
        begin_ptr = 0;
        cache = chunk_list.takeFirst();
        is_cached = true;
    }
}

qint64 HttpFileAdapter::skipAhead( qint64 nBytes )
{
    assert( nBytes >= 0 );
    nBytes = qMin( nBytes, stored() );
    qint64 skip_offs = 0;
    while( skip_offs < nBytes ) {
        int howmuch = qMin( nBytes - skip_offs, (qint64)firstChunkContains() );
        skipFromChunk( howmuch );
        skip_offs += howmuch;
    }
    return nBytes;
}

qint64 HttpFileAdapter::waitData( qint64 until )
{
    while (!is_downloaded && (until > stored()) ) {
        loop.exec();
    }
    return qMin( until, stored() );
}

void HttpFileAdapter::done()
{
    QString locationHeaderValue = reply->header(QNetworkRequest::LocationHeader).toString();
    if (!locationHeaderValue.isEmpty()) {
        QUrl redirectUrl(locationHeaderValue);
        chunk_list.clear();
        close();
        coreLog.details(tr("Redirecting to %1").arg(locationHeaderValue));
        QString fullRedirectUrl(redirectUrl.toString());
        if (!postData.isEmpty()) {
            fullRedirectUrl = redirectUrl.toString() + RemoteRequestConfig::HTTP_BODY_SEPARATOR + postData;
        }
        open(fullRedirectUrl, netManager->proxy());
        return;
    }
    is_downloaded = true;
    badstate = (reply->error() != QNetworkReply::NoError);

    loop.exit();
}

int HttpFileAdapter::getProgress() const
{
    return (total ? (int)( 100 * (float)downloaded / total ) : -1);
}

void HttpFileAdapter::progress( qint64 done_, qint64 total_ )
{
    downloaded = done_;
    total = total_;
}

QString HttpFileAdapter::errorString() const{
    QString result;
    if (reply){
        result = reply->errorString();
    } else {
        result = errorMessage;
    }
    return result;
}

void HttpFileAdapter::onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth){
    auth->setUser(proxy.user());
    auth->setPassword(proxy.password());
    disconnect(this, SLOT(onProxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)));
}

}//namespace
