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

#ifndef _U2_RPS_BLAST_SUPPORT_TASK_H_
#define _U2_RPS_BLAST_SUPPORT_TASK_H_

#include <U2Algorithm/CDSearchTaskFactory.h>

#include "BlastCommonTask.h"

namespace U2 {

class RPSBlastTask : public BlastCommonTask {
    Q_OBJECT
public:
    RPSBlastTask(const BlastTaskSettings& settings)
        : BlastCommonTask(settings) {
    }
    ExternalToolRunTask* createBlastTask() override;
};

class LocalCDSearch : public CDSearchResultListener {
public:
    LocalCDSearch(const CDSearchSettings& settings);
    Task* getTask() const override {
        return task;
    }
    QList<SharedAnnotationData> getCDSResults() const override;

private:
    RPSBlastTask* task;
};

class CDSearchLocalTaskFactory : public CDSearchFactory {
public:
    virtual CDSearchResultListener* createCDSearch(const CDSearchSettings& settings) const {
        return new LocalCDSearch(settings);
    }
};

}  // namespace U2

#endif
