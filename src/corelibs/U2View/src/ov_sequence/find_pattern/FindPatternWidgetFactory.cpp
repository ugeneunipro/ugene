/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
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

#include "FindPatternWidgetFactory.h"

#include <QPixmap>

#include <U2Core/U2SafePoints.h>

#include <U2View/AnnotatedDNAView.h>

#include "FindPatternWidget.h"

namespace U2 {

const QString FindPatternWidgetFactory::GROUP_ID = "OP_FIND_PATTERN";
const QString FindPatternWidgetFactory::GROUP_ICON_STR = ":core/images/find_dialog.png";
const QString FindPatternWidgetFactory::GROUP_DOC_PAGE = "65929429";

FindPatternWidgetFactory::FindPatternWidgetFactory() {
    objectViewOfWidget = ObjViewType_SequenceView;
}

QWidget* FindPatternWidgetFactory::createWidget(GObjectViewController* objView, const QVariantMap& /*options*/) {
    SAFE_POINT(objView != nullptr,
               QString("Internal error: unable to create widget for group '%1', object view is NULL.").arg(GROUP_ID),
               nullptr);

    auto annotatedDnaView = qobject_cast<AnnotatedDNAView*>(objView);
    SAFE_POINT(annotatedDnaView != nullptr,
               QString("Internal error: unable to cast object view to AnnotatedDNAView for group '%1'.").arg(GROUP_ID),
               nullptr);

    FindPatternWidget* widget = new FindPatternWidget(annotatedDnaView);
    widget->setObjectName("FindPatternWidget");
    return widget;
}

OPGroupParameters FindPatternWidgetFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), QObject::tr("Search in Sequence"), GROUP_DOC_PAGE);
}

const QString& FindPatternWidgetFactory::getGroupId() {
    return GROUP_ID;
}

}  // namespace U2
