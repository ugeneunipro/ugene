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

#include "Primer3Tests.h"
#include "primer3_core/primer3.h"

namespace U2{

#define DBFILEPATH_ATTR "dbfile"
#define PROTEIN_ATTR "is_protein"
#define EXPECTED_LOAD_ATTR "is_load_expected"
#define CREATION_DATE_ATTR "creation_date"
#define CONTEXT_NAME_ATTR "index"
#define CHECK_DATE_ATTR "check_date"
/*
PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY
PRIMER_MISPRIMING_LIBRARY
PRIMER_SEQUENCE_QUALITY
*/

static const QString extensionsToCheck[14] = {".nhr",".nnd",".nni",".nsd",".nsi",".nsq", ".nin",
    ".phr",".pnd",".pni",".psd",".psi",".psq", ".pin"};

namespace
{
    bool readPrimer(QDomElement element, QString prefix, Primer *outPrimer, bool internalOligo)
    {
        {
            QString buf = element.attribute(prefix);
            if(!buf.isEmpty())
            {
                outPrimer->setStart(buf.split(',')[0].toInt());
                outPrimer->setLength(buf.split(',')[1].toInt());
            }
            else
            {
                return false;
            }
        }
        {
            QString buf = element.attribute(prefix + "_TM");
            if(!buf.isEmpty())
            {
                outPrimer->setMeltingTemperature(buf.toDouble());
            }
        }
        {
            QString buf = element.attribute(prefix + "_GC_PERCENT");
            if(!buf.isEmpty())
            {
                outPrimer->setGcContent(buf.toDouble());
            }
        }
        {
            QString buf = element.attribute(prefix + "_SELF_ANY");
            if(!buf.isEmpty())
            {
                outPrimer->setSelfAny((short)(buf.toDouble()*100));
            }
        }
        {
            QString buf = element.attribute(prefix + "_SELF_END");
            if(!buf.isEmpty())
            {
                outPrimer->setSelfEnd((short)(buf.toDouble()*100));
            }
        }
        if(!internalOligo)
        {
            QString buf = element.attribute(prefix + "_END_STABILITY");
            if(!buf.isEmpty())
            {
                outPrimer->setEndStability(buf.toDouble());
            }
        }
        return true;
    }

