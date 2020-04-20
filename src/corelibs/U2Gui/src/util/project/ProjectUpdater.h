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

#ifndef _U2_PROJECTUPDATER_H_
#define _U2_PROJECTUPDATER_H_

#include <QThread>
#include <QMutex>

#include <U2Core/U2Type.h>

namespace U2 {

class Document;
class DocumentFoldersUpdate;

class ProjectUpdater : public QThread {
public:
    ProjectUpdater();

    void run();
    void stop();

    void invalidate(const Document *doc);
    bool takeData(Document *doc, DocumentFoldersUpdate &result);

    void addDocument(Document *doc);
    void removeDocument(Document *doc);

private:
    QList<U2DbiRef> getDbiRefs();
    void readData();
    void updateAccessedObjects();
    void fetchObjectsInUse(const U2DbiRef &dbiRef, U2OpStatus &os);

private:
    QMutex mutex;
    QAtomicInt stopped;
    QHash<U2DbiId, DocumentFoldersUpdate> data;
    QList<Document *> docs;
    QHash<U2DbiId, bool> valid;
};

} // U2

#endif // _U2_PROJECTUPDATER_H_
