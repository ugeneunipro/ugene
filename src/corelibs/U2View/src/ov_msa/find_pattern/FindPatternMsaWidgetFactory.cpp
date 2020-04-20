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

#include <QPixmap>

#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>

#include <U2View/AnnotatedDNAView.h>

#include "FindPatternMsaWidget.h"
#include "FindPatternMsaWidgetFactory.h"

namespace U2 {

const QString FindPatternMsaWidgetFactory::GROUP_ID = "OP_MSA_FIND_PATTERN_WIDGET";
const QString FindPatternMsaWidgetFactory::GROUP_ICON_STR = ":core/images/find_dialog.png";
const QString FindPatternMsaWidgetFactory::GROUP_DOC_PAGE = "39747586";

FindPatternMsaWidgetFactory::FindPatternMsaWidgetFactory() {
    objectViewOfWidget = ObjViewType_AlignmentEditor;
}

QWidget * FindPatternMsaWidgetFactory::createWidget(GObjectView* objView) {
    SAFE_POINT(NULL != objView,
        QString("Internal error: unable to create widget for group '%1', object view is NULL.").arg(GROUP_ID),
        NULL);

    MSAEditor* msaeditor = qobject_cast<MSAEditor*>(objView);
    SAFE_POINT(NULL != msaeditor,
        QString("Internal error: unable to cast object view to MSAEditor for group '%1'.").arg(GROUP_ID),
        NULL);
    FindPatternMsaWidget* widget = new FindPatternMsaWidget(msaeditor);
    widget->setObjectName("FindPatternMsaWidget");

    return widget;
}

OPGroupParameters FindPatternMsaWidgetFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), QObject::tr("Search in Alignment"), GROUP_DOC_PAGE);
}

const QString & FindPatternMsaWidgetFactory::getGroupId() {
    return GROUP_ID;
}

} // namespace U2
