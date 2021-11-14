/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2022 UniPro <ugene@unipro.ru>
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

#include "McaReadsTabFactory.h"

#include <QVBoxLayout>

#include <U2Core/U2SafePoints.h>

#include <U2Gui/ShowHideSubgroupWidget.h>

#include <U2View/MSAEditor.h>

#include "McaAlternativeMutationsWidget.h"

namespace U2 {

const QString McaReadsTabFactory::GROUP_ID = "OP_MCA_READS";
const QString McaReadsTabFactory::GROUP_ICON_STR = ":core/images/graphs.png";
const QString McaReadsTabFactory::GROUP_DOC_PAGE = "66814020";

McaReadsTabFactory::McaReadsTabFactory() {
    objectViewOfWidget = ObjViewType_ChromAlignmentEditor;
}

QWidget* McaReadsTabFactory::createWidget(GObjectView* objView, const QVariantMap&) {
    SAFE_POINT(objView != nullptr,
               QString("Internal error: unable to create widget for group '%1', object view is NULL.").arg(GROUP_ID),
               nullptr);

    MaEditor* ma = qobject_cast<MaEditor*>(objView);
    SAFE_POINT(ma != nullptr,
               QString("Internal error: unable to cast object view to MaEditor for group '%1'.").arg(GROUP_ID),
               nullptr);

    QWidget* widget = new QWidget(objView->getWidget());
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);

    auto alternativeMutationsWgt = new McaAlternativeMutationsWidget(widget);
    auto ui = ma->getUI();
    SAFE_POINT(ui != nullptr, "UI isn't found", nullptr);

    alternativeMutationsWgt->init(ma->getMaObject(), ui->getUI()->getSequenceArea(), ui->getStatusBar());
    auto alternativeMutations = new ShowHideSubgroupWidget("ALTERNATIVE_MUTATIONS_MODE", tr("Alternative mutations"), alternativeMutationsWgt, true);
    layout->addWidget(alternativeMutations);

    return widget;
}

OPGroupParameters McaReadsTabFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), QObject::tr("Reads"), GROUP_DOC_PAGE);
}

}  // namespace U2
