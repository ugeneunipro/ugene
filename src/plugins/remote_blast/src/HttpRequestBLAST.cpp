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

#include <U2Core/NetworkConfiguration.h>
#include <U2Core/U2SafePoints.h>

#include "HttpRequest.h"
#include "RemoteBLASTTask.h"

namespace U2 {

const QString HttpRequestBLAST::host = "https://blast.ncbi.nlm.nih.gov/Blast.cgi?";

QString HttpRequestBLAST::runHttpRequest(const QString& request) {
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::HTTP_FILE);
    IOAdapter* io = iof->createIOAdapter();
    if (!io->open(request, IOAdapterMode_Read)) {
        error = tr("Cannot open the IO adapter");
        return "";
    }
    int offs = 0;
    int read;
    int CHUNK_SIZE = 1024;
    QByteArray response(CHUNK_SIZE, 0);
    do {
        if (task->isCanceled()) {
            io->close();
            return "";
        }
        read = io->readBlock(response.data() + offs, CHUNK_SIZE);
        offs += read;
        response.resize(offs + read);
    } while (read == CHUNK_SIZE);
    QString error = io->errorString();
    io->close();
    if (read < 0) {
        taskLog.error(tr("Cannot load a page. %1").arg(error));
        return "";
    }
    response.truncate(offs);

    return QString(response);
}

void HttpRequestBLAST::sendRequest(const QString& params, const QString& query) {
    QString request = host;
    request.append(RemoteRequestConfig::HTTP_BODY_SEPARATOR);
    request.append(params);
    request.append("&TOOL=ugene&EMAIL=ugene-ncbi-blast@unipro.ru&");
    request.append(ReqParams::sequence + "=" + query);
    taskLog.trace(QString("NCBI BLAST http request: %1").arg(request));
    QString response = runHttpRequest(request);
    if (response.isEmpty()) {
        error = tr("The response is empty");
        return;
    }
    ResponseBuffer buf;
    QByteArray qbResponse(response.toLatin1());
    buf.setBuffer(&qbResponse);
    buf.open(QIODevice::ReadOnly);
    QByteArray b = buf.readLine();
    while (!b.startsWith(QString("<!--QBlastInfoBegin\n").toLatin1())) {
        if (task->isCanceled()) {
            return;
        }
        b = buf.readLine();
        if (b.indexOf("Error: Failed to read the Blast query: Nucleotide FASTA provided for protein sequence") != -1) {
            error = tr("Nucleotide FASTA provided for protein sequence");
            return;
        }
    }
    QString requestID = buf.readLine();
    requestID = requestID.split("=")[1];
    requestID = requestID.mid(1, requestID.length() - 2);
    if (requestID == "") {
        error = tr("Cannot get the request ID");
        return;
    }
    taskLog.trace(QString("NCBI BLAST Request ID: %1").arg(requestID));
    QString estimatedRunTimeValue = buf.readLine().split('=')[1];
    bool isOk;
    int estimatedRunTimeSeconds = estimatedRunTimeValue.toInt(&isOk);
    if (!isOk) {
        error = tr("Cannot get the waiting time");
        return;
    }
    taskLog.info(QString("NCBI BLAST estimated search time: %1 seconds.").arg(estimatedRunTimeSeconds));
    taskLog.info(QString("NCBI BLAST to view results in web browser: %1").arg(host + "CMD=Get&RID=" + requestID));
    request = host + "CMD=Get&FORMAT_TYPE=XML&RID=";
    request.append(requestID);
    buf.close();
    auto rTask = qobject_cast<RemoteBlastHttpRequestTask*>(task);
    SAFE_POINT(rTask != nullptr, "Not a RemoteBlastHttpRequestTask", );
    int checkTimeSeconds = qMax(5, estimatedRunTimeSeconds / 10);
    rTask->resetProgress();
    rTask->setTimeOut(10 * estimatedRunTimeSeconds);
    int iteration = 0;
    int startTime = QDateTime::currentSecsSinceEpoch();
    do {
        if (iteration > 0) {
            QThread::sleep(checkTimeSeconds);
            rTask->updateProgress();
        }
        response = runHttpRequest(request);
        if (response.isEmpty()) {
            error = tr("The response is empty");
            return;
        }
        if (response.indexOf("301 Moved Permanently") != -1) {
            int start = response.indexOf("href=") + 6;
            QString req2 = response.mid(start, response.lastIndexOf(">here</a>") - start - 1);
            req2.remove("amp;");
            response = runHttpRequest(req2);
        }
        iteration++;
    } while (response.indexOf("Status=WAITING") != -1 && rTask->isTimeOut());

    if (response.indexOf("Status=WAITING") != -1 || response.indexOf("<BlastOutput>") == -1 || response.indexOf("</BlastOutput>") == -1) {
        error = tr("Database couldn't prepare the response. You can increase timeout and perform search again.");
        return;
    }

    if (response.contains("Error: CPU usage limit was exceeded")) {
        error = tr("NCBI BLAST web server returned \"CPU usage limit was exceeded\" error. Probably, the query sequence is too large.");
        return;
    }

    taskLog.details(tr("NCBI BLAST received result in %1 seconds").arg(QDateTime::currentSecsSinceEpoch() - startTime));
    output = response.toLatin1();
    parseResult(response.toLatin1());
}

QByteArray HttpRequestBLAST::getOutputFile() {
    return output;
}

