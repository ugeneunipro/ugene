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

#include <U2Designer/PrompterBase.h>

#include <U2Lang/LocalDomain.h>

#include "CAP3SupportTask.h"

namespace U2 {

namespace LocalWorkflow {

class CAP3Prompter : public PrompterBase<CAP3Prompter> {
    Q_OBJECT
public:
    CAP3Prompter(Actor* p = 0);

protected:
    QString composeRichDoc() override;
};

class CAP3Worker : public BaseWorker {
    Q_OBJECT
public:
    CAP3Worker(Actor* a);

    void init() override;
    Task* tick() override;
    void cleanup() override;

private slots:
    void sl_taskFinished();

protected:
    IntegralBus* input;

    CAP3SupportTaskSettings settings;
    QStringList inputSeqUrls;  // contains URL(s) from a current dataset
    QString currentDatasetName;  // used on each tick() to split URL(s) in datasets
    int datasetNumber;

private:
    /** Use data filled by a user to init the CAP3 settings */
    void initSettings();

    /** Init external tool and temporary folder paths */
    void initPaths();

    /**
     * Corrects output file name, if required (for several datasets).
     * Runs the CAP3 task and cleans up input URL(s) in the settings.
     */
    Task* runCap3();
};

class CAP3WorkerFactory : public DomainFactory {
public:
    static const QString ACTOR_ID;
    static void init();
    CAP3WorkerFactory()
        : DomainFactory(ACTOR_ID) {
    }
    Worker* createWorker(Actor* a) override {
        return new CAP3Worker(a);
    }
};

}  // namespace LocalWorkflow
}  // namespace U2
