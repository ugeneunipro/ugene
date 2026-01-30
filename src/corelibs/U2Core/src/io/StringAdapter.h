/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <U2Core/IOAdapter.h>

namespace U2 {

class U2CORE_EXPORT StringAdapterFactory : public IOAdapterFactory {
    Q_OBJECT
public:
    StringAdapterFactory(QObject* p = nullptr);

    IOAdapter* createIOAdapter() override;

    IOAdapterId getAdapterId() const override {
        return BaseIOAdapters::STRING;
    }

    const QString& getAdapterName() const override {
        return name;
    }

    bool isIOModeSupported(IOAdapterMode m) const override {
        Q_UNUSED(m);
        return true;
    }  // data can be read and be written

    TriState isResourceAvailable(const GUrl& url) const override {
        Q_UNUSED(url);
        return TriState_Yes;
    }

protected:
    QString name;
};

class U2CORE_EXPORT StringAdapterFactoryWithStringData : public StringAdapterFactory {
    Q_OBJECT
public:
    StringAdapterFactoryWithStringData(const QString& data, QObject* parent = nullptr);

    IOAdapter* createIOAdapter() override;

private:
    const QString data;
};

class U2CORE_EXPORT StringAdapter : public IOAdapter {
    Q_OBJECT
public:
    StringAdapter(StringAdapterFactory* f, QObject* o = nullptr);
    ~StringAdapter() {
        if (isOpen())
            close();
    }

    /** Creates opened adapter */
    StringAdapter(const QByteArray& data, StringAdapterFactory* f);

    bool open(const GUrl& url, IOAdapterMode m) override;

    bool isOpen() const override {
        return opened;
    }

    void close() override;

    qint64 readBlock(char* data, qint64 maxSize) override;

    qint64 writeBlock(const char* data, qint64 size) override;

    bool skip(qint64 nBytes) override;

    qint64 left() const override;

    int getProgress() const override;

    qint64 bytesRead() const override;

    GUrl getURL() const override;

    const QByteArray& getBuffer() {
        return buffer;
    }

    QString errorString() const override;

private:
    bool opened;
    QByteArray buffer;
    int pos;
    GUrl url;
};

}  // namespace U2
