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

#ifndef _U2_CLONE_OBJECT_TASK_H_
#define _U2_CLONE_OBJECT_TASK_H_

#include <QPointer>

#include <U2Core/Task.h>
#include <U2Core/U2Type.h>

namespace U2 {

class Document;
class GObject;

class U2CORE_EXPORT CloneObjectTask : public Task {
    Q_OBJECT
public:
    CloneObjectTask(GObject *srcObj, Document *dstDoc, const QString &dstFolder);
    CloneObjectTask(GObject *srcObj, const U2DbiRef &dstDbiRef, const QString &dstFolder);
    ~CloneObjectTask();
    void run();

    GObject * takeResult();
    const QString & getFolder() const;
    GObject * getSourceObject() const;
    Document * getDocument() const;

private:
    QPointer<GObject> srcObj;
    QPointer<Document> dstDoc;
    U2DbiRef dstDbiRef;
    QString dstFolder;
    GObject *dstObj;
};

} // U2

#endif // _U2_CLONE_OBJECT_TASK_H_
