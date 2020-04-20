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

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QVector>
#include <QtMath>

#include <U2Core/AppContext.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TextUtils.h>

#include <U2Gui/DialogUtils.h>

#include "DIPropertiesSitecon.h"
#include "SiteconIO.h"
#include "SiteconMath.h"
#include "SiteconPlugin.h"

/* TRANSLATOR U2::IOAdapter */

namespace U2 {

const QString SiteconIO::SITECON_ID("sitecon");
const QString SiteconIO::SITECON_EXT = SiteconIO::SITECON_ID;
QString SiteconIO::getFileFilter(bool includeAll) {
    return DialogUtils::prepareFileFilter(tr("Sitecon models"), QStringList(SITECON_EXT), includeAll);
}


#define FILE_HEADER     "SITECON MODEL"
#define SETTINGS_HEADER "SETTINGS"
#define AVE_HEADER      "AVERAGE MATRIX"
#define SDEV_HEADER     "SDEVIATION MATRIX"
#define WEIGHT_HEADER   "WEIGHT MATRIX"
#define ERR1_HEADER     "ERROR_TYPE_1"
#define ERR2_HEADER     "ERROR_TYPE_2"
#define END_MARKER      "//"

#define BUFF_SIZE 4096
#define MATRIX_VAL_SEPARATOR ';'

SiteconModel SiteconIO::readModel(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si) {
    SiteconModel model;
    model.modelName = QFileInfo(url).baseName();

    QList<DiPropertySitecon*> props = SiteconPlugin::getDinucleotiteProperties();
    model.settings.props = props;

    float defaultVal = -1;
    model.err1.fill(defaultVal, 100);
    model.err2.fill(defaultVal, 100);

    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Read)) {
        si.setError(  L10N::errorOpeningFileRead(url) );
        return model;
    }
    QByteArray text;
    int size = io->left();
    if (size > 0) {
        text.reserve(size);
    }

    QByteArray block(BUFF_SIZE, '\0');
    qint64 blockLen = 0;
    while ((blockLen = io->readBlock(block.data(), BUFF_SIZE)) > 0) {
        text.append(QByteArray(block.data(), blockLen));
        if (text.size() > 1000*1000) {
            si.setError(L10N::errorFileTooLarge(url));
            break;
        }
    }
    io->close();
    if (si.hasError()) {
        return model;
    }

    QTextStream reader(text);
    enum STATE {FILE_S, SETTINGS_S, AVE_S, SDEV_S, WEIGHT_S, ERR1_S, ERR2_S};
    STATE state = FILE_S;
    QString line = reader.readLine();
    if (line!=FILE_HEADER) {
        si.setError(  tr("Not a sitecon model") );
        return model;
    }
    QSet<int> passedPropsAve;
    QSet<int> passedPropsSDev;
    QSet<int> passedPropsWeight;

    while (!reader.atEnd() && !si.hasError()) {
        line = reader.readLine();
        if (line.isEmpty()) {
            continue;
        } 
        
        if (line == SETTINGS_HEADER) {
            state = SETTINGS_S;
            continue;
        } else if (line == AVE_HEADER) {
            state = AVE_S;
            continue;
        } else if (line == SDEV_HEADER) {
            state = SDEV_S;
            continue;
        } else if (line == WEIGHT_HEADER) {
            state = WEIGHT_S;
            continue;
        } else  if (line == ERR1_HEADER) {
            state = ERR1_S;
            continue;
        } else if (line == ERR2_HEADER) {
            state = ERR2_S;
            continue;
        } else if (line == END_MARKER) {
            break;
        } 
        switch(state) {
            case FILE_S:
                {
                    model.description += line + '\n';
                }
                break;
            case SETTINGS_S:
                {
                    int i = line.indexOf(' ');
                    if (i == -1) {
                        si.setError(  tr("Error parsing settings, line %1").arg(line) );
                        break;
                    }
                    QString name = line.left(i);
                    QString val = line.mid(i+1);
                    bool ok;
                    if (name == "ORIGIN") {
                        model.aliURL = val;
                    } else if (name == "W") {
                        model.settings.windowSize = val.toInt(&ok);
                        if (!ok) {
                            si.setError(  tr("Error parsing window size: %1").arg(line) );
                            break;
                        } 
                        model.matrix.resize(model.settings.windowSize-1);
                        for (int pos = 0; pos < model.matrix.size(); pos++) {
                            PositionStats& pList = model.matrix[pos];
                            for (int p=0; p < props.size(); p++) {
                                pList.append(DiStat(props[p], 0, 0));
                            }
                        }
                    } else if (name == "CLEN") {
                        model.settings.secondTypeErrorCalibrationLen = val.toInt(&ok);
                        if (!ok) {
                            si.setError(  tr("Error parsing calibration len: %1").arg(line) );
                            break;
                        } 
                    } else if (name == "RSEED") {
                        model.settings.randomSeed = val.toInt(&ok);
                        if (!ok) {
                            si.setError(  tr("Error parsing RSEED: %1").arg(line) );
                            break;
                        } 
                    } else if (name == "NSEQ") {
                        model.settings.numSequencesInAlignment = val.toInt(&ok);
                        if (!ok) {
                            si.setError(  tr("Error parsing number of sequence in original alignment: %1").arg(line) );
                            break;
                        } 
                    } else if (name == "WALG") {
                        int alg = val.toInt(&ok);
                        if (!ok) {
                            si.setError(  tr("Error parsing in alignment %1").arg(line) );
                            break;
                        } 
                        if (alg == 0) {
                            model.settings.weightAlg = SiteconWeightAlg_None;
                        } else if (alg == 2) {
                            model.settings.weightAlg = SiteconWeightAlg_Alg2;
                        } else {
                            si.setError(  tr("Illegal weight algorithm: %1").arg(line) );
                            break;
                        }
                    }
                }
                break;
            case AVE_S:
            case SDEV_S:
            case WEIGHT_S:
                {
                    QStringList l = line.split(MATRIX_VAL_SEPARATOR);
                    if (l.size()-2 !=  model.settings.windowSize - 1) { //num values == size-2, modelLen = w-1
                        si.setError(  tr("Model size not matched: %1, expected: %2").arg(l.size()-2).arg(model.settings.windowSize-1) );
                        break;
                    }
                    QString propNum = l.first();
                    //compute property idx
                    int idx = -1;
                    for (int i=0 ; i < props.size(); i++) {
                        const DiPropertySitecon* p = props[i];
                        QString thisPNum =  p->keys.value("MI");
                        if (thisPNum == propNum) {
                            idx = i;
                            bool duplicate = false;
                            if (state == AVE_S) {
                                duplicate = passedPropsAve.contains(idx);
                                passedPropsAve.insert(idx);
                            } else if (state == SDEV_S) {
                                duplicate = passedPropsSDev.contains(idx);
                                passedPropsSDev.insert(idx);
                            } else {
                                duplicate = passedPropsWeight.contains(idx);
                                passedPropsWeight.insert(idx);
                            }
                            if (duplicate) {
                                si.setError(  tr("Duplicate property: %1").arg(propNum) );
                                break;
                            }
                            break;
                        }
                    }
                    if (si.hasError()) {
                        break;
                    }
                    if (idx == -1) {
                        si.setError(  tr("Property not recognized: %1").arg(line) );
                        break;
                    }
                    //setup position specific value for property
                    bool ok = true;
                    for (int i = 2; i < l.size(); i++) {
                        const QString& valStr  = l[i];
                        PositionStats& ps  = model.matrix[i-2];
                        float fval = 0;
                        bool  wval = false;
                        if (state == WEIGHT_S) {
                            wval = valStr.trimmed()=="1";
                        } else {
                            fval = (float)valStr.trimmed().toDouble(&ok);
                        }
                        if (!ok) {
                            si.setError(  tr("Error parsing %1 in line %2").arg(valStr).arg(line) );
                            break;
                        }
                        if (state == AVE_S) {
                            ps[idx].average = fval;
                        } else if (state == SDEV_S) {
                            ps[idx].sdeviation = fval;
                        } else {
                            ps[idx].weighted = wval;
                        }
                    }
                }
                break;
            case ERR1_S:
            case ERR2_S:
                {
                    QStringList l = line.split(' ');
                    if (l.size() != 2) {
                        si.setError(  tr("Error parsing in line %1").arg(line) );
                        break;
                    }
                    bool ok = true;
                    const QString& percentStr = l[0];
                    const QString& errStr = l[1];
                    // TODO: what purpose is this code needed for? Why does UGENE work without it?
                    // percentStr.left(percentStr.length()-1);
                    int p = percentStr.toInt(&ok);
                    if (!ok) {
                        si.setError(  tr("Error parsing %1").arg(line) );
                        break;
                    }
                    float e = (float)errStr.toDouble(&ok);
                    if (!ok) {
                        si.setError(  tr("Error parsing %1").arg(line) );
                        break;
                    }
                    if (p < 0 || p >= 100) {
                        si.setError(  tr("Illegal error %1").arg(line) );
                        break;
                    }
                    if (state == ERR1_S) {
                        model.err1[p] = e;
                    } else {
                        model.err2[p] = e;
                    }
                }
                break;
            default: si.setError(  tr("Error parsing file in line %1").arg(line) );
        }
    }
    
    if (si.hasError()) {
        return model;
    }

    if (passedPropsAve.size() != passedPropsSDev.size()) {
        si.setError(  tr("Number of 'average' and  'sdev' properties not matches").arg(passedPropsAve.size()).arg(passedPropsSDev.size()) );
        return model;
    }
    
    if (passedPropsAve.size() != passedPropsWeight.size()) {
        si.setError(  tr("Number of 'average' and 'weight' properties not matches").arg(passedPropsAve.size()).arg(passedPropsWeight.size()) );
        return model;
    }
    
    if (passedPropsAve.size() != props.size()) {
        si.setError(  tr("Property in file %1 is not the same as built-in: %2").arg(passedPropsAve.size()).arg(props.size()) );
        return model;
    }
    
    //make error list complete -> add default values
    for(int i = 100; --i >=0 && model.err1[i] == defaultVal;) {
        model.err1[i] = 1;
    }
    for(int i = 100; --i >=0 && model.err2[i] == defaultVal;) {
        model.err2[i] = 0;
    
    }
    float prevVal1 = 0;
    float prevVal2 = 1;
    for(int i = 0; i < 100; i++) {
        if (model.err1[i] == defaultVal) {
            model.err1[i] = prevVal1;
        } else {
            prevVal1 = model.err1[i];
        }
        if (model.err2[i] == defaultVal) {
            model.err2[i] = prevVal2;
        } else {
            prevVal2 = model.err2[i];
        }
    }

    if (model.err1.contains(defaultVal) || model.err2.contains(defaultVal)) {
        si.setError(  tr("Error info in file is not complete") );
        return model;
    }
    model.deviationThresh = (float)critchi(model.settings.chisquare, model.settings.numSequencesInAlignment - 1) / model.settings.numSequencesInAlignment;

    bool ok = model.checkState(false);
    if(!ok) {
        si.setError(  tr("Model verification error") );
    } 
    return model;
}

