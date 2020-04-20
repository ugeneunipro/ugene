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


#include "Peak2GeneSettings.h"
#include "Peak2GeneSupport.h"

#include <U2Core/GUrlUtils.h>
#include <U2Core/AppContext.h>
#include <U2Core/DataPathRegistry.h>

namespace U2 {

const QString Peak2GeneSettings::OUT_TYPE_UPSTREAM = "up";
const QString Peak2GeneSettings::OUT_TYPE_DOWNSTREAM = "down";
const QString Peak2GeneSettings::OUT_TYPE_ALL = "all";

const QString Peak2GeneSettings::DEFAULT_NAME = "Default";

Peak2GeneSettings::Peak2GeneSettings() {
    initDefault();
}

void Peak2GeneSettings::initDefault(){
    outpos = Peak2GeneSettings::OUT_TYPE_ALL;
    symbol = false;
    distance = 3000;
    genomePath = "";
}

QStringList Peak2GeneSettings::getArguments( const QString& treatFilePath){
    QString entrezPath = "";
    //init data path

    U2DataPathRegistry* dpr =  AppContext::getDataPathRegistry();
    if (dpr){
        U2DataPath* dp = dpr->getDataPathByName(Peak2GeneSupport::ENTREZ_TRANSLATION_DATA_NAME);
        if (dp && dp->isValid()){
            if(!dp->getDataNames().isEmpty()){
                entrezPath = dp->getPathByName(dp->getDataNames().first());
            }
        }
    }

    QStringList result;

    result << "--treat=" + GUrlUtils::getQuotedString(treatFilePath);

    result << "--name=" + DEFAULT_NAME;

    result << "--op=" + outpos;

    if (symbol){
        result << "--symbol";
    }

    result << "--distance=" + QByteArray::number(distance);

    result << "--genome=" + GUrlUtils::getQuotedString(genomePath);

    result << "--entrez=" + GUrlUtils::getQuotedString(entrezPath);

    return result;
}

} // U2
