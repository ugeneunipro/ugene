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

#include "utils/BaseShortReadsAlignerWorker.h"

namespace U2 {

class DnaAssemblyToRefTaskSettings;

namespace LocalWorkflow {

class BowtieWorker : public BaseShortReadsAlignerWorker {
    Q_OBJECT
public:
    BowtieWorker(Actor* p);

protected:
    QVariantMap getCustomParameters() const override;
    QString getDefaultFileName() const override;
    QString getBaseSubdir() const override;
    void setGenomeIndex(DnaAssemblyToRefTaskSettings& settings) override;
};  // BowtieWorker

class BowtieWorkerFactory : public BaseShortReadsAlignerWorkerFactory {
public:
    static const QString ACTOR_ID;

    BowtieWorkerFactory()
        : BaseShortReadsAlignerWorkerFactory(ACTOR_ID) {
    }
    static void init();
    Worker* createWorker(Actor* a) override;
};  // BowtieWorkerFactory

}  // namespace LocalWorkflow
}  // namespace U2
