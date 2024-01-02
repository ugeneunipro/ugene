/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include <QSet>
#include <QStringList>

#include <U2Core/Task.h>

namespace U2 {

class MultipleAlignmentObject;
class StateLocker;

class U2VIEW_EXPORT RealignSequencesInAlignmentTask : public Task {
    Q_OBJECT
public:
    RealignSequencesInAlignmentTask(MultipleAlignmentObject* msaObject,
                                    const QSet<qint64>& sequencesToAlignIds,
                                    const QString& algorithmId);

    ~RealignSequencesInAlignmentTask() override;

    ReportResult report() override;

protected:
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    MultipleAlignmentObject* originalMsaObject = nullptr;
    MultipleAlignmentObject* msaObject = nullptr;
    const QSet<qint64> rowsToAlignIds;
    QStringList originalRowOrder;
    Task* extractSequences = nullptr;
    QString extractedSequencesDirUrl;
    StateLocker* locker = nullptr;
    QString algorithmId;
};

}  // namespace U2
