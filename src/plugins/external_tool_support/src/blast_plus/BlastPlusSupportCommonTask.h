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

#ifndef _U2_BLASTPLUS_SUPPORT_TASK_H
#define _U2_BLASTPLUS_SUPPORT_TASK_H

#include <U2Core/AnnotationData.h>
#include <U2Core/ExternalToolRunTask.h>

#include "utils/BlastTaskSettings.h"

class QDomNode;

namespace U2 {

class SaveDocumentTask;
class U2PseudoCircularization;

class BlastPlusSupportCommonTask : public ExternalToolSupportTask {
    Q_OBJECT
public:
    BlastPlusSupportCommonTask(const BlastTaskSettings& settings);
    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);

    Task::ReportResult report();
    virtual QString generateReport() const;

    QList<SharedAnnotationData> getResultedAnnotations() const;
    BlastTaskSettings           getSettings() const;

    virtual ExternalToolRunTask* createBlastPlusTask() = 0;

    static QString toolIdByProgram(const QString &program);

protected:
    BlastTaskSettings               settings;
    QString                         url;

private:
    SaveDocumentTask*               saveTemporaryDocumentTask;
    ExternalToolRunTask*            blastPlusTask;
    U2SequenceObject*               sequenceObject;
    Document*                       tmpDoc;
    QList<SharedAnnotationData>     result;
    U2PseudoCircularization*        circularization;

    void parseTabularResult();
    void parseTabularLine(const QByteArray &line);

    void parseXMLResult();
    void parseXMLHit(const QDomNode &xml);
    void parseXMLHsp(const QDomNode &xml,const QString &id, const QString &def, const QString &accession);
    QString getAcceptableTempDir() const;
};

class BlastPlusSupportMultiTask : public Task {
    Q_OBJECT
public:
    BlastPlusSupportMultiTask(QList<BlastTaskSettings>& settingsList, QString& url);
    void prepare();
    QList<Task*> onSubTaskFinished(Task* subTask);

    Task::ReportResult report();
    QString generateReport() const;
private:
    QList<BlastTaskSettings>    settingsList;
    Document*                   doc;
    QString                     url;
};

class BlastDbCmdSupportTask : public Task {

};


}//namespace
#endif // _U2_BLASTPLUS_SUPPORT_TASK_H
