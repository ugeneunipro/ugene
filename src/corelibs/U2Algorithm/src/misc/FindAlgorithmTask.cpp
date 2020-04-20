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

#include <QTextStream>

#include <U2Core/TextUtils.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/Counter.h>
#include <U2Core/AppResources.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/AppContext.h>
#include <U2Core/DocumentUtils.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/IOAdapterUtils.h>

#include "FindAlgorithmTask.h"

namespace U2 {

class StrandContext;

FindAlgorithmTask::FindAlgorithmTask(const FindAlgorithmTaskSettings& s)
: Task (tr("Find in sequence task"), TaskFlag_None), config(s)
{
    if(s.countTask){
        GCOUNTER(cvar, tvar, "FindAlgorithmTask");
    }
    tpm = Progress_Manual;
    complementRun = false;
    assert(config.strand == FindAlgorithmStrand_Direct || config.complementTT!=NULL);

    addTaskResource(TaskResourceUsage(RESOURCE_MEMORY,
        FindAlgorithm::estimateRamUsageInMbytes(config.patternSettings, NULL != config.proteinTT,
        config.pattern.length(), config.maxErr), true));
}

void FindAlgorithmTask::run() {
    FindAlgorithm::find(dynamic_cast<FindAlgorithmResultsListener*>(this),
        config.proteinTT,
        config.complementTT,
        config.strand,
        config.patternSettings,
        config.useAmbiguousBases,
        config.sequence.constData(),
        config.sequence.size(),
        config.sequenceAlphabet,
        config.searchIsCircular,
        config.searchRegion,
        config.pattern.constData(),
        config.pattern.length(),
        config.maxErr,
        config.maxRegExpResultLength,
        stateInfo.cancelFlag,
        stateInfo.progress);
}

void FindAlgorithmTask::onResult(const FindAlgorithmResult& r) {
    if (r.err == FindAlgorithmResult::NOT_ENOUGH_MEMORY_ERROR) {
        stateInfo.cancelFlag = true;
        const QString error = "Pattern is too big. Not enough memory.";
        taskLog.error(error);
        return;
    }
    if(config.maxResult2Find != FindAlgorithmSettings::MAX_RESULT_TO_FIND_UNLIMITED && newResults.size() >= config.maxResult2Find){
        stateInfo.cancelFlag = true;
        return;
    }
    if(stateInfo.isCoR()){
        return;
    }
    lock.lock();
    newResults.append(r);
    lock.unlock();
}

QList<FindAlgorithmResult> FindAlgorithmTask::popResults() {
    lock.lock();
    QList<FindAlgorithmResult> res = newResults;
    newResults.clear();
    lock.unlock();
    return res;
}

//////////////////////////////////////////////////////////////////////////
//LoadPatternsFileTask
LoadPatternsFileTask::LoadPatternsFileTask( const QString& _filePath, const QString &_annotationName)
    : Task(tr("Load pattern from file"), TaskFlag_None), filePath(_filePath), isRawSequence(false), annotationName(_annotationName)
{

}

Document * LoadPatternsFileTask::getDocumentFromFilePath()
{
    // loading new document here and not reusing any loaded from the project
    // because a document in the project may be opened in 'merge' mode.

    QList<FormatDetectionResult> formats = DocumentUtils::detectFormat(filePath);
    if (formats.isEmpty()) {
        stateInfo.setError(tr("Detecting format error for file %1").arg(filePath));
        return NULL;
    }

    DocumentFormat *format = formats.first().format;
    if(format->getFormatId() == BaseDocumentFormats::RAW_DNA_SEQUENCE){
        isRawSequence = true;
        return NULL;
    }
    Q_ASSERT(format);

    GUrl fileUrl(filePath);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(fileUrl));

    QVariantMap hints;
    Document* doc = format->loadDocument(iof, fileUrl, hints, stateInfo);

    CHECK_OP(stateInfo, NULL);

    return doc;
}

void LoadPatternsFileTask::run()
{
    typedef QPair<QString, QString> NamePattern;

    Document *doc = getDocumentFromFilePath();
    if(NULL != doc && !isRawSequence) {
        const QList<GObject *> &objectsFromDoc = doc->findGObjectByType(GObjectTypes::SEQUENCE);

        foreach(GObject *object, objectsFromDoc) {
            U2SequenceObject *sequenceObject = qobject_cast<U2SequenceObject*>(object);
            assert(NULL != sequenceObject);
            QByteArray sequence = sequenceObject->getWholeSequenceData(stateInfo);
            CHECK_OP(stateInfo, );
            QString seqName = sequenceObject->getSequenceName();
            if(annotationName.isEmpty()){
                namesPatterns.append(qMakePair(seqName, QString(sequence)));
            }else{
                namesPatterns.append(qMakePair(annotationName, QString(sequence)));
            }
        }
    } else {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
            setError(QString("Cannot open a file: %1").arg(filePath));

        QTextStream stream(&file);
        int fileSize = file.size();

        while(!stream.atEnd() && !stateInfo.cancelFlag) {
            int streamPos = stream.device()->pos();
            stateInfo.progress = 100 * streamPos / fileSize;
            QString pattern = stream.readLine();
            if (!pattern.isEmpty()){
                bool contains = false;
                foreach(const NamePattern& namePattern, namesPatterns){
                    if (namePattern.second == pattern){
                        contains = true;
                        break;
                    }
                }
                if (!contains){
                    namesPatterns.append(qMakePair("pattern" + QString::number(namesPatterns.size() + 1), pattern));
                }

            }
        }
        file.close();
    }
}

} //namespace



