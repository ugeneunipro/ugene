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

#include "OPWidgetFactory.h"

#include <U2Core/U2SafePoints.h>


namespace U2 {

OPGroupParameters::OPGroupParameters(QString groupId, QPixmap headerImage, QString title, QString documentationPage)
    :  groupId(groupId),
       groupIcon(headerImage),
       groupTitle(title),
       groupDocumentationPage(documentationPage)
{
}

bool OPFactoryFilterVisitor::atLeastOneAlphabetPass(DNAAlphabetType factoryAlphabetType) {
    for (int i = 0; i < objectAlphabets.size(); i++) {
        if (objectAlphabets[i] == factoryAlphabetType)
            return true;
    }
    return false;
}

OPWidgetFactory::OPWidgetFactory()
    : QObject(),
      objView(NULL),
      objectViewOfWidget(ObjViewType_SequenceView)
{

}

OPWidgetFactory::~OPWidgetFactory() {

}

bool OPWidgetFactory::passFiltration( OPFactoryFilterVisitorInterface* filter ){
    //by default checks type only
    bool res = false;

    SAFE_POINT(filter != NULL, "OPWidgetFactory::passFiltration. Filter is null", res);

    res = filter->typePass(getObjectViewType());

    return res;
}

OPCommonWidgetFactory::OPCommonWidgetFactory(QList<QString> _groupIds)
    : groupIds(_groupIds)
{
}

OPCommonWidgetFactory::~OPCommonWidgetFactory() {
}

OPFactoryFilterVisitorInterface::~OPFactoryFilterVisitorInterface() {

}

} // namespace
