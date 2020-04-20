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

#include "WeightMatrixIO.h"
#include "WeightMatrixPlugin.h"

#include <U2Core/AppContext.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>

#include <U2Core/TextUtils.h>
#include <U2Core/SaveDocumentTask.h>

#include <U2Gui/DialogUtils.h>

#include <U2Core/DIProperties.h>

#include <QVector>
#include <QTextStream>
#include <QFile>

#include <QMessageBox>

/* TRANSLATOR U2::IOAdapter */

namespace U2 {

const QString WeightMatrixIO::WEIGHT_MATRIX_ID("weight_matrix");
const QString WeightMatrixIO::FREQUENCY_MATRIX_ID("frequency_matrix");
const QString WeightMatrixIO::WEIGHT_MATRIX_EXT("pwm");
const QString WeightMatrixIO::FREQUENCY_MATRIX_EXT("pfm");

QString WeightMatrixIO::getAllMatrixFileFilter(bool includeAll) {
    return DialogUtils::prepareFileFilter(tr("Frequency and weight matrices"), QStringList() << FREQUENCY_MATRIX_EXT << WEIGHT_MATRIX_EXT, includeAll);
}

QString WeightMatrixIO::getPFMFileFilter(bool includeAll) {
    return DialogUtils::prepareFileFilter(tr("Frequency matrices"), QStringList(FREQUENCY_MATRIX_EXT), includeAll);
}

QString WeightMatrixIO::getPWMFileFilter(bool includeAll) {
    return DialogUtils::prepareFileFilter(tr("Weight matrices"), QStringList(WEIGHT_MATRIX_EXT), includeAll);
}

#define BUFF_SIZE 4096
#define MATRIX_VAL_SEPARATOR ';'

PFMatrix WeightMatrixIO::readPFMatrix(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si) {
    PFMatrix matrix;
    QVarLengthArray<int> res;
    int len = -1, msize = 0;

    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Read)) {
        si.setError(  L10N::errorOpeningFileRead(url) );
        return matrix;
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
        return matrix;
    }

    QTextStream reader(text);
    QString line;

    while (!reader.atEnd() && !si.hasError()) {
        line = reader.readLine();
        if (line.isEmpty()) {
            continue;
        }

        QStringList curr = line.split(" ", QString::SkipEmptyParts);

        if (len == -1) {
            len = curr.length();
        }

        if (len != curr.length()) {
            si.setError(tr("Error parsing settings line %1").arg(line) );
            break;
        }

        for (int i = 0; i < len; i++) {
            bool ok;
            int val = curr[i].toInt(&ok);
            if (!ok) {
                si.setError(tr("Error parsing value %1").arg(curr[i]) );
                break;
            }
            if (val < 0) {
                si.setError(tr("Unexpected negative frequency value %1").arg(val) );
                break;
            }
            res.append(val);
        }

        msize++;
    }

    if (si.hasError()) {
        return matrix;
    }

    if (msize != 4 && msize != 16) {
        si.setError(tr("Incorrect size of weight matrix: %1").arg(msize));
        return matrix;
    }

    matrix = PFMatrix(res, (msize == 4) ? PFM_MONONUCLEOTIDE : PFM_DINUCLEOTIDE);

    QStringList splitUrl = url.split("/");
    QString name = splitUrl.last();
    splitUrl.removeLast();
    splitUrl.append(QString("matrix_list.txt"));
    QString jasparBasePath = splitUrl.join("/");
    if (!QFile::exists(jasparBasePath)) {
        return matrix;
    }
    int pos = name.lastIndexOf(".");
    name.remove(pos, name.length() - pos);
    QFile jasparBase(jasparBasePath);
    jasparBase.open(QIODevice::ReadOnly);
    bool found = false;
    while (!found && !jasparBase.atEnd()) {
        QString curr = QString(jasparBase.readLine());
        if (!curr.startsWith(name)) continue;
        found = true;
        JasparInfo info(curr);
        matrix.setInfo(info);
    }
    jasparBase.close();
    return matrix;
}

