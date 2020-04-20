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

#ifndef _U2_DATABASE_CONNECTION_ADAPTER_H_
#define _U2_DATABASE_CONNECTION_ADAPTER_H_

#include <U2Core/IOAdapter.h>
#include <U2Core/U2DbiUtils.h>

namespace U2 {

class U2CORE_EXPORT DatabaseConnectionAdapterFactory : public IOAdapterFactory {
    Q_OBJECT
public:
    explicit DatabaseConnectionAdapterFactory(QObject *parent = NULL);

    IOAdapter* createIOAdapter();

    IOAdapterId getAdapterId() const;

    const QString& getAdapterName() const;

    bool isIOModeSupported(IOAdapterMode m)  const;

    TriState isResourceAvailable(const GUrl& ) const;

protected:
    QString name;
};

class U2CORE_EXPORT DatabaseConnectionAdapter : public IOAdapter {
    Q_OBJECT
public:
    DatabaseConnectionAdapter(DatabaseConnectionAdapterFactory* factory, QObject* parent = NULL);

    bool open(const GUrl& url, IOAdapterMode m);
    bool isOpen() const;
    void close();

    /**
      * Unsupported methods
      * You can't read anything with adapter
      * Get objects via U2Dbi interface
      */
    qint64 readUntil(char*, qint64, const QBitArray&, TerminatorHandling, bool* = 0);
    qint64 readBlock(char*, qint64);
    qint64 writeBlock(const char* buff, qint64 size);
    bool skip(qint64);
    qint64 left() const;

    /**
     * Utility methods
     */
    DbiConnection getConnection() const;
    int getProgress() const;
    bool isEof();
    GUrl getURL() const;
    QString errorString() const;

private:
    DbiConnection connection;
};

}   // namespace U2

#endif // _U2_DATABASE_CONNECTION_ADAPTER_H_
