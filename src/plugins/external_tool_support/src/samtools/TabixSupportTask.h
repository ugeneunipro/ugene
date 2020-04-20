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

#ifndef _U2_TABIX_TASK_H_
#define _U2_TABIX_TASK_H_

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/GUrl.h>

#include <U2Formats/BgzipTask.h>

namespace U2 {

class CopyFileTask;

class TabixSupportTask : public ExternalToolSupportTask {
    Q_OBJECT
    Q_DISABLE_COPY(TabixSupportTask)
public:
    TabixSupportTask(const GUrl& fileUrl, const GUrl& outputUrl = GUrl());

    void prepare();
    QList<Task*> onSubTaskFinished(Task *subTask);

    const GUrl& getOutputBgzf() const;
    const GUrl getOutputTbi() const;
private:
    GUrl fileUrl;
    GUrl bgzfUrl;
    BgzipTask* bgzipTask;
    CopyFileTask* copyTask;
    ExternalToolRunTask* tabixTask;

    void initTabixTask();
};

}   // namespace U2

#endif // _U2_TABIX_TASK_H_
