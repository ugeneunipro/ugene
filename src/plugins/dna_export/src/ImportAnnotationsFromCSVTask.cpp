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

#include <QScopedPointer>

#include <QScriptEngine>
#include <QScriptValueIterator>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/AddDocumentTask.h>
#include <U2Core/LoadDocumentTask.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TextUtils.h>
#include <U2Core/ScriptTask.h>

#include <U2Gui/ObjectViewModel.h>

#include <U2View/AnnotatedDNAView.h>

#include "ImportAnnotationsFromCSVTask.h"

namespace U2 {

QBitArray CSVParsingConfig::QUOTES = TextUtils::createBitMap("\'\"");

ImportAnnotationsFromCSVTask::ImportAnnotationsFromCSVTask(ImportAnnotationsFromCSVTaskConfig& _config)
: Task(tr("Import annotations from CSV"), TaskFlags_NR_FOSCOE),
config(_config), readTask(NULL), writeTask(NULL), addTask(NULL)
{
    GCOUNTER(cvar,tvar,"ImportAnnotationsFromCSVTask");
    readTask = new ReadCSVAsAnnotationsTask(config.csvFile, config.parsingOptions);
    addSubTask(readTask);
}

static void adjustRelations(AnnotationTableObject *ao) {
    if (!ao->findRelatedObjectsByType(GObjectTypes::SEQUENCE).isEmpty()) {
        return; //nothing to adjust -> already has relation
    }

    // try automatically associate annotations doc with active sequence view
    GObjectViewWindow* activeViewWindow = GObjectViewUtils::getActiveObjectViewWindow();
    if (activeViewWindow == NULL) {
        return;
    }
    AnnotatedDNAView* seqView = qobject_cast<AnnotatedDNAView*>(activeViewWindow->getObjectView());
    if (seqView == NULL) {
        return;
    }

    foreach(U2SequenceObject *seqObj, seqView->getSequenceObjectsWithContexts()){
        U2Region seqRegion(0, seqObj->getSequenceLength());
        bool outOfRange = false;
        foreach (Annotation *ann, ao->getAnnotations()) {
            const QVector<U2Region> &locations = ann->getRegions();
            if (!seqRegion.contains(locations.last())) {
                outOfRange = true;
                break;
            }
        }
        if (!outOfRange) {
            ao->addObjectRelation(seqObj, ObjectRole_Sequence);
            seqView->addObject(ao);
            break;
        } else {
            algoLog.trace(QString("Annotation is out of the sequence range %1").arg(seqObj->getGObjectName()));
        }
    }
}


QList<Task *> ImportAnnotationsFromCSVTask::onSubTaskFinished(Task* subTask) {
    QList<Task *> result;
    if (hasError() || subTask == addTask) {
        return result;
    }

    GUrl docUrl(config.dstFile);
    Document* projDoc =  AppContext::getProject()->findDocumentByURL(docUrl);
    bool inProject = projDoc!=NULL;

    if (doc.isNull() && projDoc != NULL) {
        doc = projDoc;
    }
    if (doc.isNull()) { //document is null -> save it and add to the project
        assert(subTask == readTask);
        doc = prepareNewDocument(prepareAnnotations());
        writeTask = new SaveDocumentTask(doc);
        result.append(writeTask);
    } else if (writeTask != NULL && !inProject) { // document was saved -> add to the project
        addTask = new AddDocumentTask(doc);
        result.append(addTask);
    } else { //document already in the project -> check loaded state and add annotations to it
        assert(inProject);
        if (!doc->isLoaded()) {
            result.append(new LoadUnloadedDocumentTask(doc));
        } else {
            DocumentFormatConstraints dfc;
            dfc.flagsToSupport = DocumentFormatFlag_SupportWriting;
            dfc.supportedObjectTypes += GObjectTypes::ANNOTATION_TABLE;
            if (!doc->getDocumentFormat()->checkConstraints(dfc)) {
                setError(tr("Annotations can't be added to the document %1").arg(doc->getURLString()));
                return result;
            }
            if (doc->isStateLocked()) {
                setError(tr("Document is locked and can't be modified %1").arg(doc->getURLString()));
                return result;
            }
            QList<GObject*> objs = doc->findGObjectByType(GObjectTypes::ANNOTATION_TABLE);
            AnnotationTableObject *ao = objs.isEmpty() ? NULL : qobject_cast<AnnotationTableObject *>(objs.first());
            if (ao == NULL) {
                ao = new AnnotationTableObject("Annotations", doc->getDbiRef());
                adjustRelations(ao);
            }
            SAFE_POINT(ao != NULL, "Invalid annotation table", result );
            QMap<QString, QList<SharedAnnotationData> > groups = prepareAnnotations();
            foreach (const QString &groupName, groups.keys()) {
                ao->addAnnotations(groups[groupName], groupName);
            }
        }
    }
    return result;
}

QMap<QString, QList<SharedAnnotationData> > ImportAnnotationsFromCSVTask::prepareAnnotations() const {
    QMap<QString, QList<SharedAnnotationData> > result;

    SAFE_POINT(readTask != NULL && readTask->isFinished(), "Invalid read annotations task!", result);
    QMap<QString, QList<SharedAnnotationData> > datas = readTask->getResult();
    foreach (const QString &groupName, datas.keys()) {
        foreach (const SharedAnnotationData &d, datas[groupName]) {
            result[groupName] << d;
        }
    }
    return result;
}

Document * ImportAnnotationsFromCSVTask::prepareNewDocument(const QMap<QString, QList<SharedAnnotationData> > &groups) {
    DocumentFormat *format = AppContext::getDocumentFormatRegistry()->getFormatById(config.formatId);
    CHECK(NULL != format, NULL);

    IOAdapterId ioId = IOAdapterUtils::url2io(config.dstFile);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioId);

