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

#include "EnzymesIO.h"

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/Log.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/L10n.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2AlphabetUtils.h>

#include <U2Gui/DialogUtils.h>
#include <U2Core/Settings.h>

#include <QDir>

namespace U2 {

QString EnzymesIO::getFileDialogFilter() {
    return DialogUtils::prepareFileFilter(tr("Bairoch format"), QStringList("bairoch"));
}

QList<SEnzymeData> EnzymesIO::readEnzymes(const QString& url, TaskStateInfo& ti) {
    QList<SEnzymeData> res;

    IOAdapterId ioId = IOAdapterUtils::url2io(url);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioId);
    if (iof == NULL) {
        ti.setError(  tr("Unsupported URI type") );
        return res;
    }

    EnzymeFileFormat f = detectFileFormat(url);
    if (ti.hasError()) {
        return res;
    }
    switch(f) {
        case EnzymeFileFormat_Bairoch:
            res = readBairochFile(url, iof, ti);
            break;
        default:
            ti.setError(  tr("Unsupported enzymes file format") );
            break;
    }

    QList<SEnzymeData> resToDelete;
    //assign alphabet if needed.
    for (int i=0, n = res.count(); i<n; i++) {
        SEnzymeData& d = res[i];
        if (d->seq == QByteArray("?")){
            algoLog.trace(tr("The enzyme '%1' has unknown sequence").arg(d->id));
            resToDelete.append(d);
        }else{
            if (d->alphabet == NULL) {
                d->alphabet = U2AlphabetUtils::findBestAlphabet(d->seq);
                if(!d->alphabet){
                    algoLog.error(tr("No enzyme alphabet: '%1', sequence '%2'")
                        .arg(d->id).arg(QString(d->seq)));
                }
                if (!d->alphabet->isNucleic()) {
                    algoLog.error(tr("Non-nucleic enzyme alphabet: '%1', alphabet: %2, sequence '%3'")
                        .arg(d->id).arg(d->alphabet->getId()).arg(QString(d->seq)));
                }
            }
        }
    }
    //Remove not needed elements
    foreach(SEnzymeData d, resToDelete){
        res.removeAll(d);
    }

    return res;
}

void EnzymesIO::writeEnzymes(const QString& url, const QString& source, const QSet<QString>& enzymes, TaskStateInfo& ti) {
    IOAdapterId ioId = IOAdapterUtils::url2io(url);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioId);
    if (iof == NULL) {
        ti.setError(  tr("Unsupported URI type") );
        return;
    }

    IOAdapterId srcioId = IOAdapterUtils::url2io(source);
    IOAdapterFactory* srciof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(srcioId);
    if (srciof == NULL) {
        ti.setError(  tr("Unsupported URI type") );
        return;
    }

    EnzymeFileFormat f = detectFileFormat(source);
    if (ti.hasError()) {
        return;
    }
    switch(f) {
        case EnzymeFileFormat_Bairoch:
            writeBairochFile(url, iof, source, srciof, enzymes, ti);
            break;
        default:
            ti.setError(  tr("Unsupported enzymes file format") );
            break;
    }
}

EnzymeFileFormat EnzymesIO::detectFileFormat(const QString& url) {
    QByteArray data= IOAdapterUtils::readFileHeader(url, 256);
    if (data.startsWith("CC ") || data.startsWith("ID ")) {
        return EnzymeFileFormat_Bairoch;
    }
    return EnzymeFileFormat_Unknown;

}

QList<SEnzymeData> EnzymesIO::readBairochFile(const QString& url, IOAdapterFactory* iof, TaskStateInfo& ti) {
    QList<SEnzymeData> res;

    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Read)) {
        ti.setError(L10N::errorOpeningFileRead(url));
        return res;
    }

    SEnzymeData currentData(new EnzymeData());
    QByteArray buffArr(DocumentFormat::READ_BUFF_SIZE, 0);
    char* buff = buffArr.data();
    const QBitArray& LINE_BREAKS = TextUtils::LINE_BREAKS;
    int line = 0, len = 0;
    bool lineOk = true;
    while ((len = io->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, LINE_BREAKS, IOAdapter::Term_Include, &lineOk)) > 0 && !ti.cancelFlag) {
        line++;
        if (!lineOk) {
            ti.setError(  tr("Line is too long: %1").arg(line) );
            return res;
        }
        if (len < 2) {
            continue;
        }
        if (buff[0] == '/' && buff[1] == '/') {
            if (!currentData->id.isEmpty()) {
                res.append(currentData);
                currentData = new EnzymeData();
            } else {
                ioLog.trace(QString("Enzyme without ID, line %1, skipping").arg(line));
            }
            continue;
        }
        if (len < 4) {
            continue;
        }
        if (buff[0] == 'I' && buff[1] == 'D') {
            currentData->id = QByteArray(buff + 3, len - 3).trimmed();
            continue;
        }
        if (buff[0] == 'E' && buff[1] == 'T') {
            currentData->type = QByteArray(buff + 3, len - 3).trimmed();
            continue;
        }
        if (buff[0] == 'O' && buff[1] == 'S') {
            currentData->organizm = QByteArray(buff + 3, len - 3).trimmed();
            continue;
        }
        if (buff[0] == 'A' && buff[1] == 'C') {
            currentData->accession = QByteArray(buff + 3, len - 3).trimmed();
            int sz = currentData->accession.size();
            if (sz > 0 && currentData->accession[sz-1]==';') {
                currentData->accession.resize(sz-1);
            }
            continue;
        }
        if (buff[0] == 'R' && buff[1] == 'S') {
            QByteArray rsLine = QByteArray(buff + 3, len - 3);
            QList<QByteArray> strands = rsLine.split(';');
            for (int s = 0, n = qMin(2, strands.count()); s < n; s++) {
                const QByteArray& strandInfo = strands.at(s);
                QByteArray seq;
                int cutPos = ENZYME_CUT_UNKNOWN;
                int cutIdx = strandInfo.indexOf(',');
                if (cutIdx > 0) {
                    seq = strandInfo.left(cutIdx).trimmed();
                    QByteArray cutStr = strandInfo.mid(cutIdx+1).trimmed();
                    bool ok = true;
                    cutPos = (cutStr.length() == 1 && cutStr[0] == '?') ? ENZYME_CUT_UNKNOWN : cutStr.toInt(&ok);
                    if (!ok) {
                        ioLog.error(tr("Restriction enzymes: Illegal cut pos: %1, line %2").arg(QString(cutStr)).arg(line));
                        break;
                    }
                } else {
                    seq = strandInfo.trimmed();
                }
                if (s == 0) {
                    currentData->seq= seq;
                    currentData->cutDirect = cutPos;
                    currentData->cutComplement = cutPos;
                } else if (cutPos != ENZYME_CUT_UNKNOWN) {
                    currentData->cutComplement = cutPos;
                }
            }
        }
    }
    io->close();
    return res;
}

