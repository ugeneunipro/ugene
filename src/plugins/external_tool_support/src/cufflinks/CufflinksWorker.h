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

#ifndef _U2_CUFFLINKS_WORKER_H
#define _U2_CUFFLINKS_WORKER_H

#include "CufflinksSettings.h"

#include <U2Lang/Descriptor.h>
#include <U2Lang/LocalDomain.h>
#include <U2Lang/WorkflowUtils.h>


namespace U2 {
namespace LocalWorkflow {


class CufflinksPrompter : public PrompterBase<CufflinksPrompter>
{
    Q_OBJECT

public:
    CufflinksPrompter(Actor* parent = 0);

protected:
    QString composeRichDoc();
};


class CufflinksWorker : public BaseWorker
{
    Q_OBJECT

public:
    CufflinksWorker(Actor* actor);

    void init();
    Task *tick();
    void cleanup();

private slots:
    void sl_cufflinksTaskFinished();

protected:
    IntegralBus* input;
    IntegralBus* output;
    CufflinksSettings settings;

    bool settingsAreCorrect;

private:
    void initSlotsState();
};


class CufflinksWorkerFactory : public DomainFactory
{
public:
    static const QString ACTOR_ID;
    static void init();
    CufflinksWorkerFactory() : DomainFactory(ACTOR_ID) {}
    virtual Worker* createWorker(Actor* actor) { return new CufflinksWorker(actor); }

    static const QString OUT_DIR;
    static const QString REF_ANNOTATION;
    static const QString RABT_ANNOTATION;
    static const QString LIBRARY_TYPE;
    static const QString MASK_FILE;
    static const QString MULTI_READ_CORRECT;
    static const QString MIN_ISOFORM_FRACTION;
    static const QString FRAG_BIAS_CORRECT;
    static const QString PRE_MRNA_FRACTION;
    static const QString EXT_TOOL_PATH;
    static const QString TMP_DIR_PATH;

    static const QString OUT_MAP_DESCR_ID;
    static const QString ISO_LEVEL_SLOT_DESCR_ID;
};

} // namespace LocalWorkflow
} // namespace U2

#endif