void HttpRequestBLAST::parseResult(const QByteArray& buf) {
    QDomDocument xmlDoc;
    QString xmlError;
    xmlDoc.setContent(buf, false, &xmlError);
    if (!xmlError.isEmpty()) {
        error = tr("Cannot read the response");
        return;
    }
    QDomNodeList hits = xmlDoc.elementsByTagName("Hit");
    for (int i = 0; i < hits.count(); i++) {
        parseHit(hits.at(i));
    }

    auto rTask = qobject_cast<RemoteBlastHttpRequestTask*>(task);
    SAFE_POINT(rTask != nullptr, "Not a RemoteBlastHttpRequestTask", );
    for (int i = rTask->getProgress(); i < 100; i++) {
        rTask->updateProgress();
    }
}

void HttpRequestBLAST::parseHit(const QDomNode& xml) {
    QDomElement tmp = xml.lastChildElement("Hit_id");
    QString id = tmp.text();
    tmp = xml.lastChildElement("Hit_def");
    QString def = tmp.text();
    tmp = xml.lastChildElement("Hit_accession");
    QString accession = tmp.text();

    tmp = xml.lastChildElement("Hit_len");
    QString hitLen = tmp.text();

    QDomNodeList nodes = xml.childNodes();
    for (int i = 0; i < nodes.count(); i++) {
        if (nodes.at(i).isElement()) {
            if (nodes.at(i).toElement().tagName() == "Hit_hsps") {
                QDomNodeList hsps = nodes.at(i).childNodes();
                for (int j = 0; j < hsps.count(); j++) {
                    if (hsps.at(j).toElement().tagName() == "Hsp") {
                        parseHsp(hsps.at(j), id, def, accession, hitLen);
                    }
                }
            }
        }
    }
}

void HttpRequestBLAST::parseHsp(const QDomNode& xml, const QString& id, const QString& def, const QString& accession, const QString& hitLen) {
    SharedAnnotationData ad(new AnnotationData());
    bool isOk;

    QDomElement elem = xml.lastChildElement("Hsp_bit-score");
    if (!elem.isNull()) {
        ad->qualifiers << U2Qualifier("bit-score", elem.text());
    }

    elem = xml.lastChildElement("Hsp_score");
    if (!elem.isNull()) {
        ad->qualifiers << U2Qualifier("score", elem.text());
    }

    elem = xml.lastChildElement("Hsp_evalue");
    if (!elem.isNull()) {
        ad->qualifiers << U2Qualifier("E-value", elem.text());
    }

    elem = xml.lastChildElement("Hsp_query-from");
    int from = elem.text().toInt(&isOk);
    if (!isOk) {
        error = tr("Cannot get the location");
        return;
    }

    elem = xml.lastChildElement("Hsp_query-to");
    int to = elem.text().toInt(&isOk);
    if (!isOk) {
        error = tr("Cannot get the location");
        return;
    }

    elem = xml.lastChildElement("Hsp_hit-from");
    if (!elem.isNull()) {
        ad->qualifiers << U2Qualifier("hit-from", elem.text());
    }

    elem = xml.lastChildElement("Hsp_hit-to");
    if (!elem.isNull()) {
        ad->qualifiers << U2Qualifier("hit-to", elem.text());
    }

    elem = xml.lastChildElement("Hsp_hit-frame");
    int frame = elem.text().toInt(&isOk);
    if (!isOk) {
        error = tr("Cannot get the location");
        return;
    }
    QString frameQualifierValue = frame < 0 ? "complement" : "direct";
    ad->qualifiers << U2Qualifier("source_frame", frameQualifierValue);
    ad->setStrand(frame < 0 ? U2Strand::Complementary : U2Strand::Direct);

    elem = xml.lastChildElement("Hsp_identity");
    int identities = elem.text().toInt(&isOk);
    if (!isOk) {
        error = tr("Cannot get the identity");
        return;
    }

    elem = xml.lastChildElement("Hsp_gaps");
    int gaps = elem.text().toInt(&isOk);
    if (!isOk) {
        error = tr("Cannot evaluate the gaps");
        return;
    }

    elem = xml.lastChildElement("Hsp_align-len");
    int align_len = elem.text().toInt(&isOk);
    if (!isOk) {
        error = tr("Cannot get the alignment length");
        return;
    }

    if (from != -1 && to != -1) {
        if (to > from) {  // direct
            ad->location->regions << U2Region(from - 1, to - from + 1);
            ad->setStrand(U2Strand::Direct);
        } else {  // complement
            ad->location->regions << U2Region(to - 1, from - to + 1);
            ad->setStrand(U2Strand::Complementary);
        }
    } else {
        error = tr("Cannot evaluate the location");
        return;
    }

    if (align_len != -1) {
        if (gaps != -1) {
            float percent = (float)gaps / (float)align_len * 100.;
            QString str = QString::number(gaps) + "/" + QString::number(align_len) + " (" + QString::number(percent, 'g', 4) + "%)";
            ad->qualifiers << U2Qualifier("gaps", str);
        }
        if (identities != -1) {
            float percent = (float)identities / (float)align_len * 100.;
            QString str = QString::number(identities) + '/' + QString::number(align_len) + " (" + QString::number(percent, 'g', 4) + "%)";
            ad->qualifiers << U2Qualifier("identities", str);
        }
    }

    ad->qualifiers << U2Qualifier("id", id);
    ad->qualifiers << U2Qualifier("def", def);
    ad->qualifiers << U2Qualifier("accession", accession);
    ad->qualifiers << U2Qualifier("hit_len", hitLen);
    ad->name = "blast result";
    result.append(ad);
}

}  // namespace U2
