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

#pragma once

#include <QDomElement>
#include <QSharedPointer>
#include <QTemporaryFile>

#include <U2Core/GObject.h>
#include <U2Core/global.h>

#include <U2Test/XMLTestUtils.h>

#include "Primer3Task.h"

namespace U2 {

class GTest_Primer3 : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_Primer3, "plugin_primer_3", TaskFlags(TaskFlag_FailOnSubtaskCancel) | TaskFlag_NoRun);

    void prepare();
    Task::ReportResult report();

private:
    bool readPrimer(QDomElement element, QString prefix, QSharedPointer<PrimerSingle> outPrimer, bool internalOligo);

    bool checkPrimerPair(const QSharedPointer<PrimerPair>& primerPair, const QSharedPointer<PrimerPair>& expectedPrimerPair, QString suffix);
    bool checkPrimer(const QSharedPointer<PrimerSingle>& primer, const QSharedPointer<PrimerSingle>& expectedPrimer, QString prefix, bool internalOligo);
    bool checkIntProperty(int value, int expectedValue, QString name);
    bool checkDoubleProperty(double value, double expectedValue, QString name);

    Primer3Task* task = nullptr;
    QSharedPointer<Primer3TaskSettings> settings;
    QList<QSharedPointer<PrimerPair>> expectedBestPairs;
    QList<QSharedPointer<PrimerSingle>> expectedSinglePrimers;
    QString expectedErrorMessage;
    QString expectedWarningMessage;
    QString localErrorMessage;
    int stopCodonPos = 0;
    int qualityNumber = 0;
};


class GTest_Primer3ToAnnotations : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_Primer3ToAnnotations, "primer-3-to-annotations", TaskFlags(TaskFlag_FailOnSubtaskCancel) | TaskFlag_NoRun);

    void prepare();
    Task::ReportResult report();

private:
    Primer3ToAnnotationsTask* task = nullptr;
    QSharedPointer<Primer3TaskSettings> settings;
    U2SequenceObject* seqObj = nullptr;
    AnnotationTableObject* annObj = nullptr;
    QString seqObjCtxName;
    QString aObjName;
    QPair<U2Region, U2Region> resultPrimerAnnotationsRegions;
    QString reportPath;
    QSharedPointer<QTemporaryFile> csvReportTmpFile;

    static const QString TOP_PRIMERS;

};


}  // namespace U2
