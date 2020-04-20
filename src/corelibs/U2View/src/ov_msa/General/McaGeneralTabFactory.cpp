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

#include "McaGeneralTabFactory.h"
#include "McaGeneralTab.h"

#include "../McaEditor.h"
#include <U2Core/U2SafePoints.h>

namespace U2 {

const QString McaGeneralTabFactory::GROUP_ID = "OP_MCA_GENERAL";
const QString McaGeneralTabFactory::GROUP_ICON_STR = ":core/images/settings2.png";
const QString McaGeneralTabFactory::GROUP_DOC_PAGE = "24749096";

McaGeneralTabFactory::McaGeneralTabFactory() {
    objectViewOfWidget = ObjViewType_ChromAlignmentEditor;
}

QWidget* McaGeneralTabFactory::createWidget(GObjectView *objView) {
    SAFE_POINT(NULL != objView,
        QString("Internal error: unable to create widget for group '%1', object view is NULL.").arg(GROUP_ID),
        NULL);

    McaEditor* msa = qobject_cast<McaEditor*>(objView);
    SAFE_POINT(NULL != msa,
        QString("Internal error: unable to cast object view to McaEditor for group '%1'.").arg(GROUP_ID),
        NULL);

    McaGeneralTab *widget = new McaGeneralTab(msa);
    widget->setObjectName("McaGeneralTab");
    return widget;
}

const QString & McaGeneralTabFactory::getGroupId() {
    return GROUP_ID;
}

OPGroupParameters McaGeneralTabFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), QObject::tr("General"), GROUP_DOC_PAGE);
}

} // namespace
