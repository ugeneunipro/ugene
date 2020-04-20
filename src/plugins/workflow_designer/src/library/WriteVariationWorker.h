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

#ifndef _WRITE_VARIATION_LIST_WORKER_
#define _WRITE_VARIATION_LIST_WORKER_

#include "BaseDocWriter.h"

namespace U2 {
namespace LocalWorkflow {

class WriteVariationWorker : public BaseDocWriter {
    Q_OBJECT
public:
    WriteVariationWorker(Actor *p, const DocumentFormatId& fid);

protected:
    virtual void data2doc(Document *doc, const QVariantMap &data);
    virtual void storeEntry(IOAdapter *io, const QVariantMap &data, int entryNum);
    virtual bool hasDataToWrite(const QVariantMap &data) const;
    virtual QSet<GObject *> getObjectsToWrite(const QVariantMap &data) const;
}; // WriteVariationWorker

class WriteVariationWorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;

    WriteVariationWorkerFactory() : DomainFactory(ACTOR_ID) {}
    static void init();
    virtual Worker *createWorker(Actor *a);

}; // WriteVariationWorkerFactory

} // LocalWorkflow
} // U2

#endif // _WRITE_VARIATION_LIST_WORKER_
