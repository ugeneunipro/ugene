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
#include <QSharedPointer>

#include <U2Algorithm/TmCalculator.h>

#include <U2Core/U2Region.h>
#include <U2Core/U2Type.h>

#include <U2Test/XMLTestUtils.h>

#include <U2View/DNAStatisticsTask.h>

namespace U2 {

class DNAStatisticsTask;

class GTest_DnaStatisticsTest : public XmlTest {
    Q_OBJECT
        SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_DnaStatisticsTest, "dna-statistics")

private:
    void prepare() override;
    ReportResult report() override;

    DNAStatisticsTask* task;

    // input values
    QString docName;
    QString seqName;
    QString seq;
    QVector<U2Region> regions;
    QSharedPointer<TmCalculator> temperatureCalculator;

    // expected values
    DNAStatistics expectedStats;

    /**
     * Use either @DOC_NAME_ATTR and @SEQ_NAME_ATTR if you want to load sequence from file,
     * or @SEQ_ATTR if you set up sequence as argument and name as test name
     */
    static const QString DOC_NAME_ATTR;
    static const QString SEQ_NAME_ATTR;
    static const QString SEQ_ATTR;
    /** 
     * 0-based, GenBank-like format without 'join' word: 1..20,40..60
     */
    static const QString REGIONS_ATTR;
    /**
     * Semicolon separated list of '=' separated pairs,
     * where the first value is a key (see @QVariantMap and its heirs)
     * and the second value is the value of the corresponding parameter.
     */
    static const QString TEMPERATURE_CALCULATION_ATTR;

    static const QString EXPECTED_LENGTH;
    static const QString EXPECTED_GC_CONTENT;
    static const QString EXPECTED_MELTING_TEMP;

    static const QString EXPECTED_SS_MOLECULAR_WEIGHT;
    static const QString EXPECTED_SS_EXTINCTION_COEFFICIENT;
    static const QString EXPECTED_SS_OD260_AMOUNT_OF_SUBSTANCE;  // in nanomoles
    static const QString EXPECTED_SS_OD260_MASS;  // in micrograms

    static const QString EXPECTED_DS_MOLECULAR_WEIGHT;
    static const QString EXPECTED_DS_EXTINCTION_COEFFICIENT;
    static const QString EXPECTED_DS_OD260_AMOUNT_OF_SUBSTANCE;  // in nanomoles
    static const QString EXPECTED_DS_OD260_MASS;  // in micrograms

    static const QString EXPECTED_ISOELECTRIC_POINT;
};

class DnaStatisticsTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

}  // namespace U2
