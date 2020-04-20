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


#include "MACSSettings.h"

#include <U2Core/GUrlUtils.h>

namespace U2 {

MACSSettings::MACSSettings() {
    initDefault();
}

void MACSSettings::initDefault(){

    outDir = "";
    fileNames = "default";
    wiggleOut = true;
    wiggleSpace = 10;

    genomeSizeInMbp = 2700;
    pValue = 0.00001;
    qValue = 0.1; //(MACS 2)
    useModel = true;
    modelFold = U2Region(9, 21);
    shiftSize = 100;
    keepDublicates = "1";
    bandWidth = 300;
    extFr = false; //(???)
    //optional
    tagSize = 0; //0 for default
    //advanced
    useLambda = true;
    smallNearby = 1000;
    largeNearby = 10000;
    autoBimodal = false;
    scaleLarge = false;
    shiftControl = false;   //(MACS 2)
    halfExtend = false; //(MACS 2)
    broad = false;  //(MACS 2)
    broadCutoff = 0.1;  //(MACS 2)

}

QStringList MACSSettings::getArguments( const QString& treatFilePath, const QString& conFilePath ){
    QStringList result;

    result << "--treatment=" + GUrlUtils::getQuotedString(treatFilePath);

    if (!conFilePath.isEmpty()){
        result << "--control=" + GUrlUtils::getQuotedString(conFilePath);
    }

    result << "--name=" + fileNames;

    result << "--gsize=" + QByteArray::number(static_cast<double>(genomeSizeInMbp) * 1000*1000);

    if (tagSize > 0){
        result << "--tsize=" + QByteArray::number(tagSize);
    }

    result << "--bw=" + QByteArray::number(bandWidth);

    if (pValue >= 0 && pValue <= 1){
        result << "--pvalue=" + QByteArray::number(pValue);
    }

    result << "--mfold=" + QString("%1,%2").arg(modelFold.startPos + 1).arg(modelFold.endPos());

    if(!useLambda){
        result << "--nolambda";
    }

    if(!useModel){
        result << "--nomodel";
    }

    result << "--shiftsize=" + QByteArray::number(shiftSize);

    result << "--keep-dup=" + QByteArray(qPrintable(keepDublicates));

    if (scaleLarge){
        result << "--to-large";
    }

    result << "--slocal=" + QByteArray::number(smallNearby);
    result << "--llocal=" + QByteArray::number(largeNearby);

    if(autoBimodal){
        result << "--on-auto";
    }

    if(wiggleOut){
        result << "--wig";
        result << "--single-profile";
        result << "--space=" + QByteArray::number(wiggleSpace);
    }

    return result;
}

} // U2