    U2OpStatus2Log os;
    Document *result = format->createNewLoadedDocument(iof, config.dstFile, os);
    CHECK_OP(os, NULL);

    AnnotationTableObject* ao = new AnnotationTableObject("Annotations", result->getDbiRef());
    foreach (const QString &groupName, groups.keys()) {
        ao->addAnnotations(groups[groupName], groupName);
    }
    ao->setModified(false);
    result->addObject(ao);

    adjustRelations(ao);

    return result;
}

//////////////////////////////////////////////////////////////////////////
// ReadCSVAsAnnotationsTask

ReadCSVAsAnnotationsTask::ReadCSVAsAnnotationsTask(const QString& _file, const CSVParsingConfig& _config)
: Task(tr("Parse CSV file %1").arg(_file), TaskFlag_None), file(_file), config(_config)
{
}

#define BUFF_SIZE 8192
void ReadCSVAsAnnotationsTask::run() {
    GUrl url(file);
    IOAdapterId ioId = IOAdapterUtils::url2io(url);
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(ioId);
    QScopedPointer<IOAdapter> io(iof->createIOAdapter());

    if (!io->open(url, IOAdapterMode_Read)) {
        setError(L10N::errorOpeningFileRead(url));
        return;
    }

    QByteArray block(BUFF_SIZE, '\0');
    int blockLen = 0;
    QString text;
    while ((blockLen = io->readBlock(block.data(), BUFF_SIZE)) > 0) {
        int sizeBefore = text.length();
        QString line = QString::fromLocal8Bit(block.data(), blockLen);
        text.append(line);
        if (text.length() != sizeBefore + line.length()) {
            setError(L10N::errorReadingFile(url));
        }
        stateInfo.progress = io->getProgress();
    }
    int maxColumns = 0;
    QList<QStringList> parsedLines = parseLinesIntoTokens(text, config, maxColumns, stateInfo);

    foreach (const QStringList &lineTokens, parsedLines) {
        SharedAnnotationData a(new AnnotationData);
        bool ok = true;
        QString error;
        int startPos = -1;
        int startPosOffset = 0;
        int len = -1;
        int endPos = -1;
        QString groupName;

        for (int column = 0; column < lineTokens.size() && ok; column++) {
            if (column >= config.columns.size()) {
                break;
            }
            const ColumnConfig& columnConf = config.columns.at(column);
            const QString& token = lineTokens.at(column);
            switch(columnConf.role) {
                case ColumnRole_Qualifier:
                    assert(!columnConf.qualifierName.isEmpty());
                    a->qualifiers.append(U2Qualifier(columnConf.qualifierName, token));
                    break;
                case ColumnRole_Name:
                    a->name = token.isEmpty() ? config.defaultAnnotationName : token;
                    ok = Annotation::isValidAnnotationName(a->name);
                    if (!ok) {
                        error = tr("Invalid annotation name: '%1'").arg(a->name);
                    }
                    break;
                case ColumnRole_StartPos:
                    assert(startPos == -1);
                    startPos = token.toInt(&ok) - 1;
                    startPosOffset = columnConf.startPositionOffset;
                    if (!ok) {
                        error = tr("Start offset is not numeric: '%1'").arg(token);
                    }
                    break;
                case ColumnRole_EndPos:
                    assert(endPos == -1);
                    endPos = token.toInt(&ok) + (columnConf.endPositionIsInclusive ? 1 : 0) - 1;
                    if (!ok) {
                        error = tr("End offset is not numeric: '%1'").arg(token);
                    }
                    break;
                case ColumnRole_Length:
                    assert(len == -1);
                    len = token.toInt(&ok);
                    if (!ok) {
                        error = tr("Length is not numeric: '%1'").arg(token);
                    }
                    break;
                case ColumnRole_ComplMark:
                    a->location->strand = (columnConf.complementMark.isEmpty() || token == columnConf.complementMark)
                        ? U2Strand::Complementary : U2Strand::Direct;
                    break;
                case ColumnRole_Group:
                    groupName = token;
                    break;
                default:
                    assert(columnConf.role == ColumnRole_Ignore);
            }
        }

        //add annotation
        if (ok) {
            //set up default name
            if (a->name.isEmpty()) {
                a->name = config.defaultAnnotationName;
            }
            //set up location
            U2Region location;
            if (startPos != -1) {
                location.startPos = startPos + startPosOffset;
                if (endPos != -1) {
                    location.length = endPos - startPos;
                } else {
                    location.length = len;
                }
            } else {
                location.length = len;
                location.startPos = endPos - len;
            }
            if (location.length < 0) {
                location.startPos = location.startPos + location.length;
                location.length = - location.length;
            }
            if (location.startPos < 0 || location.startPos > location.endPos()) {
                algoLog.details(tr("Invalid location: start: %1  len: %2, in line :%3, ignoring")
                    .arg(QString::number(location.startPos)).arg(QString::number(location.length)).arg(lineTokens.join(config.splitToken)));
            } else {
                a->location->regions.append(location);
                result[groupName] << a;
            }
        } else {
            //TODO: make configurable to allow stop parsing on any error!
            algoLog.details(tr("Can't parse line: '%1', error = %2, ignoring").arg(lineTokens.join(config.splitToken)).arg(error));
        }
    }
}