    PrimerPair readPrimerPair(QDomElement element, QString suffix)
    {
        PrimerPair result;
        {
            Primer primer;
            if(readPrimer(element, "PRIMER_LEFT" + suffix, &primer, false))
            {
                result.setLeftPrimer(&primer);
            }
        }
        {
            Primer primer;
            if(readPrimer(element, "PRIMER_RIGHT" + suffix, &primer, false))
            {
                result.setRightPrimer(&primer);
            }
        }
        {
            Primer primer;
            if(readPrimer(element, "PRIMER_INTERNAL_OLIGO" + suffix, &primer, true))
            {
                result.setInternalOligo(&primer);
            }
        }
        {
            QString buf = element.attribute("PRIMER_PAIR" + suffix + "_COMPL_ANY");
            if(!buf.isEmpty())
            {
                result.setComplAny((short)(buf.toDouble()*100));
            }
        }
        {
            QString buf = element.attribute("PRIMER_PAIR" + suffix + "_COMPL_END");
            if(!buf.isEmpty())
            {
                result.setComplEnd((short)(buf.toDouble()*100));
            }
        }
        {
            QString buf = element.attribute("PRIMER_PRODUCT_SIZE" + suffix);
            if(!buf.isEmpty())
            {
                result.setProductSize(buf.toInt());
            }
        }
        return result;
    }
}

void GTest_Primer3::init(XMLTestFormat *tf, const QDomElement& el){
    Q_UNUSED(tf);

    settings.setIncludedRegion(U2Region(0,-1));

    QString buf;
    int n_quality = 0;
    QDomNodeList inputParameters = el.elementsByTagName("plugin_primer_3_in");
    for(int i=0;i<inputParameters.size(); i++) {
        QDomNode n = inputParameters.item(i);
        assert(n.isElement());
        if (!n.isElement()) {
            continue;
        }
        QDomElement elInput = n.toElement();

    foreach(QString key, settings.getIntPropertyList())
    {
        buf = elInput.attribute(key);
        if (!buf.isEmpty()){
            settings.setIntProperty(key, buf.toInt());
        }
    }
    foreach(QString key, settings.getDoublePropertyList())
    {
        buf = elInput.attribute(key);
        if (!buf.isEmpty()){
            settings.setDoubleProperty(key, buf.toDouble());
        }
    }
    foreach(QString key, settings.getAlignPropertyList())
    {
        buf = elInput.attribute(key);
        if (!buf.isEmpty()){
            settings.setAlignProperty(key, (short)(buf.toDouble()*100));
        }
    }
//1
    buf = elInput.attribute("PRIMER_SEQUENCE_ID");
    if (!buf.isEmpty()){
        settings.setSequenceName(buf.toLatin1());
    }
//2
    buf = elInput.attribute("SEQUENCE");
    if (!buf.isEmpty()){
        settings.setSequence(buf.toLatin1());
    }

    buf = elInput.attribute("CIRCULAR");
    if (!buf.isEmpty()) {
        settings.setCircularity(buf == "true" ? 1 : 0);
    }

//3
    buf = elInput.attribute("TARGET");
    if (!buf.isEmpty()){
        QList< U2Region > regionList;
        foreach(QString str,buf.split(' ',QString::SkipEmptyParts))
        {
            if(str.split(',').size() >= 2)
            {
                regionList.append(U2Region(str.split(',')[0].toInt(), str.split(',')[1].toInt()));
            }
            else
            {
                stateInfo.setError(GTest::tr("Illegal TARGET value: %1").arg(buf));
                break;
            }
        }
        settings.setTarget(regionList);
    }
//8
    buf = elInput.attribute("PRIMER_PRODUCT_SIZE_RANGE");
    if (!buf.isEmpty()){
        QList< U2Region > regionList;
        foreach(QString str,buf.split(' ',QString::SkipEmptyParts))
        {
            if(2 == str.split('-').size())
            {
                regionList.append(U2Region(str.split('-')[0].toInt(),
                                  str.split('-')[1].toInt() - str.split('-')[0].toInt() + 1));
            }
            else
            {
                stateInfo.setError(GTest::tr("Illegal PRIMER_DEFAULT_PRODUCT value: %1").arg(buf));
                break;
            }
        }
        settings.setProductSizeRange(regionList);
    }
//11
    buf = elInput.attribute("PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION");
    if (!buf.isEmpty()){
        QList< U2Region > regionList;
        foreach(QString str,buf.split(' ',QString::SkipEmptyParts))
        {
            if(2 == str.split(',').size())
            {
                regionList.append(U2Region(str.split(',')[0].toInt(), str.split(',')[1].toInt()));
            }
            else
            {
                stateInfo.setError(GTest::tr("Illegal PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION value: %1").arg(buf));
                break;
            }
        }
        settings.setInternalOligoExcludedRegion(regionList);
    }
//22???
    buf = elInput.attribute("INCLUDED_REGION");
    if (!buf.isEmpty()){
        if(2 == buf.split(',').size())
        {
            settings.setIncludedRegion(U2Region(buf.split(',')[0].toInt(), buf.split(',')[1].toInt()));//??? may be wrong
        }
        else
        {
            stateInfo.setError(GTest::tr("Illegal INCLUDED_REGION value: %1").arg(buf));
        }
    }
//32
    buf = elInput.attribute("PRIMER_LEFT_INPUT");
    if (!buf.isEmpty()){
        settings.setLeftInput(buf.toLatin1());
    }
//33
    buf = elInput.attribute("PRIMER_RIGHT_INPUT");
    if (!buf.isEmpty()){
        settings.setRightInput(buf.toLatin1());
    }
//34
    buf = elInput.attribute("PRIMER_INTERNAL_OLIGO_INPUT");
    if (!buf.isEmpty()){
        settings.setInternalInput(buf.toLatin1());
    }
//35
    buf = elInput.attribute("MARKER_NAME");
    if (!buf.isEmpty()){
        settings.setSequenceName(buf.toLatin1());
    }
//37
    buf = elInput.attribute("PRIMER_DEFAULT_PRODUCT");
    if (!buf.isEmpty()){
        QList< U2Region > regionList;
        foreach(QString str,buf.split(' ',QString::SkipEmptyParts))
        {
            if(2 == str.split('-').size())
            {
                regionList.append(U2Region(str.split('-')[0].toInt(),
                                  str.split('-')[1].toInt() - str.split('-')[0].toInt() + 1));
            }
            else
            {
                stateInfo.setError(GTest::tr("Illegal PRIMER_DEFAULT_PRODUCT value: %1").arg(buf));
                break;
            }
        }
        settings.setProductSizeRange(regionList);
    }
//38
    buf = elInput.attribute("EXCLUDED_REGION");
    if (!buf.isEmpty()){
        QList< U2Region > regionList;
        foreach(QString str,buf.split(' ',QString::SkipEmptyParts))
        {
            if(2 == str.split(',').size())
            {
                regionList.append(U2Region(str.split(',')[0].toInt(), str.split(',')[1].toInt()));
            }
            else
            {
                stateInfo.setError(GTest::tr("Illegal EXCLUDED_REGION value: %1").arg(buf));
                break;
            }
        }
        settings.setExcludedRegion(regionList);
    }
//119
    buf = elInput.attribute("PRIMER_TASK");
    if (!buf.isEmpty()){
        if(0 == buf.compare("pick_pcr_primers", Qt::CaseInsensitive))
        {
            settings.setTask(pick_pcr_primers);
        }
        else if(0 == buf.compare("pick_pcr_primers_and_hyb_probe", Qt::CaseInsensitive))
        {
            settings.setTask(pick_pcr_primers_and_hyb_probe);
        }
        else if(0 == buf.compare("pick_left_only", Qt::CaseInsensitive))
        {
            settings.setTask(pick_left_only);
        }
        else if(0 == buf.compare("pick_right_only", Qt::CaseInsensitive))
        {
            settings.setTask(pick_right_only);
        }
        else if(0 == buf.compare("pick_hyb_probe_only", Qt::CaseInsensitive))
        {
            settings.setTask(pick_hyb_probe_only);
        }
        else
        {
            stateInfo.setError(GTest::tr("Unrecognized PRIMER_TASK"));//??? may be remove from this place
        }
    }
//10
    buf = elInput.attribute("PRIMER_PICK_INTERNAL_OLIGO");
    if (!buf.isEmpty()){
        int pick_internal_oligo = buf.toInt();
        if((pick_internal_oligo == 1 || pick_internal_oligo == 0) &&
            (settings.getTask() == pick_left_only ||
            settings.getTask() == pick_right_only ||
            settings.getTask() == pick_hyb_probe_only))
        {
            stateInfo.setError(GTest::tr("Contradiction in primer_task definition"));//??? may be remove from this place
        }
        else if(pick_internal_oligo == 1)
        {
            settings.setTask(pick_pcr_primers_and_hyb_probe);
        }
        else if(pick_internal_oligo == 0)
        {
            settings.setTask(pick_pcr_primers);
        }
    }
//120
    buf = elInput.attribute("PRIMER_SEQUENCE_QUALITY");
    if (!buf.isEmpty()){
        QVector<int> qualityVecor;
        QStringList qualityList=buf.split(' ',QString::SkipEmptyParts);
        n_quality=qualityList.size();
        for(int i=0;i<n_quality;i++){
            qualityVecor.append(qualityList.at(i).toInt());
        }
        settings.setSequenceQuality(qualityVecor);
    }
//121
    buf = elInput.attribute("PRIMER_MISPRIMING_LIBRARY");
    if (!buf.isEmpty()){
        settings.setRepeatLibrary((getEnv()->getVar("COMMON_DATA_DIR") + "/primer3/" + buf).toLatin1());
    }
//122
    buf = elInput.attribute("PRIMER_INTERNAL_OLIGO_MISHYB_LIBRARY");
    if (!buf.isEmpty()){
        settings.setMishybLibrary((getEnv()->getVar("COMMON_DATA_DIR") + "/primer3/" + buf).toLatin1());
    }
    }

    //////////////////////////////////////////////////////////////////////////
    // Check parameters
    QDomNodeList outputParameters = el.elementsByTagName("plugin_primer_3_out");
    for(int i=0;i<outputParameters.size(); i++) {
        QDomNode n = outputParameters.item(i);
        assert(n.isElement());
        if (!n.isElement()) {
            continue;
        }
        QDomElement elOutput = n.toElement();
        int pairsCount = 0;

        buf = elOutput.attribute("PRIMER_PAIRS_NUMBER");
        if (!buf.isEmpty()){
            pairsCount = buf.toInt();
        }

        for(int i=0;i < pairsCount;i++)
        {
            expectedBestPairs.append(readPrimerPair(elOutput, (i > 0)? ("_" + QString::number(i)):QString()));
        }
    }

    if (settings.getSequence().isEmpty())
        stateInfo.setError(GTest::tr("Missing SEQUENCE tag"));//??? may be remove from this place
    else {
        int sequenceLength = settings.getSequence().size();
        if (settings.getIncludedRegion().length == -1) {
            settings.setIncludedRegion(U2Region(settings.getFirstBaseIndex(), sequenceLength));
        }
        if(n_quality !=0 && n_quality != sequenceLength)
            stateInfo.setError(GTest::tr("Error in sequence quality data"));//??? may be remove from this place
        if((settings.getPrimerArgs()->min_quality != 0 || settings.getPrimerArgs()->io_min_quality != 0) && n_quality == 0)
            stateInfo.setError(GTest::tr("Sequence quality data missing"));//??? may be remove from this place
        if(settings.getPrimerArgs()->min_quality != 0 && settings.getPrimerArgs()->min_end_quality < settings.getPrimerArgs()->min_quality)
            settings.getPrimerArgs()->min_end_quality = settings.getPrimerArgs()->min_quality;
    }
}

void GTest_Primer3::prepare()
{
    task = new Primer3SWTask(settings);
    addSubTask(task);
}

Task::ReportResult GTest_Primer3::report()
{
    QList<PrimerPair> currentBestPairs = task->getBestPairs();

    if(task->hasError() && (expectedBestPairs.size() > 0))
    {
        stateInfo.setError(task->getError());
        return ReportResult_Finished;
    }

    if(currentBestPairs.size() != expectedBestPairs.size()){
        stateInfo.setError(GTest::tr("PRIMER_PAIRS_NUMBER is incorrect. Expected:%2, but Actual:%3").arg(expectedBestPairs.size()).arg(currentBestPairs.size()));
        return ReportResult_Finished;
    }

    for (int i=0;i<expectedBestPairs.size();i++)
    {
        if(!checkPrimerPair(currentBestPairs[i], expectedBestPairs[i], (i > 0)? ("_" + QString::number(i)):QString()))
        {
            return ReportResult_Finished;
        }
    }
/*    for (int i=0;i<currentBestPairs.num_pairs;i++)
    {
        //currentBestPairs->pairs[i]->
        if(!(currentBestPairs.pairs[i].left->position_penalty==expectedBestPairs.pairs[i].left->position_penalty)){
            stateInfo.setError(GTest::tr("PRIMER_LEFT_PENALTY_%1 is incorrect. Expected:%2, but Actual:%3").arg(i).arg(expectedBestPairs.pairs[i].left->position_penalty).arg(currentBestPairs.pairs[i].left->position_penalty));
            return ReportResult_Finished;
        }
    }
*/

/*    need check error messages
-        PRIMER_PAIR_PENALTY="3.4770"

        PRIMER_LEFT_PENALTY="3.380952"
        PRIMER_LEFT_SEQUENCE="TGACNACTGACGATGCAGA"
        PRIMER_LEFT="15,19"
        PRIMER_LEFT_TM="57.619"
        PRIMER_LEFT_GC_PERCENT="50.000"
        PRIMER_LEFT_SELF_ANY="4.00"
        PRIMER_LEFT_SELF_END="0.00"
        PRIMER_LEFT_END_STABILITY="8.2000"

        PRIMER_RIGHT_PENALTY="0.096021"
        PRIMER_RIGHT_SEQUENCE="ATCGATTTGGGTCGGGAT"
        PRIMER_RIGHT="94,18"
        PRIMER_RIGHT_TM="60.096"
        PRIMER_RIGHT_GC_PERCENT="50.000"
        PRIMER_RIGHT_SELF_ANY="6.00"
        PRIMER_RIGHT_SELF_END="2.00"
        PRIMER_RIGHT_END_STABILITY="9.3000"

        PRIMER_INTERNAL_OLIGO_PENALTY="3.098711"
        PRIMER_INTERNAL_OLIGO_SEQUENCE="GGTATTAGTGGGCCATTCG"
        PRIMER_INTERNAL_OLIGO="58,19"
        PRIMER_INTERNAL_OLIGO_TM="57.901"
        PRIMER_INTERNAL_OLIGO_GC_PERCENT="52.632"
        PRIMER_INTERNAL_OLIGO_SELF_ANY="5.00"
        PRIMER_INTERNAL_OLIGO_SELF_END="2.00"


        PRIMER_PAIR_COMPL_ANY="4.00"
        PRIMER_PAIR_COMPL_END="3.00"
        PRIMER_PRODUCT_SIZE="80"
*/

    return ReportResult_Finished;
}

GTest_Primer3::~GTest_Primer3()
{
}

bool GTest_Primer3::checkPrimerPair(const PrimerPair &primerPair, const PrimerPair &expectedPrimerPair, QString suffix)
{
    if(!checkPrimer(primerPair.getLeftPrimer(), expectedPrimerPair.getLeftPrimer(), "PRIMER_LEFT" + suffix, false))
    {
        return false;
    }
    if(!checkPrimer(primerPair.getRightPrimer(), expectedPrimerPair.getRightPrimer(), "PRIMER_RIGHT" + suffix, false))
    {
        return false;
    }
    if(!checkPrimer(primerPair.getInternalOligo(), expectedPrimerPair.getInternalOligo(), "PRIMER_INTERNAL_OLIGO" + suffix, true))
    {
        return false;
    }
    if(!checkAlignProperty(primerPair.getComplAny(), expectedPrimerPair.getComplAny(), "PRIMER_PAIR" + suffix + "_COMPL_ANY"))
    {
        return false;
    }
    if(!checkAlignProperty(primerPair.getComplEnd(), expectedPrimerPair.getComplEnd(), "PRIMER_PAIR" + suffix + "_COMPL_END"))
    {
        return false;
    }
    if(!checkIntProperty(primerPair.getProductSize(), expectedPrimerPair.getProductSize(), "PRIMER_PRODUCT_SIZE" + suffix))
    {
        return false;
    }
    return true;
}

bool GTest_Primer3::checkPrimer(const Primer *primer, const Primer *expectedPrimer, QString prefix, bool internalOligo)
{
    if(NULL == primer)
    {
        if(NULL == expectedPrimer)
        {
            return true;
        }
        else
        {
            stateInfo.setError(GTest::tr("%1 is incorrect. Expected:%2,%3, but Actual:NULL")
                .arg(prefix)
                .arg(expectedPrimer->getStart())
                .arg(expectedPrimer->getLength()));
            return false;
        }
    }
    if(NULL == expectedPrimer)
    {
        stateInfo.setError(GTest::tr("%1 is incorrect. Expected:NULL, but Actual:%2,%3")
            .arg(prefix)
            .arg(primer->getStart())
            .arg(primer->getLength()));
        return false;
    }
    {
        if((primer->getStart() + settings.getFirstBaseIndex() != expectedPrimer->getStart()) ||
           (primer->getLength() != expectedPrimer->getLength()))
        {
            stateInfo.setError(GTest::tr("%1 is incorrect. Expected:%2,%3, but Actual:%4,%5")
                .arg(prefix)
                .arg(expectedPrimer->getStart() + settings.getFirstBaseIndex())
                .arg(expectedPrimer->getLength())
                .arg(primer->getStart())
                .arg(primer->getLength()));
            return false;
        }
    }
    if(!checkDoubleProperty(primer->getMeltingTemperature(), expectedPrimer->getMeltingTemperature(), prefix + "_TM"))
    {
        return false;
    }
    if(!checkDoubleProperty(primer->getGcContent(), expectedPrimer->getGcContent(), prefix + "_GC_PERCENT"))
    {
        return false;
    }
    if(!checkAlignProperty(primer->getSelfAny(), expectedPrimer->getSelfAny(), prefix + "_SELF_ANY"))
    {
        return false;
    }
    if(!checkAlignProperty(primer->getSelfEnd(), expectedPrimer->getSelfEnd(), prefix + "_SELF_END"))
    {
        return false;
    }
    if(!internalOligo)
    {
        if(!checkDoubleProperty(primer->getEndStabilyty(), expectedPrimer->getEndStabilyty(), prefix + "_END_STABILITY"))
        {
            return false;
        }
    }
    return true;
}

bool GTest_Primer3::checkIntProperty(int value, int expectedValue, QString name)
{
    if(value != expectedValue)
    {
        stateInfo.setError(GTest::tr("%1 is incorrect. Expected:%2, but Actual:%3").arg(name).arg(expectedValue).arg(value));
        return false;
    }
    return true;
}

bool GTest_Primer3::checkDoubleProperty(double value, double expectedValue, QString name)
{
    if(qAbs(value - expectedValue) > qAbs(value/1000))
    {
        stateInfo.setError(GTest::tr("%1 is incorrect. Expected:%2, but Actual:%3").arg(name).arg(expectedValue).arg(value));
        return false;
    }
    return true;
}

bool GTest_Primer3::checkAlignProperty(short value, short expectedValue, QString name)
{
    if(value != expectedValue)
    {
        stateInfo.setError(GTest::tr("%1 is incorrect. Expected:%2, but Actual:%3").arg(name).arg((double)expectedValue/100).arg((double)value/100));
        return false;
    }
    return true;
}

} // namespace
