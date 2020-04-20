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

#ifndef _U2_EXPORT_MCA_2_MSA_TASK_H_
#define _U2_EXPORT_MCA_2_MSA_TASK_H_

#include <U2Core/DocumentProviderTask.h>

namespace U2 {

class ConvertMca2MsaTask;
class ExportAlignmentTask;
class MultipleChromatogramAlignmentObject;

class ExportMca2MsaTask : public DocumentProviderTask {
    Q_OBJECT
public:
    ExportMca2MsaTask(MultipleChromatogramAlignmentObject *mcaObject, const QString &fileName, const DocumentFormatId &formatId, bool includeReference);

private:
    void prepare();
    QList<Task *> onSubTaskFinished(Task *subTask);

    MultipleChromatogramAlignmentObject *mcaObject;
    QString fileName;
    DocumentFormatId formatId;
    const bool includeReference;

    ConvertMca2MsaTask *convertTask;
    ExportAlignmentTask *exportTask;
};

}   // namespace U2

#endif // _U2_EXPORT_MCA_2_MSA_TASK_H_
