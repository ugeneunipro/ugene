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

#ifndef _U2_IMPORT_QUALITY_SCORES_TASK_H_
#define _U2_IMPORT_QUALITY_SCORES_TASK_H_

#include <QMap>
#include <QPointer>

#include <U2Core/DNAQuality.h>
#include <U2Core/Task.h>

namespace U2 {

class U2SequenceObject;

class ImportQualityScoresConfig {
public:
    ImportQualityScoresConfig() : type(DNAQualityType_Sanger), format(DNAQuality::QUAL_FORMAT), createNewDocument(false) {}
    QString             fileName;
    DNAQualityType      type;
    DNAQualityFormat    format;
    bool                createNewDocument;
    QString             dstFileName;

};

class ReadQualityScoresTask : public Task {
    Q_OBJECT
public:
    ReadQualityScoresTask(const QString& fileName, DNAQualityType t, const DNAQualityFormat& f);

    void run();

    const QMap<QString,DNAQuality>& getResult() const { return result; }

private:
    void recordQuality( int headerCounter );
    bool checkRawData();
    QString                     fileName;
    DNAQualityType              type;
    DNAQualityFormat            format;
    QStringList                 headers;
    QList<int>                  values;
    QByteArray                  encodedQuality;
    QMap<QString, DNAQuality>   result;
};


class ImportPhredQualityScoresTask : public Task {
    Q_OBJECT
public:
    ImportPhredQualityScoresTask(const QList<U2SequenceObject*>& sequences, ImportQualityScoresConfig& config);

    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);

private:
    ReadQualityScoresTask* readQualitiesTask;
    ImportQualityScoresConfig config;
    QList<U2SequenceObject*> seqList;
};


} // namespace U2

#endif // _U2_IMPORT_QUALITY_SCORES_TASK_H_