static bool eq(float v1, float v2, int degree) {
    float d1 = qAbs(v1-v2);
    float d2 = (float)qPow(10, -1*degree);
    return (d1 < d2);
}

void SiteconIO::writeModel(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si, const SiteconModel& model) 
{
    model.checkState();
    QByteArray res;
    res.append(FILE_HEADER).append("\n\n");
    assert(model.settings.windowSize == model.matrix.size() + 1);

    const QList<DiPropertySitecon*>& props = model.settings.props;
    int nProps = props.size();
    QVector<QByteArray> aves, sdev, wght;
    aves.resize(nProps);
    sdev.resize(nProps);
    wght.resize(nProps);

    for(int j = 0; j < nProps; j++) {
        const DiPropertySitecon* p = props[j];
        QString name = p->keys.value("PV");
        QString index = p->keys.value("MI");
        aves[j].append(index).append(MATRIX_VAL_SEPARATOR).append(name);
        sdev[j].append(index).append(MATRIX_VAL_SEPARATOR).append(name);
        wght[j].append(index).append(MATRIX_VAL_SEPARATOR).append(name);
    }
    for (int i=0; i < model.matrix.size(); i++) {
        const PositionStats& posM = model.matrix[i];
        assert(posM.size() == nProps);
        for(int j = 0; j < nProps; j++) {
            const DiStat& ds = posM[j];
            assert(ds.prop == props[j]);
            QString aveVal = QByteArray::number(ds.average, 'f', 4);
            QString devVal = QByteArray::number(ds.sdeviation, 'e', 4);
            QString wgtVal = ds.weighted ? "1" : "0";
            int aveEnd = aves[j].length() + 1 + aveVal.length();
            int devEnd = sdev[j].length() + 1 + devVal.length();
            int wgtEnd = wght[j].length() + 1 + wgtVal.length();
            const char* aliAve = TextUtils::getLineOfSpaces(aveEnd < 35 && i==0 ? 35 - aveEnd : 14 -(aveEnd-35)%15);
            const char* aliDev = TextUtils::getLineOfSpaces(devEnd < 35 && i==0 ? 35 - devEnd : 14 -(devEnd-35)%15);
            const char* aliWgt = TextUtils::getLineOfSpaces(wgtEnd < 35 && i==0 ? 35 - wgtEnd : (wgtEnd-35)%2 );
            aves[j].append(MATRIX_VAL_SEPARATOR).append(aliAve).append(aveVal);
            sdev[j].append(MATRIX_VAL_SEPARATOR).append(aliDev).append(devVal);
            wght[j].append(MATRIX_VAL_SEPARATOR).append(aliWgt).append(wgtVal);
        }
    }
    
    res.append('\n').append(SETTINGS_HEADER).append("\n");
    res.append("ORIGIN ").append(model.aliURL).append('\n');
    res.append("W ").append(QByteArray::number(model.settings.windowSize)).append('\n');
    res.append("SEED ").append(QByteArray::number(model.settings.randomSeed)).append('\n');
    res.append("CLEN ").append(QByteArray::number(model.settings.secondTypeErrorCalibrationLen)).append('\n');
    res.append("NSEQ ").append(QByteArray::number(model.settings.numSequencesInAlignment)).append('\n');
    res.append("WALG ").append(model.settings.weightAlg == SiteconWeightAlg_None ? "0" : "2").append('\n');

    res.append('\n').append(AVE_HEADER).append("\n");
    foreach(const QByteArray& a, aves) {
        res.append(a).append('\n');
    }
    

    res.append('\n').append(SDEV_HEADER).append('\n');
    foreach(const QByteArray& d, sdev) {
        res.append(d).append('\n');
    }

    res.append('\n').append(WEIGHT_HEADER).append('\n');
    foreach(const QByteArray& w, wght) {
        res.append(w).append('\n');
    }

    res.append('\n').append(ERR1_HEADER).append("\n");
    for (int i=0; i < model.err1.size(); i++) {
        if (eq(model.err1[i], 0, 6) || eq(model.err1[i], 1, 7)) {
            continue;
        }
        res.append(QByteArray::number(i)).append(' ');
        res.append(QByteArray::number(model.err1[i], 'f', 4)).append('\n');
    }

    res.append('\n').append(ERR2_HEADER).append("\n");
    for (int i=0; i < model.err2.size(); i++) {
        if (eq(model.err2[i], 0, 6) || eq(model.err2[i], 1, 7)) {
            continue;
        }
        res.append(QByteArray::number(i)).append(' ');
        res.append(QByteArray::number(model.err2[i], 'e', 4)).append('\n');
    }

    res.append('\n').append(END_MARKER);

    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Write)) {
        si.setError(  L10N::errorOpeningFileWrite(url) );
        return;
    }
    int len = io->writeBlock(res);
    if (len != res.size()) {
        si.setError(  L10N::errorWritingFile(url) );
        return;
    }
    io->close();
}

void SiteconReadTask::run() {
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    model = SiteconIO::readModel(iof, url, stateInfo);
}

void SiteconWriteTask::run() {
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    if (fileMode & SaveDoc_Roll && !GUrlUtils::renameFileWithNameRoll(url, stateInfo)) {
        return;
    }
    SiteconIO::writeModel(iof, url, stateInfo, model);
}

}//namespace
