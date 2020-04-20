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

#include "DIPropertiesTests.h"

/* TRANSLATOR U2::GTest */

namespace U2 {

#define DI_KEY_ATTR  "key"
#define DI_VAL_ATTR  "val"
#define EXPECTED_AVE_ATTR "exp_ave"
#define EXPECTED_SDEV_ATTR "exp_sdev"

void GTest_DiPropertySiteconCheckAttribs::init(XMLTestFormat *tf, const QDomElement& el) {
    Q_UNUSED(tf);

    key = el.attribute(DI_KEY_ATTR);
    if (key.isEmpty()) {
        failMissingValue(DI_KEY_ATTR);
        return;
    }

    val = el.attribute(DI_VAL_ATTR);
    if (val.isEmpty()) {
        failMissingValue(DI_VAL_ATTR);
        return;
    }
  
    QString expAveStr = el.attribute(EXPECTED_AVE_ATTR);
    if (expAveStr.isEmpty()) {
        failMissingValue(EXPECTED_AVE_ATTR);
        return;
    }
    bool isOk;
    expAve = qRound(expAveStr.toFloat(&isOk) * 10000);
    if(!isOk) {
        stateInfo.setError(  QString("Wrong conversion to the integer for %1").arg(EXPECTED_AVE_ATTR) );
        return;
    }

    QString expSdevStr = el.attribute(EXPECTED_SDEV_ATTR);
    if (expAveStr.isEmpty()) {
        failMissingValue(EXPECTED_SDEV_ATTR);
        return;
    }
    expSdev = qRound(expSdevStr.toFloat(&isOk) * 10000);
    if(!isOk) {
        stateInfo.setError(  QString("Wrong conversion to the integer for %1").arg(EXPECTED_SDEV_ATTR) );
        return;
    }
}

Task::ReportResult GTest_DiPropertySiteconCheckAttribs::report() {
    QList<DiPropertySitecon*> propList = result.getProperties();
    QList<DiPropertySitecon*>::const_iterator it;
    for (it = propList.begin(); it != propList.end(); it++) {
        if((*it)->keys.take(key) == val){
           int ave = qRound((*it)->average * 10000), sdev = qRound((*it)->sdeviation * 10000);
           if (sdev != expSdev) {
               stateInfo.setError(  QString("Expected and Actual 'SDev' values are different: %1 %2").arg(expSdev/10000).arg(sdev/10000) );
               return ReportResult_Finished;
           }
           if (ave != expAve) {
               stateInfo.setError(  QString("Expected and Actual 'Average' values are different: %1 %2").arg(expAve/10000).arg(ave/10000) );
           }
           return ReportResult_Finished;
        }
    }
    stateInfo.setError(  QString("Given key '%1' don't present in property map").arg(key) );
    return ReportResult_Finished;
}

}//ns
