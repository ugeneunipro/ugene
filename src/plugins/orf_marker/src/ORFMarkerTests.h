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

#ifndef _U2_ORF_MARKER_TESTS_H_
#define _U2_ORF_MARKER_TESTS_H_

#include <U2Test/XMLTestUtils.h>
#include <U2Core/GObject.h>
#include <QDomElement>
#include <U2Core/U2Region.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2Algorithm/ORFAlgorithmTask.h>

#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNATranslation.h>
#include <U2Core/AppContext.h>

namespace U2 {

class GTest_ORFMarkerTask : public XmlTest {
    Q_OBJECT
public:
    SIMPLE_XML_TEST_BODY_WITH_FACTORY(GTest_ORFMarkerTask, "plugin_orf-marker-search");

    void prepare();
    Task::ReportResult report();
private:
    QString seqName;
    QString translationId;
    ORFAlgorithmSettings settings;
    QVector<U2Region> expectedResults;
    ORFFindTask *task;
};

//FIXME! this class is a partial copy of DetView
class Translator {
public:
    Translator(const U2SequenceObject *s, const QString& tid);

    //void setAminoTranslation(const QString& id);
    DNATranslation* getComplTranslation() const {return complTransl;}
    DNATranslation* getAminoTranslation() const {return aminoTransl;}

private:
    //QStringList translations;
    const U2SequenceObject* seq;
    DNATranslation* complTransl;
    DNATranslation* aminoTransl;
};

} //namespace
#endif

