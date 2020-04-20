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

#include <U2Algorithm/PWMConversionAlgorithm.h>
#include <U2Algorithm/PWMConversionAlgorithmRegistry.h>

#include <U2Core/AppContext.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GObject.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/MultipleSequenceAlignmentObject.h>
#include <U2Core/U2SafePoints.h>

#include "PWMatrixTests.h"

namespace U2 {

    /* TRANSLATOR U2::GTest */

#define OBJ_ATTR    "obj"
#define OBJ_TYPE    "filetype"
#define OBJ_SIZE    "size"
#define OBJ_LENGTH  "length"
#define PFM_TYPE    "pfmtype"
#define PWM_TYPE    "pwmtype"
#define PWM_ALGO    "algorithm"
#define EXPECTED    "expected-values"

//---------------------------------------------------------------------
void GTest_PFMtoPWMConvertTest::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }
}

Task::ReportResult GTest_PFMtoPWMConvertTest::report() {

    return ReportResult_Finished;
}

void GTest_PFMCreateTest::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    objType = el.attribute(OBJ_TYPE);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_TYPE);
        return;
    }

    QString buf = el.attribute(OBJ_SIZE);
    if (buf.isEmpty()) {
        failMissingValue(OBJ_SIZE);
        return;
    }
    bool ok = false;
    size = buf.toInt(&ok);
    if (!ok) {
        failMissingValue(OBJ_SIZE);
        return;
    }

    buf = el.attribute(OBJ_LENGTH);
    if (buf.isEmpty()) {
        failMissingValue(OBJ_LENGTH);
        return;
    }
    ok = false;
    length = buf.toInt(&ok);
    if (!ok) {
        failMissingValue(OBJ_LENGTH);
        return;
    }

    buf = el.attribute(PFM_TYPE);
    if (buf.isEmpty()) {
        failMissingValue(PFM_TYPE);
        return;
    }
    int table_size;
    if (buf == "mononucleotide") {
        type = PFM_MONONUCLEOTIDE;
        table_size = 4;
    } else if (buf == "dinucleotide") {
        type = PFM_DINUCLEOTIDE;
        table_size = 16;
    } else {
        failMissingValue(PFM_TYPE);
        return;
    }
    buf = el.attribute(EXPECTED);
    if (buf.isEmpty()) {
        failMissingValue(EXPECTED);
        return;
    }
    QStringList rows = buf.split(";");
    if (rows.size() != table_size) {
        failMissingValue(EXPECTED);
        return;
    }
    for (int i = 0; i < table_size; i++) {
        values[i].resize(type == PFM_MONONUCLEOTIDE ? length : length - 1);
        QStringList curr = rows[i].split(",");
        if (curr.size() != values[i].size()) {
            failMissingValue(EXPECTED);
            return;
        }
        for (int j = 0, n = curr.size(); j < n; j++) {
            ok = false;
            values[i][j] = curr[j].toInt(&ok);
            if (!ok) {
                failMissingValue(OBJ_LENGTH);
                return;
            }
        }
    }
}

