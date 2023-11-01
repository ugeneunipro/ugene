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
#include <U2Core/U2Region.h>

#include <U2Test/XMLTestUtils.h>

#include "primer3_core/libprimer3.h"

namespace U2 {

class Annotation;
class AnnotationTableObject;
class U2SequenceObject;
class Primer3TaskSettings;
class Primer3TopLevelTask;
class PrimerPair;
class PrimerSingle;

class GTest_Primer3 : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY_EXT(GTest_Primer3, "plugin_primer_3", TaskFlags(TaskFlag_FailOnSubtaskCancel) | TaskFlag_NoRun);

    void prepare();
    Task::ReportResult report();

private:
    bool readPrimer(QDomElement element, QString prefix, QSharedPointer<PrimerSingle> outPrimer, bool internalOligo);

    bool comparePrimerSingleFromPairAndAnnotation(const QSharedPointer<PrimerPair>& pair, const QList<Annotation*>& annotations, oligo_type type);
    bool comparePrimerSingleAndAnnotation(const QSharedPointer<PrimerSingle>& primer, Annotation* annotation, const QString& primerText, int productSize = 0);

    bool checkIntProperty(int value, int expectedValue, QString name);
    bool checkDoubleProperty(double value, double expectedValue, QString name);

    QString seqObjCtx;
    bool circular = false;
    U2Region range;
    QString reportPath;
    QSharedPointer<QTemporaryFile> csvReportTmpFile;
    U2SequenceObject* seqObj = nullptr;
    QSharedPointer<AnnotationTableObject> annotationTableObject;

    Primer3TopLevelTask* task = nullptr;
    QSharedPointer<Primer3TaskSettings> settings;
    QList<QSharedPointer<PrimerPair>> expectedBestPairs;
    QList<QSharedPointer<PrimerSingle>> expectedSinglePrimers;
    QString expectedErrorMessage;
    QString expectedWarningMessage;
    QString localErrorMessage;
    int stopCodonPos = 0;
    int qualityNumber = 0;
};

}  // namespace U2
