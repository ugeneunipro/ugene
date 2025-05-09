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

#include <QDomElement>
#include <QFileInfo>

#include <U2Core/GObject.h>

#include <U2Test/XMLTestUtils.h>

#include "bwa/BwaTask.h"

namespace U2 {

class BwaGObjectTask;
class DnaAssemblyMultiTask;
class MsaObject;

class GTest_Bwa : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_Bwa, "bwa", TaskFlag_FailOnSubtaskCancel)

    void prepare() override;
    void run() override;
    Task::ReportResult report() override;
    void cleanup() override;
    QString getTempDataDir();
    QList<Task*> onSubTaskFinished(Task* subTask) override;

private:
    DnaAssemblyToRefTaskSettings config;
    QString readsFileName;
    GUrl readsFileUrl;
    QString indexName;
    QString patternFileName;
    QString negativeError;
    QString resultDirPath;
    bool usePrebuildIndex;
    bool subTaskFailed;
    BwaTask* bwaTask;
};

class BwaTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}  // namespace U2
