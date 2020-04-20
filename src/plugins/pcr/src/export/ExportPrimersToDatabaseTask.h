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

#ifndef _U2_EXPORT_PRIMERS_TO_DATABASE_TASK_H_
#define _U2_EXPORT_PRIMERS_TO_DATABASE_TASK_H_

#include <U2Core/Task.h>
#include <U2Core/U2DbiUtils.h>

#include "Primer.h"

namespace U2 {

class ExportPrimersToDatabaseTask : public Task {
    Q_OBJECT
public:
    ExportPrimersToDatabaseTask(const QList<Primer> &primers, const U2DbiRef &dbiRef, const QString &folder);

    void run();
    ReportResult report();

    const QMap<U2DataId, U2DataId> &getImportedObjectIds() const;

private:
    const QList<Primer> primers;
    const U2DbiRef dbiRef;
    const QString folder;

    TmpDbiObjects dbiSequences;
    TmpDbiObjects dbiAnnotations;
    QMap<U2DataId, U2DataId> importedObjectIds;
};

}   // namespace U2

#endif // _U2_EXPORT_PRIMERS_TO_DATABASE_TASK_H_
