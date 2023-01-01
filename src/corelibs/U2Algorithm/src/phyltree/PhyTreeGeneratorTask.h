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

#ifndef __PHYTREEGENERATORTASK_H
#define __PHYTREEGENERATORTASK_H

#include <U2Algorithm/CreatePhyTreeSettings.h>

#include <U2Core/Task.h>

#include "PhyTreeGenerator.h"

namespace U2 {

class U2ALGORITHM_EXPORT PhyTreeGeneratorTask : public Task {
    Q_OBJECT
public:
    PhyTreeGeneratorTask(const MultipleSequenceAlignment& ma,
                         const CreatePhyTreeSettings& settings,
                         const TaskFlags& taskFlags = TaskFlag_NoRun | TaskFlag_FailOnSubtaskError);

    const PhyTree& getResult() const;

    const CreatePhyTreeSettings& getSettings() const;

protected:
    const MultipleSequenceAlignment inputMA;
    PhyTree result;
    CreatePhyTreeSettings settings;
};

class U2ALGORITHM_EXPORT PhyTreeGeneratorLauncherTask : public Task {
    Q_OBJECT
public:
    PhyTreeGeneratorLauncherTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& _settings);
    const PhyTree& getResult() const;
    void prepare() override;
    ReportResult report() override;

public slots:
    void sl_onCalculationCanceled();

private:
    MultipleSequenceAlignment inputMA;
    QStringList originalRowNameByIndex;
    PhyTree result;
    CreatePhyTreeSettings settings;
    PhyTreeGeneratorTask* task = nullptr;
};

}  // namespace U2

#endif
