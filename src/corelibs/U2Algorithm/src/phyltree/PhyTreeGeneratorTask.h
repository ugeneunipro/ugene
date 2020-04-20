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

#ifndef  __PHYTREEGENERATORTASK_H
#define  __PHYTREEGENERATORTASK_H

#include <U2Core/Task.h>
#include "PhyTreeGenerator.h"
#include <U2Algorithm/CreatePhyTreeSettings.h>

namespace U2{

class U2ALGORITHM_EXPORT PhyTreeGeneratorTask: public Task{
    Q_OBJECT
public:
    PhyTreeGeneratorTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& _settings);
    ~PhyTreeGeneratorTask(){}
    void run();
    PhyTree getResult() { return result; }
    const CreatePhyTreeSettings& getSettings() { return settings; }
    ReportResult report();
protected:
    const MultipleSequenceAlignment inputMA;
    PhyTree                     result;
    CreatePhyTreeSettings       settings;
};

class SeqNamesConvertor {
public:
    SeqNamesConvertor() : lastIdStr("a") {}

    void replaceNamesWithAlphabeticIds(MultipleSequenceAlignment& ma);
    void restoreNames(const PhyTree& tree);

private:
    const QString& generateNewAlphabeticId();

    QString lastIdStr;
    QMap<QString, QString> namesMap;
};

class U2ALGORITHM_EXPORT PhyTreeGeneratorLauncherTask: public Task{
    Q_OBJECT
public:
    PhyTreeGeneratorLauncherTask(const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& _settings);
    ~PhyTreeGeneratorLauncherTask(){};
    PhyTree getResult() { return result; }
    void prepare();
    void run(){};
    ReportResult report();
private slots:
    void sl_onCalculationCanceled();
private:
    MultipleSequenceAlignment                  inputMA;
    PhyTree                     result;
    CreatePhyTreeSettings       settings;
    PhyTreeGeneratorTask*       task;
    SeqNamesConvertor           namesConvertor;
};


} //namespace

#endif