Task::ReportResult GTest_PFMCreateTest::report() {
    if (objType == "alignment") {
        Document* doc = getContext<Document>(this, objContextName);
        if (doc == NULL) {
            stateInfo.setError(GTest::tr("context not found %1").arg(objContextName));
            return ReportResult_Finished;
        }

        QList<GObject*> list = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
        if (list.size() == 0) {
            stateInfo.setError(GTest::tr("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
            return ReportResult_Finished;
        }
        MultipleSequenceAlignmentObject *myAlign = qobject_cast<MultipleSequenceAlignmentObject *>(list.first());
        const MultipleSequenceAlignment al = myAlign->getMultipleAlignment();
        PFMatrix pfm(al, type);
        for (int i = 0, n = (type == PFM_MONONUCLEOTIDE) ? 4 : 16; i < n; i++) {
            for (int j = 0, nn = (type == PFM_MONONUCLEOTIDE) ? length : length - 1; j < nn; j++) {
                if (values[i][j] != pfm.getValue(i, j)) {
                    stateInfo.setError(  QString("Actual results not equal with expected") );
                    return ReportResult_Finished;
                }
            }
        }
    } else if (objType == "sequences") {
        Document* doc = getContext<Document>(this, objContextName);
        if (doc == NULL) {
            stateInfo.setError(GTest::tr("context not found %1").arg(objContextName));
            return ReportResult_Finished;
        }

        QList<GObject*> list = doc->findGObjectByType(GObjectTypes::SEQUENCE);
        if (list.size() == 0 || list.size() < size) {
            stateInfo.setError(GTest::tr("container of object with type \"%1\" is empty or less than %2").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT).arg(size));
            return ReportResult_Finished;
        }
        QList<DNASequence*> data;
        for (int i = 0; i < size; i++) {
            U2SequenceObject* seq = (U2SequenceObject*)list[i];
            if (seq->getSequenceLength() != length) {
                stateInfo.setError(QString("wrong length of %1 sequence: %2").arg(i + 1).arg(seq->getSequenceLength()));
                return ReportResult_Finished;
            }
            if (seq->getAlphabet()->getType() != DNAAlphabet_NUCL) {
                stateInfo.setError(QString("Wrong sequence alphabet"));
                return ReportResult_Finished;
            }
            DNASequence* s = new DNASequence(QString("%1 sequence").arg(i + 1), seq->getWholeSequenceData(stateInfo), seq->getAlphabet());
            CHECK_OP(stateInfo, ReportResult_Finished);
            data.push_back(s);
        }
        PFMatrix pfm(data, type);
        for (int i = 0, n = (type == PFM_MONONUCLEOTIDE) ? 4 : 16; i < n; i++) {
            for (int j = 0, nn = (type == PFM_MONONUCLEOTIDE) ? length : length - 1; j < nn; j++) {
                if (values[i][j] != pfm.getValue(i, j)) {
                    stateInfo.setError( GTest::tr("Actual results not equal with expected, row %1 column %2\nExpected %3, got %4").arg(i).arg(j).arg(values[i][j]).arg(pfm.getValue(i, j)));
                    return ReportResult_Finished;
                }
            }
        }
    } else {
        stateInfo.setError(QString("wrong value: %1").arg(OBJ_TYPE));
        return ReportResult_Finished;
    }

    return ReportResult_Finished;
}

void GTest_PWMCreateTest::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    objContextName = el.attribute(OBJ_ATTR);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_ATTR);
        return;
    }

    objType = el.attribute(OBJ_TYPE);
    if (objContextName.isEmpty()) {
        failMissingValue(OBJ_TYPE);
        return;
    }

    QString buf = el.attribute(OBJ_SIZE);
    if (buf.isEmpty()) {
        failMissingValue(OBJ_SIZE);
        return;
    }
    bool ok = false;
    size = buf.toInt(&ok);
    if (!ok) {
        failMissingValue(OBJ_SIZE);
        return;
    }

    buf = el.attribute(OBJ_LENGTH);
    if (buf.isEmpty()) {
        failMissingValue(OBJ_LENGTH);
        return;
    }
    ok = false;
    length = buf.toInt(&ok);
    if (!ok) {
        failMissingValue(OBJ_LENGTH);
        return;
    }

    buf = el.attribute(PWM_TYPE);
    if (buf.isEmpty()) {
        failMissingValue(PWM_TYPE);
        return;
    }
    int table_size;
    if (buf == "mononucleotide") {
        type = PWM_MONONUCLEOTIDE;
        table_size = 4;
    } else if (buf == "dinucleotide") {
        type = PWM_DINUCLEOTIDE;
        table_size = 16;
    } else {
        failMissingValue(PWM_TYPE);
        return;
    }
    buf = el.attribute(EXPECTED);
    if (buf.isEmpty()) {
        failMissingValue(EXPECTED);
        return;
    }

    algo = el.attribute(PWM_ALGO);
    if (algo.isEmpty()) {
        failMissingValue(PWM_ALGO);
        return;
    }

    QStringList rows = buf.split(";");
    if (rows.size() != table_size) {
        failMissingValue(EXPECTED);
        return;
    }
    for (int i = 0; i < table_size; i++) {
        values[i].resize(type == PWM_MONONUCLEOTIDE ? length : length - 1);
        QStringList curr = rows[i].split(",");
        if (curr.size() != values[i].size()) {
            failMissingValue(EXPECTED);
            return;
        }
        for (int j = 0, n = curr.size(); j < n; j++) {
            ok = false;
            values[i][j] = curr[j].toDouble(&ok);
            if (!ok) {
                failMissingValue(OBJ_LENGTH);
                return;
            }
        }
    }
}

