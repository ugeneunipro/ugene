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

#include <U2Lang/DatasetFetcher.h>
#include <U2Lang/LocalDomain.h>

#include "Kraken2ClassifyTask.h"

namespace U2 {
namespace LocalWorkflow {

class Kraken2ClassifyWorker : public BaseWorker {
    Q_OBJECT
public:
    Kraken2ClassifyWorker(Actor *actor);

    void init() override;
    Task *tick() override;
    void cleanup() override;
    bool isReady() const override;

private slots:
    void sl_taskFinished(Task *task);

private:
    bool isReadyToRun() const;
    bool dataFinished() const;

    Kraken2ClassifyTaskSettings getSettings(U2OpStatus &os);
    IntegralBus *input = nullptr;
    IntegralBus *pairedInput = nullptr;
    bool isPairedReadsInput = false;

    static const QString KRAKEN_DIR;
};

}  // namespace LocalWorkflow
}  // namespace U2
