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

#ifndef _U2_IOADAPTER_H_
#define _U2_IOADAPTER_H_

#include <QObject>

#include <U2Core/GUrl.h>

namespace U2 {

enum IOAdapterMode {
    IOAdapterMode_Read,
    IOAdapterMode_Write,
    IOAdapterMode_Append
};

class IOAdapter;

class U2CORE_EXPORT IOAdapterFactory : public QObject {
public:
    IOAdapterFactory(QObject* p) : QObject(p) {}

    virtual IOAdapter* createIOAdapter() = 0;

    virtual IOAdapterId getAdapterId() const = 0;

    virtual const QString& getAdapterName() const = 0;

    virtual bool isIOModeSupported(IOAdapterMode m) const = 0;

    /** Returns YES if resource exists and available to read */
    virtual TriState isResourceAvailable(const GUrl& url) const = 0;

};

class U2CORE_EXPORT IOAdapter : public QObject {
    Q_OBJECT
public:
    IOAdapter(IOAdapterFactory* f, QObject* o = NULL) : QObject(o), formatMode(BinaryMode), factory(f) {}

    virtual ~IOAdapter() {}

    IOAdapterId getAdapterId() const { return factory->getAdapterId(); }

    virtual const QString& getAdapterName() const { return factory->getAdapterName(); }

    virtual bool isIOModeSupported(IOAdapterMode m) const { return factory->isIOModeSupported(m); }

    IOAdapterFactory* getFactory() const { return factory; }

    virtual bool open(const GUrl& url, IOAdapterMode m) = 0;

    virtual bool isOpen() const = 0;

    virtual void close() = 0;

    enum TerminatorHandling {
        Term_Exclude,   // stop before terminators
        Term_Include,   // include all terminators into result
        Term_Skip       // do not include terminators to the result, but skip to after last terminator
    };

    enum FormatMode {
        TextMode,    //Format is represented by text
        BinaryMode   //Format is represented by binary data
    };

    //if format is represented by text (not by binary data) you need to call this func
    virtual void setFormatMode(FormatMode mode) { formatMode = mode; }

    //return 0 if at the end of file, -1 if error
    virtual qint64 readUntil(char* buff, qint64 maxSize, const QBitArray& readTerminators,
        TerminatorHandling th, bool* terminatorFound = 0);

    virtual bool getChar(char* buff) { return 1 == readBlock(buff, 1); }

    //If an error occurs, this function returns -1
    virtual qint64 readBlock(char* buff, qint64 maxSize) = 0;

    //read a single line of text and skips one EOL, returns length of line w/o terminator or -1
    virtual qint64 readLine(char* buff, qint64 maxSize, bool* terminatorFound = 0);

    virtual qint64 writeBlock(const char* buff, qint64 size) = 0;

    qint64 writeBlock(const QByteArray& a) { return writeBlock(a.data(), a.size()); }

    /**
     * Both positive and negative values are accepted.
     * Implementations should support skipping backwards within 32K of total read data.
     */
    virtual bool skip(qint64 nBytes) = 0;

    //returns -1 if not supported
    virtual qint64 left() const = 0;

    /* Percent values in range 0..100, negative if unknown. */
    virtual int getProgress() const = 0;

    virtual bool isEof();

    virtual qint64 bytesRead() const { return -1; }

    virtual GUrl getURL() const = 0;

    virtual QString toString() const { return getURL().getURLString(); }

    /* Returns a human-readable description of the last device error that occurred */
    virtual QString errorString() const = 0;

    bool hasError() const { return !errorString().isEmpty(); }

protected:
    static void cutByteOrderMarks(char* data, QString& errorString, qint64& length);

    FormatMode formatMode;
    QString errorMessage;

private:
    IOAdapterFactory* factory;
};

class U2CORE_EXPORT IOAdapterRegistry : public QObject {
public:
    IOAdapterRegistry(QObject* p = NULL) : QObject(p) {}

    virtual bool registerIOAdapter(IOAdapterFactory* io) = 0;

    virtual bool unregisterIOAdapter(IOAdapterFactory* io) = 0;

    virtual const QList<IOAdapterFactory*>& getRegisteredIOAdapters() const = 0;

    virtual IOAdapterFactory* getIOAdapterFactoryById(IOAdapterId id) const = 0;
};

class U2CORE_EXPORT BaseIOAdapters {
public:
    static const IOAdapterId LOCAL_FILE;
    static const IOAdapterId GZIPPED_LOCAL_FILE;
    static const IOAdapterId HTTP_FILE;
    static const IOAdapterId GZIPPED_HTTP_FILE;
    static const IOAdapterId VFS_FILE;
    static const IOAdapterId STRING;
    static const IOAdapterId DATABASE_CONNECTION;
};


}//namespace

#endif