static QStringList removeQuotes(const QStringList& tokens) {
    QStringList result;
    foreach(const QString& token, tokens) {
        if (token.length() < 2) {
            result.append(token);
            continue;
        }
        char c1 = token.at(0).toLatin1();
        char c2 = token.at(token.length()-1).toLatin1();
        if (c1!=c2 || !CSVParsingConfig::QUOTES.testBit(quint8(c1))) {
            result.append(token);
            continue;
        }
        QString unquoted = token.mid(1, token.length()-2);
        result.append(unquoted);
    }
    return result;
}

QList<QStringList> ReadCSVAsAnnotationsTask::parseLinesIntoTokens(const QString& text, const CSVParsingConfig& config, int& maxColumns, TaskStateInfo& ti) {
    QList<QStringList> result;
    assert(!config.splitToken.isEmpty() || !config.parsingScript.isEmpty());
    maxColumns = 0;
    QStringList lines = text.split('\n', QString::SkipEmptyParts);
    int lineNum = 1;
    for (int l = 0; l < lines.size(); l++) {
        if (l < config.linesToSkip) {
            continue;
        }
        QString line = lines.at(l).trimmed();
        if (!config.prefixToSkip.isEmpty() && line.startsWith(config.prefixToSkip)) {
            continue;
        }

        QStringList tokens = parseLineIntoTokens(line, config, ti, lineNum);
        if (config.removeQuotes) {
            tokens = removeQuotes(tokens);
        }
        lineNum++;
        maxColumns = qMax(maxColumns, tokens.size());
        result.append(tokens);
    }
    return result;
}

