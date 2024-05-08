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

#pragma once

#include <U2Core/MsaObject.h>
#include <U2Core/Task.h>
#include <U2Core/global.h>

namespace U2 {

class DNATranslation;

/**
 Performs in-place translation of multiple sequence alignment object
*/

class U2ALGORITHM_EXPORT TranslateMsa2AminoTask : public Task {
    Q_OBJECT
public:
    TranslateMsa2AminoTask(MsaObject* obj);
    TranslateMsa2AminoTask(MsaObject* obj, const QString& trId);
    const Msa& getTaskResult() {
        return resultMA;
    }
    void run() override;
    ReportResult report() override;

private:
    Msa resultMA;
    MsaObject* maObj;
    DNATranslation* translation;
};

/**
 Wrapper for multiple alignment task
*/

class U2ALGORITHM_EXPORT AlignGObjectTask : public Task {
    Q_OBJECT
public:
    AlignGObjectTask(const QString& taskName, TaskFlags f, MsaObject* maobj)
        : Task(taskName, f), obj(maobj) {
    }
    virtual void setMAObject(MsaObject* maobj) {
        obj = maobj;
    }
    MsaObject* getMAObject() {
        return obj;
    }

protected:
    QPointer<MsaObject> obj;
};

/**
 Multi task converts alignment object to amino representation if possible.
 This allows one to:
 1) speed up alignment
 2) avoid errors of inserting gaps within codon boundaries
*/

class U2ALGORITHM_EXPORT AlignInAminoFormTask : public Task {
    Q_OBJECT
    Q_DISABLE_COPY(AlignInAminoFormTask)
public:
    AlignInAminoFormTask(MsaObject* obj, AlignGObjectTask* alignTask, const QString& traslId);
    ~AlignInAminoFormTask();

    void prepare() override;
    void run() override;
    ReportResult report() override;

protected:
    AlignGObjectTask* alignTask;
    MsaObject *maObj, *clonedObj;
    QString traslId;
    Document* tmpDoc;
    QMap<qint64, QVector<U2MsaGap>> rowsGapModel;
    QMap<qint64, QVector<U2MsaGap>> emptyGapModel;
};

}  // namespace U2
