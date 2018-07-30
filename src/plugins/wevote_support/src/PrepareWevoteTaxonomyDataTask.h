/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_PREPARE_WEVOTE_TAXONOMY_DATA_TASK_H_
#define _U2_PREPARE_WEVOTE_TAXONOMY_DATA_TASK_H_

#include <U2Core/Task.h>

namespace U2 {

namespace FileStorage {
class WorkflowProcess;
}

class PrepareWevoteTaxonomyDataTask : public Task {
public:
    PrepareWevoteTaxonomyDataTask(FileStorage::WorkflowProcess &workflowProcess);

    const QString &getWevoteTaxonomyDir() const;

private:
    void run();
    ReportResult report();

    bool isActual() const;
    void prepareNamesFile();
    void prepareNodesFile();

    FileStorage::WorkflowProcess &workflowProcess;
    QString taxonomyNamesUrl;
    QString taxonomyNodesUrl;
    bool removeDestinationFiles;
    QString wevoteTaxonomyDir;

    static const QString WEVOTE_DIR;
    static const QString WEVOTE_NODES;
    static const QString WEVOTE_NAMES;
    static const qint64 BUFFER_SIZE;
    static const QString SCIENTIFIC_NAME;
};

}   // namespace U2

#endif // _U2_PREPARE_WEVOTE_TAXONOMY_DATA_TASK_H_
