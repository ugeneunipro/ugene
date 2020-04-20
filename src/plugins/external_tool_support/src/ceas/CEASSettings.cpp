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

#include <assert.h>

#include "CEASSettings.h"

namespace U2 {

CEASSettings::CEASSettings() {
    initializeDefaults();
}

CEASSettings::CEASSettings(const QString &_bedFile, const QString &_wigFile, const QString &_gdbFile)
: bedFile(_bedFile), wigFile(_wigFile), gdbFile(_gdbFile)
{
    initializeDefaults();
}

void CEASSettings::initializeDefaults() {
    bedFile = "";
    wigFile = "";
    gdbFile = "";
    bg = false;
    span = -1;
    profilingRes = -1;
    relativeDist = -1;
    name2 = false;
    dump = false;
    experimentName = "tmp";
}

void CEASSettings::setBedFile(const QString &value) {
    bedFile = value;
}

void CEASSettings::setWigFile(const QString &value) {
    wigFile = value;
}

void CEASSettings::setGdbFile(const QString &value) {
    gdbFile = value;
}

void CEASSettings::setImagePath(const QString &value) {
    imageFile = value;
}

void CEASSettings::setImageFormat(const QString &value) {
    imageFormat = value;
}

void CEASSettings::setAnnsFilePath(const QString &value) {
    annsFile = value;
}

void CEASSettings::setExtraBedFile(const QString &value) {
    extraBedFile = value;
}

void CEASSettings::setExperimentName(const QString &value) {
    experimentName = value;
}

void CEASSettings::setSizes(int value) {
    sizes.clear();
    sizes << value;
}

void CEASSettings::setSizes(int val1, int val2, int val3) {
    sizes.clear();
    sizes << val1 << val2 << val3;
}

void CEASSettings::setBisizes(int value) {
    bisizes.clear();
    bisizes << value;
}

void CEASSettings::setBisizes(int val1, int val2) {
    bisizes.clear();
    bisizes << val1 << val2;
}

void CEASSettings::setBg(bool value) {
    bg = value;
}

void CEASSettings::setSpan(int value) {
    span = value;
}

void CEASSettings::setProfilingRes(int value) {
    profilingRes = value;
}

void CEASSettings::setRelativeDist(int value) {
    relativeDist = value;
}

void CEASSettings::addGeneGroup(const QString &file) {
    if (!file.isEmpty()) {
        geneGroups << file;
    }
}

void CEASSettings::removeGeneGroups() {
    geneGroups.clear();
}

void CEASSettings::addGeneGroupName(const QString &name) {
    geneNames << name;
}

void CEASSettings::removeGeneGroupsNames() {
    geneNames.clear();
}

void CEASSettings::setName2(bool value) {
    name2 = value;
}

void CEASSettings::setDump(bool value) {
    dump = value;
}

QStringList CEASSettings::getArgumentList() const {
    QStringList result;
    if (!bedFile.isEmpty()){
        result << "--bed=" + getCorrectArgValue(bedFile);
    }
    if (!wigFile.isEmpty()){
        result << "--wig=" + getCorrectArgValue(wigFile);
    }
    if (!gdbFile.isEmpty()){
        result << "--gt=" + getCorrectArgValue(gdbFile);
    }

    if (!extraBedFile.isEmpty()) {
        result << "--ebed=" + getCorrectArgValue(extraBedFile);
    }
    if (!experimentName.isEmpty()) {
        result << "--name=" + getCorrectArgValue(experimentName);
    }
    if (!sizes.isEmpty()) {
        assert(1 == sizes.size() || 3 == sizes.size());
        QString sizesArg = "--sizes=" + QByteArray::number(sizes[0]);
        for (int i = 1; i < sizes.size(); i++) {
            sizesArg += "," + QByteArray::number(sizes[i]);
        }
        result << sizesArg;
    }
    if (!bisizes.isEmpty()) {
        assert(1 == bisizes.size() || 2 == bisizes.size());
        QString bisizesArg = "--bisizes=" + QByteArray::number(bisizes[0]);
        if (2 == bisizes.size()) {
            bisizesArg += "," + QByteArray::number(bisizes[1]);
        }
        result << bisizesArg;
    }
    if (bg) {
        result << "--bg";
    }
    if (span > 0) {
        result << "--span=" + QByteArray::number(span);
    }
    if (profilingRes > 0) {
        result << "--pf-res=" + QByteArray::number(profilingRes);
    }
    if (relativeDist > 0) {
        result << "--rel-dist=" + QByteArray::number(relativeDist);
    }
    if (!geneGroups.isEmpty()) {
        QString geneArgValue = geneGroups[0];
        for (int i=0; i<geneGroups.size(); i++) {
            geneArgValue += "," + geneGroups[i];
        }
        result << "--gn-groups=" + getCorrectArgValue(geneArgValue);
    }
    if (!geneNames.isEmpty()) {
        QString nameArgValue = geneNames[0];
        for (int i=0; i<geneGroups.size(); i++) {
            nameArgValue += "," + geneNames[i];
        }
        result << "--gn-group-names=" + getCorrectArgValue(nameArgValue);
    }
    if (name2) {
        result << "--gname2";
    }
    if (dump) {
        result << "--dump";
    }

    return result;
}

QString CEASSettings::getCorrectArgValue(const QString &value) const {
    if (value.contains(QRegExp("\\s"))) {
        //QString res = value;
        //res = res.replace(QRegExp("\\s"), "\\ ");
        //return res;
        return "\"" + value + "\"";
    }
    return value;
}

const QString & CEASSettings::getImageFilePath() const {
    return imageFile;
}

const QString & CEASSettings::getImageFileFormat() const {
    return imageFormat;
}

const QString & CEASSettings::getAnnsFilePath() const {
    return annsFile;
}

} // U2
