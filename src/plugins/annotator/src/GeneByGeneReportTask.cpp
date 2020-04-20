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

#include "GeneByGeneReportTask.h"

#include <U2Core/Counter.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapterUtils.h>

#include <QFile>
#include <QSet>

namespace U2 {
//////////////////////////////////////////////////////////////////////////
//GeneByGeneReportSettings

const QString GeneByGeneReportSettings::MERGE_EXISTING = "Merge";
const QString GeneByGeneReportSettings::OVERWRITE_EXISTING = "Overwrite";
const QString GeneByGeneReportSettings::RENAME_EXISTING = "Rename";

GeneByGeneReportSettings::GeneByGeneReportSettings(){
    initDefaults();
}

void GeneByGeneReportSettings::initDefaults(){
    outFile = "";
    existingFile = GeneByGeneReportSettings::MERGE_EXISTING;
    identity = 90.0f;
    annName = "blast_result";
}

//////////////////////////////////////////////////////////////////////////
//GeneByGeneCompareResult
const QString GeneByGeneCompareResult::IDENTICAL_YES = "Yes";
const QString GeneByGeneCompareResult::IDENTICAL_NO = "No";

//////////////////////////////////////////////////////////////////////////
//GeneByGeneComparator

#define BLAST_IDENT "identities"
#define BLAST_GAPS "gaps"
GeneByGeneCompareResult GeneByGeneComparator::compareGeneAnnotation(const DNASequence& seq, const QList<SharedAnnotationData> &annData,
    const QString& annName, float identity)
{
    GeneByGeneCompareResult result;

    float maxIdentity = -1.0F;
    foreach (const SharedAnnotationData &adata, annData) {
        if (adata->name == annName) {
            U2Location location = adata->location;
            if (location->isSingleRegion()) {
                int reglen = location->regions.first().length;
                float lenRatio  = reglen * 100 /static_cast<float>(seq.length());
                maxIdentity = qMax(maxIdentity, lenRatio);
                if(lenRatio >= identity){ //check length ratio
                    QString ident = adata->findFirstQualifierValue(BLAST_IDENT);
                    if (!ident.isEmpty()){
                        //create BLAST string  YES/identity/gaps
                        float blastIdent = parseBlastQual(ident);
                        if (blastIdent != -1.0f && blastIdent >= identity){
                            result.identical = true;
                            result.identityString = GeneByGeneCompareResult::IDENTICAL_YES;
                            result.identityString.append(QString("\\%1").arg(blastIdent));
                            QString gaps = adata->findFirstQualifierValue(BLAST_GAPS);
                            if (!gaps.isEmpty()){
                                float blastGaps = parseBlastQual(gaps);
                                if (blastGaps!=1.0f){
                                    result.identityString.append(QString("\\%1").arg(blastGaps));
                                }
                            }else{
                                result.identityString.append(QString("\\0"));
                            }
                        }
                    }else{ //not a blast annotation
                        result.identical = true;
                        result.identityString = GeneByGeneCompareResult::IDENTICAL_YES;
                    }
                }
            }
            break;
        }
    }

    if (!result.identical && maxIdentity != -1.0f){
        result.identityString.append(QString("\\%1").arg(maxIdentity));
    }

    return result;
}

float GeneByGeneComparator::parseBlastQual( const QString& ident ){
    float res = -1.0f;

    //identities="1881/1881 (100%)"
    QRegExp rx("(\\d+)/(\\d+)");
    if (rx.indexIn(ident) != -1){
        int v1 = rx.cap(1).toInt();
        int v2 = rx.cap(2).toInt();

        if (v2 != 0){
            res = v1 *100 / static_cast<float>(v2);
        }
    }
    return res;
}


//////////////////////////////////////////////////////////////////////////
//GeneByGeneReportIO
GeneByGeneReportIO::GeneByGeneReportIO( const QString& _outFile, const QString& _existingMode )
:outFile(_outFile)
,existingMode(_existingMode)
,io(NULL)
,mergedGenomesSize(0){

}

GeneByGeneReportIO::~GeneByGeneReportIO(){
    const QList<QString>& keys = mergedTable.keys();

    foreach(const QString& key, keys){
        QList<QString> toWrite;
        toWrite.append(key);
        toWrite.append(mergedTable.take(key));
        toWrite.append(GeneByGeneCompareResult::IDENTICAL_NO);
        writeRow(toWrite);
    }

    if(io != NULL){
        io->close();
        delete io;
        io = NULL;
    }
}

void GeneByGeneReportIO::prepareOutputFile( U2OpStatus& os ){
    if(QFile::exists(outFile)){
        if (GeneByGeneReportSettings::RENAME_EXISTING == existingMode){
            outFile = GUrlUtils::rollFileName(outFile, QSet<QString>());
        }else if (GeneByGeneReportSettings::MERGE_EXISTING == existingMode){
            readMergedTable(outFile, os);
            if(os.hasError()){
                return;
            }
        }
        //over write otherwise
    }

    io = IOAdapterUtils::open(outFile, os, IOAdapterMode_Write);
    if (os.hasError()){
        io = NULL;
        return;
    }
    writeHeader(io);
}

void GeneByGeneReportIO::writeTableItem( const QString& geneName, const QString& identicalString, U2OpStatus& os ){
    if (io == NULL){
        os.setError("Gene by gene writer has not prepared an output file");
        return;
    }

    QList<QString> toWrite;
    toWrite.append(geneName);

    if (!mergedTable.isEmpty() || mergedGenomesSize > 0){
        QList<QString> oldIdentities;
        if (mergedTable.contains(geneName)){
            oldIdentities = mergedTable.take(geneName);
        }else{
            for(int i = 0; i < mergedGenomesSize; i++){
                oldIdentities.append(GeneByGeneCompareResult::IDENTICAL_NO);
            }
        }
        toWrite.append(oldIdentities);
    }

    toWrite.append(identicalString);

    writeRow(toWrite);
}

#define READ_BUF_SIZE 2*4096
void GeneByGeneReportIO::readMergedTable( const QString& filePath, U2OpStatus& os ){
    QScopedPointer<IOAdapter> readIO(IOAdapterUtils::open(filePath, os, IOAdapterMode_Read));
    if (os.hasError()){
        return;
    }

    QByteArray data;
    data.resize(READ_BUF_SIZE);

    int columnsCount = -1;

    qint64 len = 0;
    while(!readIO->isEof()){
        len = readIO->readLine(data.data(), data.size() - 1);
        data.data()[len] = '\0';
        data = data.trimmed();
        if (data.isEmpty() || data.startsWith("#")){ //skip comments
            continue;
        }
        const QList<QByteArray>& columns = data.left(len).split('\t');

        //check/get columns count
        if (columnsCount == -1 && columns.size() > 1){
            columnsCount = columns.size() - 1; //exclude gene column
            mergedGenomesSize = columnsCount;
        }else if (columnsCount != columns.size() - 1 ){
            continue; // bad columns count
        }

        if (columnsCount!= -1){
            QList<QString> identities;
            QString geneName = columns[0];
            for ( int i = 0 ; i < columnsCount; i++){
                QString ident = columns[i + 1];
                identities.append(ident);
            }
            mergedTable.insert(geneName, identities);
        }
    }

    if (mergedTable.isEmpty()){
        mergedGenomesSize = 0;
    }

    readIO->close();
}

void GeneByGeneReportIO::writeHeader( IOAdapter* io ){
    if(io == NULL){
        return;
    }

    QString header;

    header.append("#Characteristic format: (Yes) \\ (blast identities %) \\ (blast gaps %)\n");
    header.append("#Characteristic format: (NO) \\ (the longest similar region and sequence length ratio %) \n");
    header.append("#Gene");

    for (int i = 0; i < mergedGenomesSize + 1; i++ ){
        header.append(QString("\tGenome%1").arg(i+1));
    }

    header.append("\n");
    io->writeBlock(header.toLatin1());
}

void GeneByGeneReportIO::writeRow( const QList<QString>& rowData ){
    if (io == NULL){
        return;
    }

    QString toWrite;

    bool first = true;
    foreach(const QString& item, rowData){
        if (!first){
            toWrite.append("\t");
        }else{
            first = false;
        }
        toWrite.append(item);
    }
    toWrite.append("\n");

    io->writeBlock(toWrite.toLatin1());
}

//////////////////////////////////////////////////////////////////////////
//GeneByGeneReportTask
GeneByGeneReportTask::GeneByGeneReportTask(const GeneByGeneReportSettings &settings,
    const QMap<QString, QPair<DNASequence, QList<SharedAnnotationData> > > &geneData)
    : Task(tr("Generating gene-by-gene approach report"), TaskFlag_None), settings(settings), geneData(geneData)
{

}

GeneByGeneReportTask::~GeneByGeneReportTask(){
    geneData.clear();
}

void GeneByGeneReportTask::run(){
    if (isCanceled()){
        return;
    }
    GCOUNTER(cvar, tvar, "GeneByGeneTask");

    GeneByGeneReportIO io(settings.outFile, settings.existingFile);
    io.prepareOutputFile(stateInfo);
    if (stateInfo.hasError()){
        return;
    }

    stateInfo.progress = 0;
    float progressCounter = 0;

    const QList<QString>& keys = geneData.keys();
    float progressStep  = keys.size() / static_cast<float>(100);

    foreach(const QString& key, keys){
        if (isCanceled()){
            return;
        }

        {
            const QPair<DNASequence, QList<SharedAnnotationData> >& seqAnnData = geneData[key];
            const GeneByGeneCompareResult& res = GeneByGeneComparator::compareGeneAnnotation(seqAnnData.first, seqAnnData.second, settings.annName, settings.identity);
            io.writeTableItem(key, res.identityString, stateInfo);
            if (stateInfo.hasError()){
                return;
            }
        }

        progressCounter+=progressStep;
        stateInfo.progress = static_cast<int>(progressCounter + 0.5f);
    }

    stateInfo.progress = 100;

}

}//namespace