PWMatrix WeightMatrixIO::readPWMatrix(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si) {
    PWMatrix matrix;
    QVarLengthArray<float> res;
    int len = -1, msize = 0;

    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Read)) {
        si.setError(  L10N::errorOpeningFileRead(url) );
        return matrix;
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
        return matrix;
    }

    QTextStream reader(text);
    QString line;

    while (!reader.atEnd() && !si.hasError()) {
        line = reader.readLine();
        if (line.isEmpty()) {
            continue;
        }
        if (line.indexOf(":") > 2 || line.indexOf(":") < 0) {
            UniprobeInfo info(line);
            matrix.setInfo(info);
            continue;
        }

        QStringList curr = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (len == -1) {
            len = curr.length() - 1;
        }

        if (len != curr.length() - 1) {
            si.setError(tr("Error parsing settings line %1").arg(line) );
            break;
        }

        for (int i = 1; i <= len; i++) {
            bool ok;
            QString tmp = curr[i];
            float val = tmp.toFloat(&ok);
            if (!ok) {
                si.setError(tr("Error parsing value %1").arg(tmp));
                break;
            }
            res.append(val);
        }
        msize++;
    }

    if (si.hasError()) {
        return matrix;
    }

    if (msize != 4 && msize != 16) {
        si.setError(tr("Incorrect size of weight matrix: %1").arg(msize));
        return matrix;
    }

    matrix = PWMatrix(res, (msize == 4) ? PWM_MONONUCLEOTIDE : PWM_DINUCLEOTIDE);
    return matrix;
}

void WeightMatrixIO::writePFMatrix(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si, const PFMatrix& model)
{
    assert (model.getLength() >= 0);
    QByteArray res;
    int size = (model.getType() == PFM_MONONUCLEOTIDE) ? 4 : 16;
    for (int i = 0; i < size; i++) {
        for (int j = 0, n = model.getLength(); j < n; j++) {
            res.append(QString("%1").arg(model.getValue(i, j), 4));
        }
        res.append("\n");
    }

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

void WeightMatrixIO::writePWMatrix(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si, const PWMatrix& model)
{
    assert (model.getLength() >= 0);
    QByteArray res;
    int size = (model.getType() == PWM_MONONUCLEOTIDE) ? 4 : 16;
    for (int i = 0; i < size; i++) {
        if (model.getType() == PWM_MONONUCLEOTIDE) {
            res.append(DiProperty::fromIndex(i));
            res.append(":   ");
        } else {
            res.append(DiProperty::fromIndexHi(i));
            res.append(DiProperty::fromIndexLo(i));
            res.append(":  ");
        }
        for (int j = 0, n = model.getLength(); j < n; j++) {
            res.append(QString("%1").arg(model.getValue(i, j), -20, 'f', 15));
        }
        res.append("\n");
    }

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


void PFMatrixReadTask::run() {
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    model = WeightMatrixIO::readPFMatrix(iof, url, stateInfo);
}

void PFMatrixWriteTask::run() {
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    if (fileMode & SaveDoc_Roll && !GUrlUtils::renameFileWithNameRoll(url, stateInfo)) {
        return;
    }
    WeightMatrixIO::writePFMatrix(iof, url, stateInfo, model);
}

void PWMatrixReadTask::run() {
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    model = WeightMatrixIO::readPWMatrix(iof, url, stateInfo);
}

void PWMatrixWriteTask::run() {
    IOAdapterFactory* iof = AppContext::getIOAdapterRegistry()->getIOAdapterFactoryById(IOAdapterUtils::url2io(url));
    if (fileMode & SaveDoc_Roll && !GUrlUtils::renameFileWithNameRoll(url, stateInfo)) {
        return;
    }
    WeightMatrixIO::writePWMatrix(iof, url, stateInfo, model);
}

}//namespace
