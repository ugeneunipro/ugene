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

#include "SubstMatrixRegistry.h"

#include <U2Core/Log.h>
#include <U2Core/AppContext.h>
#include <U2Core/U2AlphabetUtils.h>

#include <QDir>

namespace U2 {

SubstMatrixRegistry::SubstMatrixRegistry(QObject* pOwn)
: QObject(pOwn), mutex(QMutex::Recursive)
{
    readMatrices();
}

SMatrix SubstMatrixRegistry::getMatrix(const QString& name) {
    QMutexLocker lock(&mutex);
    return matrixByName.value(name);
}

QList<SMatrix> SubstMatrixRegistry::getMatrices() const {
    QMutexLocker lock(&mutex);
    return matrixByName.values();
}

QStringList SubstMatrixRegistry::getMatrixNames() const {
    QMutexLocker lock(&mutex);
    QStringList result;
    foreach(const SMatrix& m, matrixByName.values()) {
        result.append(m.getName());
    }
    return result;
}

QList<SMatrix> SubstMatrixRegistry::selectMatricesByAlphabet(const DNAAlphabet* al) const {
    QMutexLocker lock(&mutex);
    QList<SMatrix> result;
    foreach(const SMatrix& m, getMatrices()) {
        if (m.getAlphabet() == al) {
            result.append(m);
        }
    }
    return result;
}

QStringList SubstMatrixRegistry::selectMatrixNamesByAlphabet(const DNAAlphabet* al) const {
    QMutexLocker lock(&mutex);
    QStringList result;
    foreach(const SMatrix& m, matrixByName.values()) {
        const DNAAlphabet* mAlpha = m.getAlphabet();
        if (al->getType() == mAlpha->getType() && al->getNumAlphabetChars() <= mAlpha->getNumAlphabetChars()) {
            QByteArray aChars = al->getAlphabetChars(), mChars = mAlpha->getAlphabetChars();
            bool addToResult = true;
            foreach(char c, aChars){
                if(!mChars.contains(c)){
                    addToResult = false;
                    break;
                }
            }
            if(addToResult){
                result.append(m.getName());
            }
        }
    }
    result.sort();
    return result;
}

void SubstMatrixRegistry::registerMatrix(const SMatrix& m) {
    assert(!m.isEmpty());
    QMutexLocker lock(&mutex);
    matrixByName[m.getName()] = m;
}


void SubstMatrixRegistry::readMatrices() {
    QString builtInMatrixDir =  QDir::searchPaths( PATH_PREFIX_DATA ).first() + "/weight_matrix";
    QStringList ls = QDir(builtInMatrixDir).entryList(QStringList("*.txt"));
    for (int i = 0; i < ls.size(); i++) {
        const QString& fileName = ls.at(i);
        QFileInfo fi(builtInMatrixDir + "/" + fileName);
        coreLog.trace(tr("Reading substitution matrix from %1").arg(fi.canonicalFilePath()));
        QString error;
        SMatrix matrix = readMatrixFromFile(fi.canonicalFilePath(), error);
        if (!matrix.isEmpty()) {
            assert(error.isEmpty());
            registerMatrix(matrix);
        } else {
            assert(!error.isEmpty());
            coreLog.error(tr("Error weight matrix file '%1' : %2").arg(fi.canonicalFilePath()).arg(error));
        }
    }
}

SMatrix SubstMatrixRegistry::readMatrixFromFile(const QString& fileName, QString& error) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        error = tr("Error opening file for read: %1").arg(fileName);
        return SMatrix();
    }
    int fileSize = (int)file.size();
    QByteArray data(fileSize, 0);
    int readSize = file.read(data.data(), fileSize);
    if (readSize!=fileSize) {
        error = tr("Error reading file: %1").arg(fileName);
        return SMatrix();
    }
    return parseMatrix(QFileInfo(fileName).completeBaseName(), data, error);
}

SMatrix SubstMatrixRegistry::parseMatrix(const QString& name, const QByteArray& text, QString& error) {
    QList<QByteArray> lines = text.split('\n');

    QByteArray alphaRow;
    QList<SScore> charScores;
    QString description;
    const DNAAlphabet* alphabet = NULL;
    QByteArray mappedAlphas; //cache of mapped characters. Used to check that no character is mapped twice
    // put comments into description
    for (int i=0; i < lines.length(); i++) {
        QString line = lines.at(i).trimmed();
        if (line.isEmpty()) {
            continue;
        }
        if (line.startsWith("#")) {
            QString commentLine = line.mid(1).trimmed();
            if (line.isEmpty() && commentLine.isEmpty()) {
                continue; //skip first empty lines
            }
            description += commentLine + "\n";
            continue;
        }
        if (alphaRow.isEmpty()) {
            // this is the first row with symbols
            QStringList tokens = line.simplified().split(' ');
            foreach (const QString& token, tokens) {
                if (token.length() != 1) {
                    error = tr("Invalid character token '%1' , line %2").arg(token).arg(i+1);
                    return SMatrix();
                }
                char c = token.at(0).toUpper().toLatin1();
                if (alphaRow.contains(c)) {
                    error = tr("Duplicate character '%1' , line %2").arg(token).arg(i+1);
                    return SMatrix();
                }
                alphaRow.append(c);
            }
            alphabet = U2AlphabetUtils::findBestAlphabet(alphaRow);
            if (!alphabet->isAmino() && !alphabet->isNucleic()) {
                error = tr("Alphabet is neither nucleic nor protein!");
                return SMatrix();
            }
            if (!alphaRow.contains(alphabet->getDefaultSymbol())) {
                error = tr("Matrix doesn't contains score for default alphabet character '%1'").arg(alphabet->getDefaultSymbol());
                return SMatrix();
            }
        } else {
            // this is the [second +] row with scores
            QStringList tokens = line.simplified().split(' ');
            QString token = tokens.at(0);
            if (token.length() !=1 ) {
                error = tr("Invalid character token '%1' , line %2").arg(token).arg(i+1);
                return SMatrix();
            }
            char c1 = token.at(0).toUpper().toLatin1();
            if (!alphaRow.contains(c1)) {
                error = tr("Invalid character row '%1' , line %2").arg(token).arg(i+1);
                return SMatrix();
            }
            if (mappedAlphas.contains(c1)) {
                error = tr("Duplicate character mapping '%1' , line %2").arg(token).arg(i+1);
                return SMatrix();
            }
            mappedAlphas.append(c1);
            if (tokens.length() - 1 != alphaRow.length()) {
                error = tr("Invalid number of columns '%1' , line %2").arg(tokens.length()).arg(i+1);
                return SMatrix();
            }
            for (int j=1; j < tokens.length(); j++) {
                const QString& weightToken = tokens.at(j);
                bool ok = true;
                float weight = weightToken.toFloat(&ok);
                if (!ok) {
                    error = tr("Can't parse numeric value '%1', line %2").arg(weightToken).arg(i+1);
                    return SMatrix();
                }
                char c2 = alphaRow.at(j-1);
                charScores.append(SScore(c1, c2, weight));
            }
        }
    }
    if (alphabet == NULL) {
        error = tr("Unexpected end of file!");
        return SMatrix();
    }
    assert(alphaRow.size() > 0 && charScores.size() == alphaRow.size()*alphaRow.size());
    return SMatrix(name, alphabet, charScores, description);
}

} // namespace
