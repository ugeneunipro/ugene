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

#ifndef _U2_NEXUS_PARSER_H_
#define _U2_NEXUS_PARSER_H_

#include <QTextStream>

#include <U2Core/IOAdapter.h>
#include <U2Core/Task.h>

#include <cassert>

namespace U2
{

class GObject;

class Tokenizer
// Breaks input stream on tokens (QString)
{
public:
    Tokenizer(IOAdapter *io_) :
        io(io_)
    {
        assert(io_ && "IO must exist");
    }

    QString look();
    QString get();
    void skip() { (void)get(); }

    QStringList getUntil(QString what, Qt::CaseSensitivity cs = Qt::CaseInsensitive);
    void skipUntil(QString what, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

    QString readUntil(QRegExp rwhat);

    bool isEof() { return io->isEof(); }
    IOAdapter* getIO() { return io; }

private:
    IOAdapter *io;
    QString next;

    static const int BUFF_SIZE;
    QString buff;
    QTextStream buffStream;
};

class NEXUSParser
// NEXUS File format parser
{
public:
    NEXUSParser (IOAdapter *io_, const U2DbiRef& dbiRef_, const QString& folder, U2OpStatus &ti_)
        : io(io_), dbiRef(dbiRef_), folder(folder), ti(ti_), tz(io)
    {
        assert(io_ && "IO must exist");
    }

    QList<GObject*> loadObjects(const U2DbiRef &dbiRef);

    bool hasError() { return !errors.isEmpty(); }
    bool hasWarnings() { return !warnings.isEmpty(); }

    const QStringList& getErrors() const { return errors; }
    const QStringList& getWarnings() const { return warnings; }

private:
    typedef QMap<QString, QString> Context;

    bool skipCommand();
    bool readSimpleCommand(Context &ctx);

    bool readBlock(Context &ctx, const U2DbiRef &dbiRef);
    bool skipBlockContents();
    bool readTaxaContents(Context &ctx);
    bool readDataContents(Context &ctx);
    bool readTreesContents(Context &ctx, const U2DbiRef &dbiRef);

    void reportProgress() { ti.setProgress(io->getProgress()); }

    // append object to objects, and resolve name conflicts
    void addObject(GObject *obj);

private:
    static const QString BEGIN;
    static const QString END;
    static const QString BLK_TAXA;
    static const QString BLK_DATA;
    static const QString BLK_CHARACTERS;
    static const QString CMD_DIMENSIONS;
    static const QString CMD_FORMAT;
    static const QString CMD_MATRIX;
    static const QString BLK_TREES;
    static const QString CMD_TREE;
    static const QString CMD_UTREE;
    static const QString CMD_TRANSLATE;

private:
    IOAdapter *io;
    const U2DbiRef& dbiRef;
    QString folder;
    U2OpStatus &ti;
    Tokenizer tz;

    QList<GObject*> objects;
    QSet<QString> objectNames;

    Context global;

    QStringList errors;
    QStringList warnings;
};

} // namespace U2

#endif	// #ifndef _U2_NEXUS_PARSER_H_
