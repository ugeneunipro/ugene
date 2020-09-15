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

#include "MaExportConsensusTabFactory.h"

#include <QPixmap>

#include <U2Core/U2SafePoints.h>

#include <U2Gui/ShowHideSubgroupWidget.h>

#include <U2View/MSAEditor.h>

#include "../General/MaConsensusModeWidget.h"
#include "MaExportConsensusWidget.h"

namespace U2 {

const QString GROUP_ICON_STR = ":core/images/consensus.png";
const QString GROUP_DOC_PAGE_MSA = "20880299";
const QString GROUP_DOC_PAGE_MCA = "20880557";
const QString MsaExportConsensusTabFactory::GROUP_ID = "OP_EXPORT_CONSENSUS";
const QString McaExportConsensusTabFactory::GROUP_ID = "OP_CONSENSUS";

MsaExportConsensusTabFactory::MsaExportConsensusTabFactory() {
    objectViewOfWidget = ObjViewType_AlignmentEditor;
}

QWidget *MsaExportConsensusTabFactory::createWidget(GObjectView *objView, const QVariantMap & /*options*/) {
    SAFE_POINT(objView != nullptr,
               QString("Internal error: unable to create widget for group '%1', object view is NULL.").arg(GROUP_ID),
               nullptr);

    MSAEditor *ma = qobject_cast<MSAEditor *>(objView);
    SAFE_POINT(ma != nullptr,
               QString("Internal error: unable to cast object view to MsaEditor for group '%1'.").arg(GROUP_ID),
               nullptr);

    return new MaExportConsensusWidget(ma);
}

OPGroupParameters MsaExportConsensusTabFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), QObject::tr("Export Consensus"), GROUP_DOC_PAGE_MSA);
}

McaExportConsensusTabFactory::McaExportConsensusTabFactory() {
    objectViewOfWidget = ObjViewType_ChromAlignmentEditor;
}

QWidget *McaExportConsensusTabFactory::createWidget(GObjectView *objView, const QVariantMap & /*options*/) {
    SAFE_POINT(objView != nullptr,
               QString("Internal error: unable to create widget for group '%1', object view is NULL.").arg(GROUP_ID),
               nullptr);

    MaEditor *ma = qobject_cast<MaEditor *>(objView);
    SAFE_POINT(ma != nullptr,
               QString("Internal error: unable to cast object view to MaEditor for group '%1'.").arg(GROUP_ID),
               nullptr);

    QWidget *widget = new QWidget(objView->getWidget());
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);

    MaConsensusModeWidget *consensusModeWgt = new MaConsensusModeWidget(widget);
    consensusModeWgt->init(ma->getMaObject(), ma->getUI()->getConsensusArea());
    ShowHideSubgroupWidget *consensusMode = new ShowHideSubgroupWidget("CONSENSUS_MODE", tr("Consensus mode"), consensusModeWgt, true);

    MaExportConsensusWidget *exportWidget = new MaExportConsensusWidget(ma, widget);
    exportWidget->layout()->setContentsMargins(9, 9, 9, 9);
    ShowHideSubgroupWidget *exportConsensus = new ShowHideSubgroupWidget("EXPORT_CONSENSUS", tr("Export consensus"), exportWidget, true);

    layout->addWidget(consensusMode);
    layout->addWidget(exportConsensus);
    return widget;
}

OPGroupParameters McaExportConsensusTabFactory::getOPGroupParameters() {
    return OPGroupParameters(GROUP_ID, QPixmap(GROUP_ICON_STR), QObject::tr("Consensus"), GROUP_DOC_PAGE_MCA);
}

const QString &McaExportConsensusTabFactory::getGroupId() {
    return GROUP_ID;
}

}    // namespace U2
