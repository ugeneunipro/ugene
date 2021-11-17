/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#ifndef _U2_EXTRACT_PRIMER_TASK_H_
#define _U2_EXTRACT_PRIMER_TASK_H_

#include <U2Core/DNASequence.h>
#include <U2Core/Task.h>
#include <U2Core/U2Region.h>
#include <U2Core/U2Type.h>

#include <src/PCRPrimerDesignForDNAAssemblyTaskSettings.h>

namespace U2 {

class Document;
class LoadDocumentTask;

struct ExtractPrimerTaskSettings {
    U2EntityRef sequenceRef;
    U2Region fragmentLocation;
    QString fragmentName;
    QString originalSequenceFileName;
    QString outputFileUrl;
    QByteArray backboneSequence;
    PCRPrimerDesignForDNAAssemblyTaskSettings::BackboneBearings direction = PCRPrimerDesignForDNAAssemblyTaskSettings::BackboneBearings::Backbone5;
};

class ExtractPrimerTask : public Task {
    Q_OBJECT
public:
    ExtractPrimerTask(const ExtractPrimerTaskSettings &settings);
    ~ExtractPrimerTask();

    void run() override;
    /* Moves the document to the main thread */
    Document *takeResult();

private:
    DNASequence getProductSequence();
    QString getProductName() const;

    ExtractPrimerTaskSettings settings;
    Document *result = nullptr;
    LoadDocumentTask *loadTask = nullptr;

    const static QString BACKBONE_ANNOTATION_NAME;
    const static QString RESULT_ANNOTATION_GROUP_NAME;
};

/*
* Wrapper for GUI usage.
*/

class ExtractPrimerAndOpenDocumentTask : public Task {
    Q_OBJECT
public:
    ExtractPrimerAndOpenDocumentTask(const ExtractPrimerTaskSettings &settings);

    // Task
    void prepare() override;
    QList<Task *> onSubTaskFinished(Task *subTask);
    ReportResult report();

private:
    void prepareUrl();

    ExtractPrimerTaskSettings settings;
    ExtractPrimerTask *extractTask = nullptr;
};

}

#endif
