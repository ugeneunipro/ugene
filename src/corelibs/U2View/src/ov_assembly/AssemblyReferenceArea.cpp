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

#include "AssemblyReferenceArea.h"

#include <QPainter>
#include <QMouseEvent>
#include <U2Core/Timer.h>
#include <U2Core/U2SafePoints.h>

#include "AssemblyBrowser.h"

namespace U2 {

AssemblySequenceArea::AssemblySequenceArea(AssemblyBrowserUi * ui_, char skipChar_) :
    QWidget(ui_), browser(ui_->getWindow()), ui(ui_), model(ui_->getModel()), cellRenderer(NULL), skipChar(skipChar_)
{
    setFixedHeight(FIXED_HEIGHT);
    connectSlots();
    sl_redraw();
    setMouseTracking(true);

    setNormalCellRenderer();

    setObjectName("Assembly reference sequence area");
}

void AssemblySequenceArea::connectSlots() {
    connect(browser, SIGNAL(si_zoomOperationPerformed()), SLOT(sl_zoomPerformed()));
    connect(browser, SIGNAL(si_offsetsChanged()), SLOT(sl_offsetsChanged()));
}

void AssemblySequenceArea::setNormalCellRenderer() {
    initCellRenderer(AssemblyCellRendererFactory::ALL_NUCLEOTIDES);
    needsReference = false;
}

void AssemblySequenceArea::setDiffCellRenderer() {
    initCellRenderer(AssemblyCellRendererFactory::DIFF_NUCLEOTIDES);
    needsReference = true;
}

void AssemblySequenceArea::initCellRenderer(QString id) {
    AssemblyCellRendererFactoryRegistry *factories = browser->getCellRendererRegistry();
    AssemblyCellRendererFactory *f = factories->getFactoryById(id);
    SAFE_POINT(f != NULL, QString("AssemblyCellRendererFactory with id '%1' not found!").arg(id),);
    cellRenderer.reset(f->create());
}

void AssemblySequenceArea::drawAll() {
    if(canDrawSequence()) {
        if (redraw) {
            cachedView = QPixmap(size() * devicePixelRatio());
            cachedView.setDevicePixelRatio(devicePixelRatio());
            cachedView.fill(Qt::transparent);
            QPainter p(&cachedView);
            redraw = false;
            drawSequence(p);
        }
        QPainter p(this);
        p.drawPixmap(0, 0, cachedView);
    }
}

U2Region AssemblySequenceArea::getVisibleRegion() const {
    return browser->getVisibleBasesRegion();
}

bool AssemblySequenceArea::areCellsVisible() const {
    return browser->areCellsVisible();
}

void AssemblySequenceArea::drawSequence(QPainter & p) {
    GTIMER(c1, t1, "AssemblySequenceArea::drawSequence");

    if(areCellsVisible()) {
        p.fillRect(rect(), Qt::transparent);

        U2OpStatusImpl status;
        QByteArray visibleSequence = getSequenceRegion(status);
        CHECK_OP(status,);

        int letterWidth = browser->getCellWidth();
        int letterHeight = FIXED_HEIGHT;

        int x_pix_start = 0;
        const int y_pix_start = 0;

        bool text = browser->areLettersVisible();
        QFont f = browser->getFont();
        if(text) {
            int pointSize = qMin(letterWidth, letterHeight) / 2;
            if(pointSize) {
                f.setPointSize(pointSize);
            } else {
                text = false;
            }
        }
        cellRenderer->render(QSize(letterWidth, letterHeight), devicePixelRatio(), text, f);
        QByteArray referenceFragment;
        if(needsReference) {
            referenceFragment = model->getReferenceRegionOrEmpty(getVisibleRegion());
        }

        for(int i = 0; i < visibleSequence.length(); ++i, x_pix_start+=letterWidth) {
            QRect r(x_pix_start, y_pix_start, letterWidth, letterHeight);
            char c = visibleSequence.at(i);
            // TODO: not hard-coded
            if(c != skipChar) {
                QPixmap cellImage;
                if(referenceFragment.isEmpty() || i >= referenceFragment.length()) {
                    cellImage = cellRenderer->cellImage(c);
                } else {
                    cellImage = cellRenderer->cellImage(U2AssemblyRead(), c, referenceFragment.at(i));
                }
                p.drawPixmap(r, cellImage);
            }
        }
    }
}

void AssemblySequenceArea::paintEvent(QPaintEvent * e) {
    drawAll();
    QWidget::paintEvent(e);
}

void AssemblySequenceArea::resizeEvent(QResizeEvent * e) {
    sl_redraw();
    QWidget::resizeEvent(e);
}

void AssemblySequenceArea::mouseMoveEvent(QMouseEvent * e) {
    emit si_mouseMovedToPos(e->pos());
    QWidget::mouseMoveEvent(e);
}

void AssemblySequenceArea::sl_redraw() {
        redraw = true;
        update();
}

void AssemblySequenceArea::sl_offsetsChanged() {
    sl_redraw();
}

void AssemblySequenceArea::sl_zoomPerformed() {
    sl_redraw();
}

/////////////////////////////////////////////////////////////////
// AssemblyReferenceArea

AssemblyReferenceArea::AssemblyReferenceArea(AssemblyBrowserUi * ui_) :
    AssemblySequenceArea(ui_), referenceAreaMenu(new QMenu(this)), unassociateReferenceAction(NULL)
{
    setToolTip(tr("Reference sequence"));
    // setup menu
    referenceAreaMenu->addAction(ui_->getWindow()->getSetReferenceAction());
    unassociateReferenceAction = referenceAreaMenu->addAction(tr("Unassociate"));
    unassociateReferenceAction->setObjectName("unassociateReferenceAction");
    connect(unassociateReferenceAction, SIGNAL(triggered()), SIGNAL(si_unassociateReference()));
    connect(getModel().data(), SIGNAL(si_referenceChanged()), SLOT(sl_onReferenceChanged()));
    sl_onReferenceChanged();
}

bool AssemblyReferenceArea::canDrawSequence() {
    return !getModel()->isEmpty() && (getModel()->hasReference() || getModel()->isLoadingReference());
}

QByteArray AssemblyReferenceArea::getSequenceRegion(U2OpStatus &os) {
    return getModel()->getReferenceRegion(getVisibleRegion(), os);
}

void AssemblyReferenceArea::mousePressEvent(QMouseEvent* e) {
    if(e->button() == Qt::RightButton) {
        referenceAreaMenu->exec(QCursor::pos());
    }
}

void AssemblyReferenceArea::drawSequence(QPainter &p) {
    if(getModel()->isLoadingReference()) {
        p.drawText(rect(), Qt::AlignCenter, tr("Reference is loading..."));
    } else {
        AssemblySequenceArea::drawSequence(p);
    }
}

void AssemblyReferenceArea::sl_onReferenceChanged() {
    unassociateReferenceAction->setEnabled(getModel()->referenceAssociated() && !getModel()->isLoadingReference());
}

} //ns