void EnzymesIO::writeBairochFile(const QString& url, IOAdapterFactory* iof, const QString& source, IOAdapterFactory* srciof, const QSet<QString>& enzymes, TaskStateInfo& ti) {

    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Write)) {
        ti.setError(L10N::errorOpeningFileWrite(url));
        return;
    }

    QScopedPointer<IOAdapter> srcio(srciof->createIOAdapter());
    if (!srcio->open(source, IOAdapterMode_Read)) {
        ti.setError(L10N::errorOpeningFileRead(source));
        return;
    }

    QByteArray buffArr(DocumentFormat::READ_BUFF_SIZE, 0);
    char* buff = buffArr.data();
    const QBitArray& LINE_BREAKS = TextUtils::LINE_BREAKS;
    int line = 0, len = 0;
    bool lineOk = true;
    bool writeString = true;

    while ((len = srcio->readUntil(buff, DocumentFormat::READ_BUFF_SIZE, LINE_BREAKS, IOAdapter::Term_Include, &lineOk)) > 0 && !ti.cancelFlag) {
        line++;
        if (!lineOk) {
            ti.setError(  tr("Line is too long: %1").arg(line) );
        }
        if (len < 2) {
            if (writeString) io->writeBlock(buff, len);
            continue;
        }

        if (buff[0] == 'I' && buff[1] == 'D') {
            QString currID(QByteArray(buff + 3, len - 3).trimmed());
            writeString = enzymes.contains(currID);
        }
        if (writeString) io->writeBlock(buff, len);
    }
    srcio->close();
    io->close();
}

SEnzymeData EnzymesIO::findEnzymeById(const QString& id, const QList<SEnzymeData>& enzymes) {
    const QString idLower = id.toLower();
    foreach(const SEnzymeData& ed, enzymes) {
        if (ed->id.toLower() == idLower) {
            return ed;
        }
    }
    return SEnzymeData();
}

static QList<SEnzymeData> loadEnzymesData() {
    QList<SEnzymeData> res;
    TaskStateInfo ti;

    QString url = AppContext::getSettings()->getValue(EnzymeSettings::DATA_FILE_KEY).toString();

    if (url.isEmpty()) {
        QString dataDir = QDir::searchPaths( PATH_PREFIX_DATA ).first() + "/enzymes/";
        url = dataDir + DEFAULT_ENZYMES_FILE;
    }

    if (QFileInfo(url).exists()) {
        res = EnzymesIO::readEnzymes(url, ti);
    }

    return res;
}


QList<SEnzymeData> EnzymesIO::getDefaultEnzymesList()
{
    QList<SEnzymeData> data = loadEnzymesData();
    return data;
}

//////////////////////////////////////////////////////////////////////////
// load task

LoadEnzymeFileTask::LoadEnzymeFileTask(const QString& _url)
: Task(tr("Load enzymes from %1").arg(_url), TaskFlag_None), url(_url)
{
}

void LoadEnzymeFileTask::run() {
    enzymes = EnzymesIO::readEnzymes(url, stateInfo);
}

//////////////////////////////////////////////////////////////////////////
// save task

SaveEnzymeFileTask::SaveEnzymeFileTask(const QString& _url, const QString& _source, const QSet<QString>& _enzymes)
: Task(tr("Save enzymes to %1").arg(_url), TaskFlag_None), url(_url), source(_source), enzymes(_enzymes)
{
}

void SaveEnzymeFileTask::run() {
    EnzymesIO::writeEnzymes(url, source, enzymes, stateInfo);
}

}//namespace
