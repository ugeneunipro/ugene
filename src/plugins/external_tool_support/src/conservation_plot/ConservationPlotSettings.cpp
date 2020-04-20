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


#include "ConservationPlotSettings.h"

#include <U2Core/GUrlUtils.h>

namespace U2 {

ConservationPlotSettings::ConservationPlotSettings() {
    initDefault();
}

void ConservationPlotSettings::initDefault(){
    outFile = "";
    title = "Average Phastcons around the Center of Sites";
    label = "Conservation at peak summits";
    assemblyVersion = "";
    windowSize = 1000;
    height = 1000;
    width = 1000;
}

QStringList ConservationPlotSettings::getArguments(const QList<QString>& bedFiles){
    QStringList result;

    result << "--phasdb=" + GUrlUtils::getQuotedString(assemblyVersion);

    if(height > 0){
        result << "--height=" + QByteArray::number(height);
    }

    if(width > 0){
        result << "--width=" + QByteArray::number(width);
    }

    if(windowSize > 0){
        result << "-w " + QByteArray::number(windowSize);
    }

    result << "--title=" + GUrlUtils::getQuotedString(title);

    //labels
    result << "--bed-label=" + GUrlUtils::getQuotedString(label);

    foreach(QString bedFile, bedFiles){
        bedFile = bedFile.replace(' ', '_');
        result << bedFile;
    }

    return result;
}

} // U2
