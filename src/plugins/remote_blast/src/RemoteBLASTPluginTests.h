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

#include <U2Test/XMLTestUtils.h>

#include "RemoteBLASTTask.h"

namespace U2 {

class U2SequenceObject;

// cppcheck-suppress noConstructor
class GTest_RemoteBLAST : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_RemoteBLAST, "plugin_remote-query");

    void prepare();
    void cleanup();
    Task::ReportResult report();

private:
    QString seqId;
    QString annId;
    U2SequenceObject* seqObj = nullptr;
    QPointer<AnnotationTableObject> ao;
    Task* task = nullptr;
    int minLength;
    int maxLength;
    QString index;
    QString sequence;
    QString request;
    QString algoritm;
    QStringList expectedResults;
    bool simple;
};

}  // namespace U2
