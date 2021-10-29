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
 
#ifndef _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_TASK_TEST_H_
#define _U2_PCR_PRIMER_DESIGN_FOR_DNA_ASSEMBLY_TASK_TEST_H_

#include <QObject>
#include <QDomElement>

#include <U2Core/Task.h>

#include <U2Test/XMLTestUtils.h>

#include "PCRPrimerDesignForDNAAssemblyTask.h"

namespace U2 {

class GTest_PCRPrimerDesignForDNAAssemblyTaskTest : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_PCRPrimerDesignForDNAAssemblyTaskTest, "primer-design-for-dna-assembly-task", TaskFlags_NR_FOSCOE);
    void prepare();
    Task::ReportResult report();

private:
    QString docName;
    QString seqName;
    PCRPrimerDesignForDNAAssemblyTaskSettings settings;
    PCRPrimerDesignForDNAAssemblyTask *task = nullptr;
    QByteArray sequence;
    QList<U2Region> result;
};

class PCRPrimerDesignForDNAAssemblyTaskTest {
public:
    static QList<XMLTestFactory *> createTestFactories();
};

}

#endif
