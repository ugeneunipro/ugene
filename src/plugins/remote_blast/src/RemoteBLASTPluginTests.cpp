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

#include <U2Core/DNASequenceObject.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2DbiRegistry.h>

#include "RemoteBLASTPluginTests.h"

namespace U2 {

void GTest_RemoteBLAST::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    ao = NULL;
    task = NULL;
    sequence = el.attribute(SEQUENCE_ATTR);
    if (sequence.isEmpty()) {
        failMissingValue(SEQUENCE_ATTR);
        return;
    }

    algoritm = el.attribute(ALG_ATTR);
    if(algoritm.isEmpty()) {
        failMissingValue(ALG_ATTR);
        return;
    }


    QString database = el.attribute(DATABASE_ATTR);
    if(database.isEmpty()) {
        failMissingValue(DATABASE_ATTR);
        return;
    }
    bool isCddSearch = algoritm == "blastp" && (0 == database.compare("cdd", Qt::CaseInsensitive) || database.contains("oasis", Qt::CaseInsensitive));
    if(isCddSearch) {
        request = "CMD=Put";
        addParametr(request,ReqParams::program, algoritm);
        addParametr(request,ReqParams::database, database);
        addParametr(request,ReqParams::service, "rpsblast");

        QString eValue = el.attribute(EVALUE_ATTR);
        if(eValue.isEmpty()) {
            failMissingValue(EVALUE_ATTR);
            return;
        }
        addParametr(request,ReqParams::expect,eValue);

        QString hits = el.attribute(HITS_ATTR);
        if(hits.isEmpty()) {
            failMissingValue(HITS_ATTR);
            return;
        }
        addParametr(request,ReqParams::hits,hits);
    }
    else {
        request = "CMD=Put";
        addParametr(request,ReqParams::program,algoritm);
        addParametr(request,ReqParams::database,database);

        QString megablast = el.attribute(MEGABLAST_ATTR);
        if(megablast.isEmpty()) {
            megablast = "no";
        }
        addParametr(request,ReqParams::megablast,megablast);

        QString hits = el.attribute(HITS_ATTR);
        if(hits.isEmpty()) {
            failMissingValue(HITS_ATTR);
            return;
        }
        addParametr(request,ReqParams::hits,hits);

        QString eValue = el.attribute(EVALUE_ATTR);
        if(eValue.isEmpty()) {
            failMissingValue(EVALUE_ATTR);
            return;
        }

        QString wordSize = el.attribute(WORD_SIZE_ATTR);
        if(wordSize.isEmpty()) {
            failMissingValue(WORD_SIZE_ATTR);
            return;
        }

        QString filter = el.attribute(FILTERS_ATTR);
        bool isOk;
        int shortSeq = el.attribute(SHORTSEQ_ATTR).toInt(&isOk);
        if(!isOk) {
            failMissingValue(SHORTSEQ_ATTR);
            return;
        }
        if(shortSeq == 1) {
            eValue = "1000";
            filter = "";
            wordSize = "7";
        }
        addParametr(request,ReqParams::expect,eValue);
        addParametr(request,ReqParams::wordSize,wordSize);
        addParametr(request,ReqParams::filter,filter);

        QString gapCost = el.attribute(GAP_ATTR);
        if(gapCost.isEmpty()) {
            failMissingValue(GAP_ATTR);
            return;
        }
        addParametr(request,ReqParams::gapCost,gapCost);

        QString entrezQuery = el.attribute(ENTREZ_QUERY_ATTR);
        if(false == entrezQuery.isEmpty())
            addParametr(request,ReqParams::entrezQuery,entrezQuery);

        QString alph = el.attribute(ALPH_ATTR);
        if(alph.isEmpty()) {
            failMissingValue(ALPH_ATTR);
            return;
        }
        if(alph == "nucleo") {
            QString scores = el.attribute(MATCHSCORE_ATTR);
            if(scores.isEmpty()) {
                failMissingValue(MATCHSCORE_ATTR);
                return;
            }
            QString match = scores.split(" ").first();
            QString mismatch = scores.split(" ").last();
            addParametr(request,ReqParams::matchScore,match);
            addParametr(request,ReqParams::mismatchScore,mismatch);
        }
        else {
            QString matrix = el.attribute(MATRIX_ATTR);
            if(matrix.isEmpty()) {
                failMissingValue(MATRIX_ATTR);
                return;
            }
            addParametr(request,ReqParams::matrix,matrix);

            QString service = el.attribute(SERVICE_ATTR);
            if(service.isEmpty()) {
                service = "plain";
            }
            addParametr(request,ReqParams::service,service);
            if(service=="phi") {
                QString phiPattern = el.attribute(PATTERN_ATTR);
                addParametr(request,ReqParams::phiPattern,phiPattern);
            }
        }
    }

