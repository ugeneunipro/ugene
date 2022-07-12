/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include <QFile>
#include <QVector>

#include <U2Core/AppContext.h>
#include <U2Core/DIProperties.h>
#include <U2Core/FileFilters.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/IOAdapterTextStream.h>
#include <U2Core/IOAdapterUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/SaveDocumentTask.h>
#include <U2Core/TextUtils.h>

#include "WeightMatrixPlugin.h"

namespace U2 {

const QString WeightMatrixIO::WEIGHT_MATRIX_ID("weight_matrix");
const QString WeightMatrixIO::FREQUENCY_MATRIX_ID("frequency_matrix");
const QString WeightMatrixIO::WEIGHT_MATRIX_EXT("pwm");
const QString WeightMatrixIO::FREQUENCY_MATRIX_EXT("pfm");

QString WeightMatrixIO::getAllMatrixFileFilter(bool isSingleFileFilterMode) {
    QString name = tr("Frequency and weight matrices");
    QStringList extensions({FREQUENCY_MATRIX_EXT, WEIGHT_MATRIX_EXT});
    return isSingleFileFilterMode ? FileFilters::createSingleFileFilter(name, extensions, false)
                                  : FileFilters::createFileFilter(name, extensions);
}

QString WeightMatrixIO::getPFMFileFilter(bool isSingleFileFilterMode) {
    QString name = tr("Frequency matrices");
    QStringList extensions({FREQUENCY_MATRIX_EXT});
    return isSingleFileFilterMode ? FileFilters::createSingleFileFilter(name, extensions, false)
                                  : FileFilters::createFileFilter(name, extensions);
}

QString WeightMatrixIO::getPWMFileFilter() {
    return FileFilters::createFileFilter(tr("Weight matrices"), {WEIGHT_MATRIX_EXT});
}

#define BUFF_SIZE 4096
#define MATRIX_VAL_SEPARATOR ';'

PFMatrix WeightMatrixIO::readPFMatrix(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si) {
    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Read)) {
        si.setError(L10N::errorOpeningFileRead(url));
        return {};
    }
    IOAdapterReader reader(io.data());
    return readPFMatrix(reader, si);
}

PFMatrix WeightMatrixIO::readPFMatrix(IOAdapterReader& reader, TaskStateInfo& si) {
    QString url = reader.getURL().getURLString();
    PFMatrix matrix;
    QVarLengthArray<int> res;
    int len = -1;
    int matrixSize = 0;

    while (!reader.atEnd() && matrixSize <= 16) {
        QString line = reader.readLine(si, 10000);
        CHECK_OP(si, {});
        if (line.isEmpty()) {
            continue;
        }

        QStringList curr = line.split(" ", QString::SkipEmptyParts);

        if (len == -1) {
            len = curr.length();
        }

        if (len != curr.length()) {
            si.setError(tr("Error parsing settings line %1").arg(line));
            break;
        }

        for (int i = 0; i < len; i++) {
            bool ok;
            int val = curr[i].toInt(&ok);
            if (!ok) {
                si.setError(tr("Error parsing value %1").arg(curr[i]));
                break;
            }
            if (val < 0) {
                si.setError(tr("Unexpected negative frequency value %1").arg(val));
                break;
            }
            res.append(val);
        }

        matrixSize++;
    }
    CHECK_OP(si, {});

    if (matrixSize != 4 && matrixSize != 16) {
        si.setError(tr("Incorrect size of weight matrix: %1").arg(matrixSize));
        return matrix;
    }

    matrix = PFMatrix(res, (matrixSize == 4) ? PFM_MONONUCLEOTIDE : PFM_DINUCLEOTIDE);

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
        if (!curr.startsWith(name))
            continue;
        found = true;
        JasparInfo info(curr);
        matrix.setInfo(info);
    }
    jasparBase.close();
    return matrix;
}

PWMatrix WeightMatrixIO::readPWMatrix(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si) {
    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Read)) {
        si.setError(L10N::errorOpeningFileRead(url));
        return {};
    }
    IOAdapterReader reader(io.data());
    return readPWMatrix(reader, si);
}

PWMatrix WeightMatrixIO::readPWMatrix(IOAdapterReader& reader, TaskStateInfo& si) {
    QString url = reader.getURL().getURLString();
    PWMatrix matrix;
    QVarLengthArray<float> res;
    int len = -1;
    int matrixSize = 0;

    while (!reader.atEnd() && matrixSize <= 16) {
        QString line = reader.readLine(si, 10000);
        CHECK_OP(si, {});
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
            si.setError(tr("Error parsing settings line %1").arg(line));
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
        matrixSize++;
    }
    CHECK_OP(si, {});

    if (matrixSize != 4 && matrixSize != 16) {
        si.setError(tr("Incorrect size of weight matrix: %1").arg(matrixSize));
        return matrix;
    }

    matrix = PWMatrix(res, matrixSize == 4 ? PWM_MONONUCLEOTIDE : PWM_DINUCLEOTIDE);
    return matrix;
}

void WeightMatrixIO::writePFMatrix(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si, const PFMatrix& model) {
    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Write)) {
        si.setError(L10N::errorOpeningFileWrite(url));
        return;
    }
    IOAdapterWriter writer(io.data());
    writePFMatrix(writer, si, model);
}

void WeightMatrixIO::writePFMatrix(IOAdapterWriter& writer, TaskStateInfo& si, const PFMatrix& model) {
    SAFE_POINT(model.getLength() >= 0, "Model is empty", );
    QString res;
    int size = model.getType() == PFM_MONONUCLEOTIDE ? 4 : 16;
    for (int i = 0; i < size; i++) {
        for (int j = 0, n = model.getLength(); j < n; j++) {
            res.append(QString("%1").arg(model.getValue(i, j), 4));
        }
        res.append("\n");
    }
    writer.write(si, res);
}

void WeightMatrixIO::writePWMatrix(IOAdapterFactory* iof, const QString& url, TaskStateInfo& si, const PWMatrix& model) {
    QScopedPointer<IOAdapter> io(iof->createIOAdapter());
    if (!io->open(url, IOAdapterMode_Write)) {
        si.setError(L10N::errorOpeningFileWrite(url));
        return;
    }
    IOAdapterWriter writer(io.data());
    writePWMatrix(writer, si, model);
}

void WeightMatrixIO::writePWMatrix(IOAdapterWriter& writer, TaskStateInfo& si, const PWMatrix& model) {
    SAFE_POINT(model.getLength() >= 0, "Model is empty", );
    QString res;
    int size = model.getType() == PWM_MONONUCLEOTIDE ? 4 : 16;
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
    writer.write(si, res);
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

}  // namespace U2
