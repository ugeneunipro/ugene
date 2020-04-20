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

#ifndef _FIND_EXON_REGIONS_TASK_H_
#define _FIND_EXON_REGIONS_TASK_H_

#include <U2Core/U2Region.h>
#include <U2Core/Task.h>

namespace U2 {

class LoadRemoteDocumentTask;
class SplicedAlignmentTask;
class U2SequenceObject;

class FindExonRegionsTask : public Task
{
    Q_OBJECT
public:
                                FindExonRegionsTask( U2SequenceObject *dnaObj,
                                    const QString &exonAnnotaitonName );
    QList<U2Region>             getRegions( ) { return exonRegions; }

    void                        prepare( );
    QList<Task *>               onSubTaskFinished( Task *subTask );
    ReportResult                report( );

private:
    QList<U2Region>             exonRegions;
    U2SequenceObject *          dnaObj;    
    QString                     exonAnnName;
};

} // namespace U2

#endif // _FIND_EXON_REGIONS_TASK_H_
