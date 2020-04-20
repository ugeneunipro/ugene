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

#ifndef _U2_GENE_BY_GENE_REPORT_TASK_H_
#define _U2_GENE_BY_GENE_REPORT_TASK_H_

#include <U2Core/AnnotationData.h>
#include <U2Core/DNASequence.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/Task.h>

namespace U2 {

//////////////////////////////////////////////////////////////////////////
//Settings
class GeneByGeneReportSettings{
public:
    GeneByGeneReportSettings();

public:
    QString     outFile;
    QString     existingFile;
    float       identity;
    QString     annName;

    static const QString MERGE_EXISTING;
    static const QString OVERWRITE_EXISTING;
    static const QString RENAME_EXISTING;

private:
    void initDefaults();
};

//////////////////////////////////////////////////////////////////////////
//Algorithm
class GeneByGeneCompareResult{
public:
    GeneByGeneCompareResult()
        :identical(false)
        ,identityString(IDENTICAL_NO)
    {}

    bool        identical;
    QString     identityString;

    static const QString IDENTICAL_YES;
    static const QString IDENTICAL_NO;
};

class GeneByGeneComparator{
public:
    GeneByGeneComparator(){}

    static GeneByGeneCompareResult compareGeneAnnotation(const DNASequence &seq, const QList<SharedAnnotationData> &annData,
        const QString &annName, float identity);

    static float parseBlastQual (const QString& ident);
};


//////////////////////////////////////////////////////////////////////////
//IO
class GeneByGeneReportIO{
public:
    GeneByGeneReportIO(const QString& _outFile, const QString& _existingMode);
    ~GeneByGeneReportIO();

    void prepareOutputFile(U2OpStatus& os);
    void writeTableItem(const QString& geneName, const QString& identicalString, U2OpStatus& os);

private:
    QString                         outFile;
    QString                         existingMode;
    QMap<QString, QList<QString> >  mergedTable;
    IOAdapter*                      io;
    int                             mergedGenomesSize;

private:
    void readMergedTable(const QString& filePath, U2OpStatus& os);
    void writeHeader(IOAdapter* io);
    void writeRow(const QList<QString>& rowData);
};


//////////////////////////////////////////////////////////////////////////
//Task
class GeneByGeneReportTask : public Task {
    Q_OBJECT
public:
    GeneByGeneReportTask(const GeneByGeneReportSettings &settings, const QMap<QString, QPair<DNASequence, QList<SharedAnnotationData> > > &geneData);
    virtual ~GeneByGeneReportTask();

    virtual void run();

    const GeneByGeneReportSettings& getSettings(){return settings;}

private:
    GeneByGeneReportSettings settings;
    QMap<QString, QPair<DNASequence, QList<SharedAnnotationData> > > geneData;

};


} //namespace U2

#endif //_U2_GENE_BY_GENE_REPORT_TASK_H_


