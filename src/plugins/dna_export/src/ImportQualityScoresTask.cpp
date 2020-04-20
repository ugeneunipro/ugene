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

#include <QFile>
#include <QTextStream>

#include <U2Core/Log.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/AppContext.h>
#include <U2Core/TextUtils.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/U2SafePoints.h>

#include "ImportQualityScoresTask.h"
#include <time.h>

namespace U2 {

ReadQualityScoresTask::ReadQualityScoresTask( const QString& file, DNAQualityType t, const DNAQualityFormat& f)
: Task("ReadPhredQuality", TaskFlag_None), fileName(file), type(t), format(f)
{

}

#define READ_BUF_SIZE 4096

void ReadQualityScoresTask::run() {

    if (!checkRawData()) {
        return;
    }

    IOAdapterFactory* f = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    QScopedPointer<IOAdapter> io( f->createIOAdapter() );

    if (!io->open(fileName, IOAdapterMode_Read) ) {
        stateInfo.setError("Can not open quality file");
        return;
    }

    int headerCounter = -1;
    QByteArray readBuf(READ_BUF_SIZE+1, 0);
    char* buf = readBuf.data();
    int lineCount = 0;

    while (!stateInfo.cancelFlag) {

        int len = io->readUntil(buf, READ_BUF_SIZE, TextUtils::LINE_BREAKS, IOAdapter::Term_Include);
        ++lineCount;
        stateInfo.progress = io->getProgress();

        if (len == 0) {
            recordQuality(headerCounter);
            break;
        }

        if (buf[0] == '>') {
            recordQuality(headerCounter);
            QByteArray header = readBuf.mid(1, len - 1).trimmed();
            headers.append(header);
            values.clear();
            ++headerCounter;
            continue;
        }

        QByteArray valsBuf = readBuf.mid(0, len).trimmed();
        if (format == DNAQuality::QUAL_FORMAT) {
            QList<QByteArray> valList = valsBuf.split(' ');
            foreach(const QByteArray& valStr, valList) {
                bool ok = false;
                if(!valStr.isEmpty()){
                    values.append( valStr.toInt(&ok) );
                    if (!ok) {
                        setError(tr("Failed parse quality value: file %1, line %2").arg(fileName).arg(lineCount));
                        return;
                    }
                }
            }
        } else {
            encodedQuality = valsBuf;
        }
    }

    io->close();

}


void ReadQualityScoresTask::recordQuality( int headerCounter )
{
    if (headerCounter != -1) {
        QByteArray qualCodes;
        if (format == DNAQuality::QUAL_FORMAT) {
            foreach (int v, values) {
                char code = DNAQuality::encode(v, type);
                qualCodes.append(code);
            }
        } else {
            qualCodes = encodedQuality;
        }
        result.insert(headers[headerCounter], DNAQuality(qualCodes,type));
        //log.trace( QString("Phred quality parsed: %1 %2").arg(headers[headerCounter]).arg(qualCodes.constData()) );
    }
}

#define RAW_BUF_SIZE 256

bool ReadQualityScoresTask::checkRawData()
{
    IOAdapterFactory* f = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(BaseIOAdapters::LOCAL_FILE);
    QScopedPointer<IOAdapter> io( f->createIOAdapter() );

    QByteArray buf;
    buf.reserve(RAW_BUF_SIZE);

    if (!io->open(fileName, IOAdapterMode_Read )) {
        setError(tr("Failed to open quality file %1").arg(fileName));
        return false;
    }
    int len =  io->readBlock(buf.data(), RAW_BUF_SIZE);
    if (len == 0 || len == -1) {
        setError(tr("Failed to read data from quality file %1, probably it is empty. %2").arg(fileName).arg(io->errorString()));
        return false;
    }
    if (buf[0] != '>') {
        setError(tr("File  %1 is not a quality file").arg(fileName));
        return false;
    }

    io->close();
    return true;

}


//////////////////////////////////////////////////////////////////////////


ImportPhredQualityScoresTask::ImportPhredQualityScoresTask(const QList<U2SequenceObject*>& sequences, ImportQualityScoresConfig& cfg )
: Task("ImportPhredQualityScores", TaskFlags_NR_FOSCOE), readQualitiesTask(NULL), config(cfg), seqList(sequences)
{

}

void ImportPhredQualityScoresTask::prepare() {
    readQualitiesTask = new ReadQualityScoresTask(config.fileName, config.type, config.format);
    addSubTask(readQualitiesTask);
}

QList<Task*> ImportPhredQualityScoresTask::onSubTaskFinished( Task* subTask ) {
    QList<Task*> subTasks;
    CHECK_OP(subTask->getStateInfo(), subTasks);

    if (subTask != readQualitiesTask) {
        return subTasks;
    }
    QMap<QString,DNAQuality> qualities = readQualitiesTask->getResult();
    if (config.createNewDocument) {
        assert(0);
        //TODO: consider creating this option
    } else {
        foreach (U2SequenceObject* obj, seqList) {
            if (obj->isStateLocked()) {
                setError(QString("Unable to modify sequence %1: object is locked.").arg(obj->getGObjectName()));
                continue;
            }
            QString seqName = obj->getGObjectName().split(' ').first();
            if (qualities.contains(seqName)) {
                obj->setQuality(qualities.value(seqName));
            } else {
                setError(QString("Quality scores for %1 are not found.").arg(seqName));
                break;
            }
        }
    }
    return subTasks;
}




} // namespace U2
