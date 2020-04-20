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

#ifndef _U2_IMPORT_ANNOTATIONS_FROM_CSV_TASK_H_
#define _U2_IMPORT_ANNOTATIONS_FROM_CSV_TASK_H_

#include <QMap>
#include <QPointer>

#include <U2Core/Task.h>
#include <U2Core/AnnotationData.h>

#include "CSVColumnConfiguration.h"

namespace U2 {

// FIXME: implement splitToken as a default value for parsing script (i.e line.split(<separator>))
class CSVParsingConfig {
public:
    CSVParsingConfig() : defaultAnnotationName("misc_feature"), linesToSkip(0), keepEmptyParts(true), removeQuotes(true){}
    QString             defaultAnnotationName;
    QString             splitToken;
    int                 linesToSkip;
    QString             prefixToSkip;
    bool                keepEmptyParts;
    QList<ColumnConfig> columns;
    QString             parsingScript;
    bool                removeQuotes;

    static QBitArray    QUOTES;
};

class ImportAnnotationsFromCSVTaskConfig {
public:
    ImportAnnotationsFromCSVTaskConfig() : addToProject(true) {}

    QString             csvFile;
    QString             dstFile;
    bool                addToProject;
    DocumentFormatId    formatId;

    CSVParsingConfig parsingOptions;
};

class ReadCSVAsAnnotationsTask;
class SaveDocumentTask;
class AddDocumentTask;
class Annotation;
class Document;

class ImportAnnotationsFromCSVTask: public Task {
    Q_OBJECT
public:
    ImportAnnotationsFromCSVTask(ImportAnnotationsFromCSVTaskConfig& config);

    QList<Task*> onSubTaskFinished(Task* subTask);

private:
    QMap<QString, QList<SharedAnnotationData> > prepareAnnotations() const;

    Document * prepareNewDocument(const QMap<QString, QList<SharedAnnotationData> > &annotations);

    ImportAnnotationsFromCSVTaskConfig  config;
    ReadCSVAsAnnotationsTask*           readTask;
    SaveDocumentTask*                   writeTask;
    AddDocumentTask*                    addTask;
    QPointer<Document>                  doc;
};

class ReadCSVAsAnnotationsTask : public Task {
Q_OBJECT
public:
    ReadCSVAsAnnotationsTask(const QString& file, const CSVParsingConfig& config);

    void run();

    QMap<QString, QList<SharedAnnotationData> > getResult() const { return result; }

    static QList<QStringList> parseLinesIntoTokens(const QString& text, const CSVParsingConfig& config, int& maxColumns, TaskStateInfo& ti);

    static QStringList parseLineIntoTokens(const QString& line, const CSVParsingConfig& config, TaskStateInfo& ti, int lineNum = 1);

    static QString guessSeparatorString(const QString& text, const CSVParsingConfig& config);

    // script variable that holds line value
    static QString LINE_VAR;
    // script variable that holds parsed line numbers. Lines that skipped to not increment this value
    static QString LINE_NUM_VAR;

private:
    QString                         file;
    CSVParsingConfig                config;
    // Group name <-> annotations
    QMap<QString, QList<SharedAnnotationData> > result;
};

} // namespace U2

#endif
