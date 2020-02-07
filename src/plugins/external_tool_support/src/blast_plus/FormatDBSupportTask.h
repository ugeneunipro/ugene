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

#ifndef _U2_FORMATDB_SUPPORT_TASK_H
#define _U2_FORMATDB_SUPPORT_TASK_H

#include <U2Core/ExternalToolRunTask.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/Task.h>

#include "utils/ExportTasks.h"

namespace U2 {

class PrepareInputFastaFilesTask;

/*Options for FormatDB
-t  Title for database file [String]  Optional
  -i  Input file(s) for formatting [File In]  Optional
  -l  Logfile name: [File Out]  Optional
    default = formatdb.log
  -p  Type of file
         T - protein
         F - nucleotide [T/F]  Optional
    default = T
  -o  Parse options
         T - True: Parse SeqId and create indexes.
         F - False: Do not parse SeqId. Do not create indexes.
 [T/F]  Optional
    default = F
  -a  Input file is database in ASN.1 format (otherwise FASTA is expected)
         T - True,
         F - False.
 [T/F]  Optional
    default = F
  -b  ASN.1 database in binary mode
         T - binary,
         F - text mode.
 [T/F]  Optional
    default = F
  -e  Input is a Seq-entry [T/F]  Optional
    default = F
  -n  Base name for BLAST files [String]  Optional
  -v  Database volume size in millions of letters [Integer]  Optional
    default = 4000
  -s  Create indexes limited only to accessions - sparse [T/F]  Optional
    default = F
  -V  Verbose: check for non-unique string ids in the database [T/F]  Optional
    default = F
  -L  Create an alias file with this name
        use the gifile arg (below) if set to calculate db size
        use the BLAST db specified with -i (above) [File Out]  Optional
  -F  Gifile (file containing list of gi's) [File In]  Optional
  -B  Binary Gifile produced from the Gifile specified above [File Out]  Optional
  -T  Taxid file to set the taxonomy ids in ASN.1 deflines [File In]  Optional
*/

class FormatDBSupportTaskSettings {
public:
    FormatDBSupportTaskSettings() {reset();}
    void reset();

    QStringList     inputFilesPath;
    QString         outputPath;
    QString         databaseTitle;
    QString         tempDirPath;
    /*-p  Type of file
            T - protein
            F - nucleotide*/
    bool            isInputAmino;
};


class FormatDBSupportTask : public Task {
    Q_OBJECT
public:
    FormatDBSupportTask(const QString& id, const FormatDBSupportTaskSettings& settings);

private:
    void prepare();
    QList<Task *> onSubTaskFinished(Task *subTask);
    Task::ReportResult report();
    QString generateReport() const;

    QString                     prepareTempDir();
    QString                     prepareLink(const QString &path) const;
    void                        createFormatDbTask();

    QString                     externalToolLog;
    PrepareInputFastaFilesTask *prepareTask;
    ExternalToolRunTask*        formatDBTask;
    QString                     toolId;
    FormatDBSupportTaskSettings settings;

    QStringList                 inputFastaFiles;
    QStringList                 fastaTmpFiles;
};

}   // namespace U2

#endif // _U2_FORMATDB_SUPPORT_TASK_H
