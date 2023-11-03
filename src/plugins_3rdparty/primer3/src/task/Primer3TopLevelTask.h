/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#pragma once

#include <U2Core/Task.h>

#include <QSharedPointer>

namespace U2 {

class AnnotationTableObject;
class CheckComplementTask;
class CreateAnnotationsTask;
class Document;
class FindExonRegionsTask;
class ImportSequenceFromRawDataTask;
class Primer3TaskSettings;
class Primer3Task;
class ProcessPrimer3ResultsToAnnotationsTask;
class SaveDocumentTask;
class U2SequenceObject;

/**
 * This class represents a top-level Primer3 task - the one should be send to TaskScheduler.
 */
class Primer3TopLevelTask : public Task {
    Q_OBJECT
public:
    /**
     * This constructor should be used, when you have a sequence already opened in UGENE.
     */
    Primer3TopLevelTask(const QSharedPointer<Primer3TaskSettings>& settings,
                        U2SequenceObject* seqObj,
                        AnnotationTableObject* aobj,
                        const QString& groupName,
                        const QString& annName,
                        const QString& annDescription);
    /**
     * This constructor should be used, when you have no opened sequences and want to process
     * Primer3 only having primers.
     */
    Primer3TopLevelTask(const QSharedPointer<Primer3TaskSettings>& settings,
                        const QString& resultFilePath,
                        bool openView = true);

    ~Primer3TopLevelTask();

    void prepare() override;
    QList<Task*> onSubTaskFinished(Task* subTask) override;
    QString generateReport() const override;

    QPointer<AnnotationTableObject> getAnnotationTableObject() const;

private:
    Task* onFindExonTaskFinished();
    Task* onPrimer3TaskFinished();
    Task* onCheckComplementTaskFinished();
    Task* onProcessPrimer3ResultsToAnnotationsTaskFinished();
    Task* onImportSequenceFromRawDataTaskFinished();
    Task* onCreateAnnotationsTaskFinished();
    Task* onSaveDocumentTaskFinished();

    ProcessPrimer3ResultsToAnnotationsTask* createProcessPrimer3ResultsToAnnotationsTaskPrimer() const;

    QSharedPointer<Primer3TaskSettings> settings;

    // Parameters if target sequence is set
    QPointer<U2SequenceObject> seqObj;
    QPointer<AnnotationTableObject> annotationTableObject;
    QString groupName;
    QString annName;
    QString annDescription;

    // Parameter if no target sequence
    QString resultFilePath;
    bool openView = false;

    // If we run Primer 3 without target sequence, we need to create sequence with result primers.
    // This is the document, which stores sequence and annotation object of this file.
    // We create this document and pass it to the Project (so we do not need to delete it,
    // it will be deleted when user closes Project/closes UGENE/removes document from the Project).
    // BUT!!! If we created document, but did not passed it to the Project
    // (e.g. some error appeared or task was canceled) we need to delete it on our own.
    QPointer<Document> document;

    // Child tasks
    FindExonRegionsTask* findExonsTask = nullptr;
    Primer3Task* primer3Task = nullptr;
    CheckComplementTask* checkComplementTask = nullptr;
    ImportSequenceFromRawDataTask* importSequenceFromRawDataTask = nullptr;
    ProcessPrimer3ResultsToAnnotationsTask* processPrimer3ResultsToAnnotationsTask = nullptr;
    CreateAnnotationsTask* createAnnotationsTask = nullptr;
    SaveDocumentTask* saveDocumentTask = nullptr;
};


}
