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

#ifndef _U2_RPS_BLAST_SUPPORT_TASK_H_
#define _U2_RPS_BLAST_SUPPORT_TASK_H_

#include "BlastPlusSupportCommonTask.h"

#include <U2Algorithm/CDSearchTaskFactory.h>


namespace U2 {

class RPSBlastSupportTask : public BlastPlusSupportCommonTask {
    Q_OBJECT
public:
    RPSBlastSupportTask(const BlastTaskSettings& settings) : BlastPlusSupportCommonTask(settings) {}
    virtual ExternalToolRunTask* createBlastPlusTask();
};

class LocalCDSearch : public CDSearchResultListener {
public:
    LocalCDSearch(const CDSearchSettings& settings);
    virtual Task * getTask() const { return task; }
    virtual QList<SharedAnnotationData> getCDSResults() const;
private:
    RPSBlastSupportTask* task;
};

class CDSearchLocalTaskFactory : public CDSearchFactory {
public:
    virtual CDSearchResultListener* createCDSearch(const CDSearchSettings& settings) const { return new LocalCDSearch(settings); }
};

} //namespace



#endif