Task::ReportResult GTest_PWMCreateTest::report() {
    PWMConversionAlgorithmFactory* fact = AppContext::getPWMConversionAlgorithmRegistry()->getAlgorithmFactory(algo);
    if (fact == NULL) {
        stateInfo.setError(GTest::tr("algorithm not found %1").arg(algo));
        return ReportResult_Finished;
    }
    PWMConversionAlgorithm* algorithm = fact->createAlgorithm();
    if (algorithm == NULL) {
        stateInfo.setError(GTest::tr("unable to create algorithm %1").arg(algo));
        return ReportResult_Finished;
    }
    PWMatrix pwm;
    PFMatrixType pftype = (type == PWM_MONONUCLEOTIDE) ? PFM_MONONUCLEOTIDE : PFM_DINUCLEOTIDE;
    if (objType == "alignment") {
        Document* doc = getContext<Document>(this, objContextName);
        if (doc == NULL) {
            stateInfo.setError(GTest::tr("context not found %1").arg(objContextName));
            return ReportResult_Finished;
        }

        QList<GObject*> list = doc->findGObjectByType(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT);
        if (list.size() == 0) {
            stateInfo.setError(GTest::tr("container of object with type \"%1\" is empty").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT));
            return ReportResult_Finished;
        }
        MultipleSequenceAlignmentObject * myAlign = (MultipleSequenceAlignmentObject*)list.first();
        const MultipleSequenceAlignment al = myAlign->getMultipleAlignment();

        PFMatrix pfm(al, pftype);
        pwm = algorithm->convert(pfm);

    } else if (objType == "sequences") {
        Document* doc = getContext<Document>(this, objContextName);
        if (doc == NULL) {
            stateInfo.setError(GTest::tr("context not found %1").arg(objContextName));
            return ReportResult_Finished;
        }

        QList<GObject*> list = doc->findGObjectByType(GObjectTypes::SEQUENCE);
        if (list.size() == 0 || list.size() < size) {
            stateInfo.setError(GTest::tr("container of object with type \"%1\" is empty or less than %2").arg(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT).arg(size));
            return ReportResult_Finished;
        }
        QList<DNASequence*> data;
        for (int i = 0; i < size; i++) {
            U2SequenceObject* seq = (U2SequenceObject*)list[i];
            if (seq->getSequenceLength() != length) {
                stateInfo.setError(GTest::tr("wrong length of %1 sequence: %2").arg(i).arg(seq->getSequenceLength()));
                return ReportResult_Finished;
            }
            if (seq->getAlphabet()->getType() != DNAAlphabet_NUCL) {
                stateInfo.setError(GTest::tr("Wrong %1 sequence alphabet").arg(i));
                return ReportResult_Finished;
            }
            DNASequence* s = new DNASequence(QString("%1 sequence").arg(i), seq->getWholeSequenceData(stateInfo), seq->getAlphabet());
            CHECK_OP(stateInfo, ReportResult_Finished);
            data.push_back(s);
        }

        PFMatrix pfm(data, pftype);
        pwm = algorithm->convert(pfm);
    } else {
        stateInfo.setError(GTest::tr("wrong value: %1").arg(OBJ_TYPE));
        return ReportResult_Finished;
    }

    for (int i = 0, n = (type == PWM_MONONUCLEOTIDE) ? 4 : 16; i < n; i++) {
        for (int j = 0, nn = (type == PWM_MONONUCLEOTIDE) ? length : length - 1; j < nn; j++) {
            if (qAbs(values[i][j] - pwm.getValue(i, j)) > 1e-4) {
                stateInfo.setError( GTest::tr("Actual results not equal with expected, row %1 column %2\nExpected %3, got %4").arg(i).arg(j).arg(values[i][j], 20, 'f', 12).arg(pwm.getValue(i, j), 20, 'f', 12));
                return ReportResult_Finished;
            }
        }
    }

    return ReportResult_Finished;
}

//---------------------------------------------------------------------
QList<XMLTestFactory*> PWMatrixTests::createTestFactories() {
    QList<XMLTestFactory*> res;
    res.append(GTest_PFMtoPWMConvertTest::createFactory());
    res.append(GTest_PFMCreateTest::createFactory());
    res.append(GTest_PWMCreateTest::createFactory());
    return res;
}

}