    if(!isCddSearch) {
        bool isOk;
        maxLength = el.attribute(MAX_LEN_ATTR).toInt(&isOk);
        if (!isOk) {
            stateInfo.setError( QString("value not set %1, or unable to convert to integer ").arg(MAX_LEN_ATTR));
            return;
        }

        minLength = el.attribute(MIN_LEN_ATTR).toInt(&isOk);
        if (!isOk) {
            stateInfo.setError( QString("value not set %1, or unable to convert to integer ").arg(MIN_LEN_ATTR));
            return;
        }
    }

    QString expected = el.attribute(EXPECTED_ATTR);
    if (!expected.isEmpty()) {
        QStringList expectedList = expected.split(QRegExp("\\,"));
        foreach(QString id, expectedList) {
            expectedResults.append(id);
        }
    }

    QString simpleStr = el.attribute(SIMPLE_ATTR);
    if (simpleStr == "true") {
        simple = true;
    }else{
        simple = false;
    }
}

void GTest_RemoteBLAST::prepare() {
    QByteArray query(sequence.toLatin1());
    const U2DbiRef dbiRef = AppContext::getDbiRegistry()->getSessionTmpDbiRef(stateInfo);
    SAFE_POINT_OP(stateInfo,);
    ao = new AnnotationTableObject("aaa", dbiRef);
    RemoteBLASTTaskSettings cfg;
    cfg.dbChoosen = algoritm;
    cfg.aminoT = NULL;
    cfg.complT = NULL;
    cfg.query = query;
    cfg.retries = 600;
    cfg.filterResult = 0;
    cfg.useEval = 0;
    cfg.params = request;
    task = new RemoteBLASTToAnnotationsTask(cfg, 0, ao, "", QString("result"), "");
    addSubTask(task);
}

Task::ReportResult GTest_RemoteBLAST::report() {
    QStringList result;
    if(task->hasError()) {
        stateInfo.setError("");
        return ReportResult_Finished;
    }
    if (ao != NULL){
        QList<Annotation *> alist(ao->getAnnotations());
        foreach (Annotation *an, alist) {
            foreach (const U2Qualifier &q, an->getQualifiers()) {
                if (q.name == "accession") {
                    if(!result.contains(q.value)) //Don't count different hsp
                        result.append(q.value);
                }
            }
        }
    }

    if (simple) {
        if (result.isEmpty()) {
            stateInfo.setError(QString("Simplified test returns empty result"));
        }
        return ReportResult_Finished;
    }

    if(result.size() != expectedResults.size()){
        stateInfo.setError( QString("Expected and Actual sizes of lists of regions are different: %1 %2").arg(expectedResults.size()).arg(result.size()));
        return ReportResult_Finished;
    }
    result.sort(); expectedResults.sort();
    QStringListIterator e(expectedResults), a(result);
    for (; e.hasNext();) {
        QString exp = e.next(), act =  a.next();
    }
    if (result != expectedResults) {
        //stateInfo.setError( QString("Expected and actual id's not equal"));
        QString res = "";
        foreach(const QString &str, result) {
            res.append(str);
            res.append("  ");
        }
        stateInfo.setError( QString("Expected and actual id's not equal: %1").arg(res));
        return ReportResult_Finished;
    }
    return ReportResult_Finished;
}

void GTest_RemoteBLAST::cleanup() {
    if(ao != NULL){
        delete ao;
        ao = NULL;
    }

    XmlTest::cleanup();
}

}