QString ReadCSVAsAnnotationsTask::LINE_VAR("line");
QString ReadCSVAsAnnotationsTask::LINE_NUM_VAR("lineNum");

QStringList ReadCSVAsAnnotationsTask::parseLineIntoTokens(const QString& line, const CSVParsingConfig& config, TaskStateInfo& ti, int lineNum) {
    QStringList result;
    if (config.parsingScript.isEmpty()) {
        result = line.split(config.splitToken, config.keepEmptyParts ? QString::KeepEmptyParts: QString::SkipEmptyParts);
        return result;
    }
    //run script
    QMap<QString, QScriptValue> vars;
    QScriptEngine engine;
    vars[LINE_VAR] = QScriptValue(&engine, line);
    vars[LINE_NUM_VAR] = QScriptValue(&engine, lineNum);
    QScriptValue scriptResult = ScriptTask::runScript(&engine, vars, config.parsingScript, ti);
    if (ti.cancelFlag || ti.hasError()) {
        return result;
    }
    if (scriptResult.isString()) {
        result.append(scriptResult.toString());
    } else if (scriptResult.isArray()) {
        QScriptValueIterator it(scriptResult);
        while (it.hasNext()) {
            it.next();
            if (it.flags() & QScriptValue::SkipInEnumeration)
                continue;
            QScriptValue val = it.value();
            QString strVal = val.toString();
            result.append(strVal);
        }
    } else {
        ti.setError(tr("Script result is not an array of strings!"));
    }
    return result;
}

class CharStat {
public:
    CharStat() : ch(0), count(0) {}
    char ch;
    int  count;
};

static QVector<CharStat> countFreqs(const QString& line) {
    QVector<CharStat> result(256);
    QByteArray ba = line.toLocal8Bit();
    const char* data = ba.constData();
    char prevChar = 0;
    for (int i = 0, n = ba.length(); i < n; i++) {
        char c = data[i];

        if (c == prevChar && (c == ' ' || c == '\t')) { //do not count repeating ws
            continue;
        }
        result[uchar(c)].ch = c;
        result[uchar(c)].count++;
        prevChar = c;
    }
    return result;
}

static void mergeFreqs(QVector<CharStat>& globalFreqs, const QVector<CharStat>& localFreqs) {
    assert(globalFreqs.size() == localFreqs.size());
    for(int i = 0, n = globalFreqs.size(); i < n; i++) {
        if (globalFreqs.at(i).count != localFreqs.at(i).count) {
            globalFreqs[i].count = 0;
        }
    }
}

QString ReadCSVAsAnnotationsTask::guessSeparatorString(const QString& text, const CSVParsingConfig& config) {
    QVector<CharStat> globalFreqs;
    QStringList lines = text.split('\n', QString::SkipEmptyParts);
    for (int l = 0; l < lines.size(); l++) {
        if (l < config.linesToSkip) {
            continue;
        }
        QString line = lines.at(l).trimmed();
        QVector<CharStat> lineFreqs = countFreqs(line);
        if (globalFreqs.isEmpty()) {
            globalFreqs = lineFreqs;
            continue;
        }
        if (!config.prefixToSkip.isEmpty() && line.startsWith(config.prefixToSkip)) {
            continue;
        }
        mergeFreqs(globalFreqs, lineFreqs);
    }
    CharStat max;
    float maxWeight = 0;
    static QString doubleWeightChars = ",;: \t"; // chars that are often used as separators
    static QString lowWeightChars= "\'\""; // quotes and other frequent chars that rare used as separators
    for (int i = 0; i < globalFreqs.size(); i++) {
        const CharStat& cs = globalFreqs.at(i);
        float csWeight = cs.count;
        if (doubleWeightChars.contains(cs.ch)) {
            csWeight = csWeight * 2;
        } else if (lowWeightChars.contains(cs.ch)) {
            csWeight = csWeight / 2;
        }
        if (csWeight > maxWeight) {
            max = cs;
            maxWeight = csWeight;
        }
    }
    if (max.count == 0) {
        return QString();
    }
    return QString(QChar(max.ch));
}

} //namespace
