/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include "RefSeqCommonWidget.h"

#include <U2Core/U2SafePoints.h>

#include <U2Gui/ShowHideSubgroupWidget.h>
#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/MsaEditor.h>

#include "../SequenceSelectorWidgetController.h"

namespace U2 {

RefSeqCommonWidget::RefSeqCommonWidget(MsaEditor* _msaEditor)
    : msaEditor(_msaEditor) {
    connect(msaEditor, SIGNAL(si_referenceSeqChanged(qint64)), SLOT(sl_refSeqChanged(qint64)));

    auto mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(5);
    mainLayout->setAlignment(Qt::AlignTop);

    QWidget* refSeqGroup = new ShowHideSubgroupWidget("REFERENCE", tr("Reference sequence"), createReferenceGroup(), true);
    mainLayout->addWidget(refSeqGroup);

    setLayout(mainLayout);
}

QWidget* RefSeqCommonWidget::createReferenceGroup() {
    auto group = new QWidget(this);
    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    layout->setAlignment(Qt::AlignTop);

    reSeqSelector = new SequenceSelectorWidgetController(msaEditor);

    connect(reSeqSelector, SIGNAL(si_selectionChanged()), SLOT(sl_textControllerChanged()));

    layout->addWidget(reSeqSelector);
    group->setLayout(layout);

    return group;
}

void RefSeqCommonWidget::sl_refSeqChanged(qint64 sequenceId) {
    reSeqSelector->setSequenceId(sequenceId);
}

void RefSeqCommonWidget::sl_textControllerChanged() {
    msaEditor->setReference(reSeqSelector->sequenceId());
}

RefSeqCommonWidgetFactory::RefSeqCommonWidgetFactory(QList<QString> groupIds)
    : OPCommonWidgetFactory(groupIds) {
}

RefSeqCommonWidgetFactory::~RefSeqCommonWidgetFactory() {
}

QWidget* RefSeqCommonWidgetFactory::createWidget(GObjectViewController* objView, const QVariantMap& /*options*/) {
    SAFE_POINT(objView != nullptr, QString("NULL object view!"), nullptr);

    auto msa = qobject_cast<MsaEditor*>(objView);
    SAFE_POINT(msa != nullptr, QString("Not MSAEditor!"), nullptr);

    return new RefSeqCommonWidget(msa);
}

}  // namespace U2
