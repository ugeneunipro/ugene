/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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
#include <U2Core/U2Type.h>

#include "SequenceWalkerTask.h"

namespace U2 {

class DNATranslation;
class SequenceDbiWalkerSubtask;

class U2CORE_EXPORT SequenceDbiWalkerConfig : public SequenceWalkerConfig {
public:
    U2EntityRef seqRef;
    /** If true, not 3 but only the first frame is translated. Same for complement & direct modes. */
    bool translateOnlyFirstFrame = false;
};

class U2CORE_EXPORT SequenceDbiWalkerCallback {
public:
    virtual ~SequenceDbiWalkerCallback() {
    }

    virtual void onRegion(SequenceDbiWalkerSubtask* t, TaskStateInfo& ti) = 0;

    /* implement this to give SequenceDbiWalkerSubtask required resources
     * here are resources for ONE(!) SequenceDbiWalkerSubtask execution e.g. for one execution of onRegion function
     */
    virtual QList<TaskResourceUsage> getResources(SequenceDbiWalkerSubtask*) {
        return QList<TaskResourceUsage>();
    }
};

class U2CORE_EXPORT SequenceDbiWalkerTask : public Task {
    Q_OBJECT
public:
    SequenceDbiWalkerTask(const SequenceDbiWalkerConfig& config, SequenceDbiWalkerCallback* callback, const QString& name, TaskFlags tf = TaskFlags_NR_FOSE_COSC);

    SequenceDbiWalkerCallback* getCallback() const {
        return callback;
    }
    const SequenceDbiWalkerConfig& getConfig() const {
        return config;
    }

    // reverseMode - start splitting from the end of the range
    static QVector<U2Region> splitRange(const U2Region& range, int chunkSize, int overlapSize, int lastChunkExtraLen, bool reverseMode);

    void setError(const QString& err) {
        stateInfo.setError(err);
    }

private:
    QList<SequenceDbiWalkerSubtask*> prepareSubtasks();
    QList<SequenceDbiWalkerSubtask*> createSubs(const QVector<U2Region>& chunks, bool doCompl, bool doAmino);

    SequenceDbiWalkerConfig config;
    SequenceDbiWalkerCallback* callback;
};

class U2CORE_EXPORT SequenceDbiWalkerSubtask : public Task {
    Q_OBJECT
public:
    SequenceDbiWalkerSubtask(SequenceDbiWalkerTask* _t, const U2Region& glob, bool lo, bool ro, const U2EntityRef& seqRef, bool _doCompl, bool _doAmino);

    void run() override;

    /** Returns region sequence with all transformation applied. Computes the region sequence during the first call. */
    const QByteArray& getRegionSequence();

    /** Returns global coordinates of the sequence region processed by this sub-task. */
    const U2Region& getGlobalRegion() const;

    /** Returns top-level sequence walker task. */
    SequenceDbiWalkerTask* getSequenceDbiWalkerTask() const;

private:
    /** Prepares region sequence data if needed. */
    void prepareRegionSequence();

    SequenceDbiWalkerTask* t;
    U2Region globalRegion;
    U2EntityRef seqRef;
    bool doCompl;
    bool doAmino;
    bool leftOverlap;
    bool rightOverlap;

    /** Result region sequence with all needed transformations applied. */
    QByteArray regionSequence;

    /** Used for lazy initialization of the local sequence. */
    bool isRegionSequencePrepared = false;
};

}  // namespace U2
