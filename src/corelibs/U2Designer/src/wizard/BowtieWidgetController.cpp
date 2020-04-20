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

#include <QPushButton>

#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include "BowtieWidgetController.h"
#include "PropertyWidget.h"

namespace U2 {

BowtieWidgetController::BowtieWidgetController(WizardController *wc, BowtieWidget *bw, int labelSize)
: WidgetController(wc), bw(bw)
{
    dirW = new AttributeWidget();
    dirW->setInfo(bw->idxDir);
    dirCtrl = new DefaultPropertyController(wc, dirW, labelSize);
    dirCtrl->setNoDelegate(true);

    nameW = new AttributeWidget();
    nameW->setInfo(bw->idxName);
    nameCtrl = new DefaultPropertyController(wc, nameW, labelSize);
    nameCtrl->setNoDelegate(true);
}

BowtieWidgetController::~BowtieWidgetController() {
    delete dirCtrl;
    delete dirW;

    delete nameCtrl;
    delete nameW;
}

QWidget * BowtieWidgetController::createGUI(U2OpStatus &os) {
    QScopedPointer<QWidget> result(new QWidget());

    QVBoxLayout *vl = new QVBoxLayout();
    vl->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *hl = new QHBoxLayout(result.data());
    hl->setContentsMargins(0, 0, 0, 0);
    hl->addLayout(vl);

    vl->addWidget(dirCtrl->createGUI(os));
    CHECK_OP(os, NULL);
    vl->addWidget(nameCtrl->createGUI(os));
    CHECK_OP(os, NULL);

    QPushButton *browseButton = new QPushButton(tr("Select\nbowtie index file"), result.data());
    browseButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    connect(browseButton, SIGNAL(clicked()), SLOT(sl_browse()));
    hl->addWidget(browseButton);

    return result.take();
}

void BowtieWidgetController::sl_browse() {
    LastUsedDirHelper lod;
    QString lastDir = lod.dir;
    QString url = U2FileDialog::getOpenFileName(NULL, tr("Select one of Bowtie index files"), lastDir);
    if (url.isEmpty()) {
        return;
    }
    lod.url = url;

    QString dirUrl = finalyze(url);
    dirCtrl->updateGUI(dirUrl);
    wc->setAttributeValue(bw->idxDir, dirUrl);
}

QString BowtieWidgetController::finalyze(const QString &url) {
    return NoFileURLWidget::finalyze(url, wc->getTags(bw->idxDir));
}

} // U2
