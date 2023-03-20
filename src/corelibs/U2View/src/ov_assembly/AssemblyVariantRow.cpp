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

#include "AssemblyVariantRow.h"

#include <QPainter>
#include <QVBoxLayout>

#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/VariantTrackObject.h>

#include "AssemblyAnnotationsArea.h"
#include "AssemblyBrowser.h"

namespace U2 {

const int AssemblyVariantRow::FIXED_HEIGHT = 30;
const int AssemblyVariantRow::TOP_OFFSET = 13;

/************************************************************************/
/* AssemblyVariantRow */
/************************************************************************/
AssemblyVariantRow::AssemblyVariantRow(QWidget* parent, VariantTrackObject* _trackObj, AssemblyBrowser* _browser)
    : QWidget(parent), trackObj(_trackObj), browser(_browser), redraw(true), contextMenu(new QMenu(this)), hint(this) {
    this->setFixedHeight(FIXED_HEIGHT + TOP_OFFSET);
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setToolTip(tr("Variation track: %1").arg(trackObj->getGObjectName()));
    this->setObjectName("AssemblyVariantRow_" + trackObj->getGObjectName());

    AssemblyCellRendererFactoryRegistry* factories = browser->getCellRendererRegistry();
    AssemblyCellRendererFactory* f = factories->getFactoryById(AssemblyCellRendererFactory::ALL_NUCLEOTIDES);
    SAFE_POINT(f != nullptr, QString("AssemblyCellRendererFactory with id '%1' not found!").arg(AssemblyCellRendererFactory::ALL_NUCLEOTIDES), );
    nuclRenderer.reset(f->create());

    f = factories->getFactoryById(AssemblyCellRendererFactory::ALL_NUCLEOTIDES);
    SAFE_POINT(f != nullptr, QString("AssemblyCellRendererFactory with id '%1' not found!").arg(AssemblyCellRendererFactory::ALL_NUCLEOTIDES), );
    snpRenderer.reset(f->create());

    currentData.updateHint = false;

    // setup menu
    QAction* removeTrackAction = contextMenu->addAction(tr("Remove track from the view"));
    connect(removeTrackAction, SIGNAL(triggered()), SIGNAL(si_removeRow()));
    connect(trackObj, SIGNAL(si_nameChanged(const QString&)), SLOT(sl_redraw()));
}

VariantTrackObject* AssemblyVariantRow::getTrackObject() const {
    return trackObj;
}

void AssemblyVariantRow::sl_zoomPerformed() {
    sl_redraw();
}

void AssemblyVariantRow::sl_offsetsChanged() {
    sl_redraw();
}

void AssemblyVariantRow::paintEvent(QPaintEvent* e) {
    draw();
    QWidget::paintEvent(e);
}

void AssemblyVariantRow::resizeEvent(QResizeEvent* e) {
    sl_redraw();
    QWidget::resizeEvent(e);
}

void AssemblyVariantRow::mouseMoveEvent(QMouseEvent* e) {
    emit si_mouseMovedToPos(e->pos());
    currentData.pos = e->pos();
    currentData.updateHint = true;
    update();
    QWidget::mouseMoveEvent(e);
}

void AssemblyVariantRow::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::RightButton) {
        contextMenu->exec(QCursor::pos());
    }
}

void AssemblyVariantRow::leaveEvent(QEvent*) {
    QPoint curInHintCoords = hint.mapFromGlobal(QCursor::pos());
    if (!hint.rect().contains(curInHintCoords)) {
        sl_hideHint();
    }
}

bool AssemblyVariantRow::isSNP(const U2Variant& v) {
    if (v.refData.length() != 1) {
        return false;
    }
    if (v.obsData.length() != 1) {
        return false;
    }

    return true;
}

void AssemblyVariantRow::draw() {
    if (redraw) {
        redraw = false;
        cachedView.fill(Qt::transparent);
        QPainter p(&cachedView);

        if (browser->areCellsVisible()) {
            U2OpStatusImpl os;
            currentData.region = browser->getVisibleBasesRegion();
            QScopedPointer<U2DbiIterator<U2Variant>> varIter(trackObj->getVariants(currentData.region, os));
            SAFE_POINT_OP(os, );

            int snpWidth = browser->getCellWidth();
            int snpHeight = FIXED_HEIGHT;
            int halfHeight = int(0.5 + snpHeight / 2.0);
            int yStart = 0;
            this->prepareRenderers(snpWidth, snpHeight);

            currentData.variants.clear();
            currentData.snpWidth = snpWidth;
            while (varIter->hasNext()) {
                U2Variant variant = varIter->next();
                currentData.variants << variant;
                int xStart = (variant.startPos - currentData.region.startPos) * snpWidth;

                if (isSNP(variant)) {
                    QRect upRect(xStart, yStart, snpWidth, halfHeight);
                    QPixmap upCellImage = snpRenderer->cellImage(variant.refData.at(0));
                    p.drawPixmap(upRect, upCellImage);

                    QRect downRect(xStart, yStart + halfHeight, snpWidth, halfHeight);
                    QPixmap downCellImage = snpRenderer->cellImage(variant.obsData.at(0));
                    p.drawPixmap(downRect, downCellImage);
                } else {
                    for (int i = 0; i < variant.refData.length(); i++, xStart += snpWidth) {
                        QRect snpRect(xStart, yStart, snpWidth, snpHeight);
                        char c = variant.refData.at(i);
                        QPixmap cellImage = nuclRenderer->cellImage(c);
                        p.drawPixmap(snpRect, cellImage);
                    }
                }
            }
        }
    }

    QPainter rowP(this);
    rowP.drawText(this->rect(), trackObj->getGObjectName());
    rowP.drawPixmap(0, TOP_OFFSET, cachedView);
    if (currentData.updateHint) {
        currentData.updateHint = false;
        updateHint();
    }
}

bool AssemblyVariantRow::findVariantOnPos(QList<U2Variant>& variants) {
    bool found = false;
    int startPos = currentData.region.startPos;
    foreach (const U2Variant& v, currentData.variants) {
        int xStart = (v.startPos - startPos) * currentData.snpWidth;
        int xEnd = 0;
        if (isSNP(v)) {  // SNP
            xEnd = xStart + currentData.snpWidth;
        } else {
            xEnd = (v.refData.length()) * currentData.snpWidth + xStart;
        }

        if (currentData.pos.x() >= xStart && currentData.pos.x() <= xEnd) {
            variants << v;
            found = true;
        }
    }
    return found;
}

void AssemblyVariantRow::sl_hideHint() {
    hint.hide();
    update();
}

void AssemblyVariantRow::updateHint() {
    if (currentData.variants.isEmpty()) {
        sl_hideHint();
        return;
    }

    // 1. find variant we stay on
    QList<U2Variant> variants;
    bool found = findVariantOnPos(variants);
    if (!found) {
        sl_hideHint();
        return;
    }

    // 2. set hint info
    hint.setData(variants);

    // 3. move hint if needed
    QRect readsAreaRect(mapToGlobal(rect().topLeft()), mapToGlobal(rect().bottomRight()));
    QRect hintRect = hint.rect();
    hintRect.moveTo(QCursor::pos() + AssemblyReadsAreaHint::OFFSET_FROM_CURSOR);
    QPoint offset(0, 0);
    if (hintRect.right() > readsAreaRect.right()) {
        offset -= QPoint(hintRect.right() - readsAreaRect.right(), 0);
    }
    if (hintRect.bottom() > readsAreaRect.bottom()) {
        offset -= QPoint(0, hintRect.bottom() - readsAreaRect.bottom());  // move hint bottom to reads area
        offset -= QPoint(0, readsAreaRect.bottom() - QCursor::pos().y() + AssemblyReadsAreaHint::OFFSET_FROM_CURSOR.y());
    }
    QPoint newPos = QCursor::pos() + AssemblyReadsAreaHint::OFFSET_FROM_CURSOR + offset;
    if (hint.pos() != newPos) {
        hint.move(newPos);
    }
    if (!hint.isVisible()) {
        hint.show();
    }
}

void AssemblyVariantRow::prepareRenderers(int cellWidth, int cellHeight) {
    bool text = browser->areLettersVisible();
    QFont f = browser->getFont();
    int halfHeight = int(0.5 + cellHeight / 2.0);
    bool snpText = text;
    QFont snpF = f;
    if (text) {
        int pointSize = qMin(cellWidth, cellHeight) / 2;
        if (pointSize) {
            f.setPointSize(pointSize);
        } else {
            text = false;
        }

        int snpPointSize = qMin(cellWidth, halfHeight) / 2;
        if (snpPointSize) {
            snpF.setPointSize(snpPointSize);
        } else {
            snpText = false;
        }
    }
    nuclRenderer->render(QSize(cellWidth, cellHeight), devicePixelRatio(), text, f);
    snpRenderer->render(QSize(cellWidth, halfHeight), devicePixelRatio(), snpText, snpF);
}

void AssemblyVariantRow::sl_redraw() {
    cachedView = QPixmap(size() * devicePixelRatio());
    cachedView.setDevicePixelRatio(devicePixelRatio());
    redraw = true;
    update();
}

/************************************************************************/
/* AssemblyVariantRowManager */
/************************************************************************/
AssemblyVariantRowManager::AssemblyVariantRowManager(AssemblyBrowserUi* _ui)
    : ui(_ui), browser(_ui->getWindow()), model(_ui->getModel()) {
    connect(model.data(), SIGNAL(si_trackAdded(VariantTrackObject*)), SLOT(sl_trackAdded(VariantTrackObject*)));
    connect(model.data(), SIGNAL(si_trackRemoved(VariantTrackObject*)), SLOT(sl_trackRemoved(VariantTrackObject*)));
}

void AssemblyVariantRowManager::sl_trackRemoved(VariantTrackObject* objToRemove) {
    AssemblyAnnotationsArea* annsArea = ui->getAnnotationsArea();

    QLayout* layout = annsArea->layout();
    auto vertLayout = qobject_cast<QVBoxLayout*>(layout);
    SAFE_POINT(nullptr != vertLayout, "Internal error: layout problems", );

    for (int i = 0; i < vertLayout->count(); i++) {
        QLayoutItem* it = vertLayout->itemAt(i);
        QWidget* w = it->widget();
        auto row = dynamic_cast<AssemblyVariantRow*>(w);
        if (row == nullptr) {
            continue;
        }
        VariantTrackObject* trackObj = row->getTrackObject();
        if (objToRemove == trackObj) {
            vertLayout->removeWidget(row);
            row->hide();
            break;
        }
    }
}

void AssemblyVariantRowManager::sl_trackAdded(VariantTrackObject* newTrackObj) {
    AssemblyAnnotationsArea* annsArea = ui->getAnnotationsArea();

    QLayout* layout = annsArea->layout();
    auto vertLayout = qobject_cast<QVBoxLayout*>(layout);
    SAFE_POINT(nullptr != vertLayout, "Internal error: layout problems", );

    AssemblyVariantRow* row = new AssemblyVariantRow(annsArea, newTrackObj, browser);
    vertLayout->addWidget(row);

    connect(browser, SIGNAL(si_zoomOperationPerformed()), row, SLOT(sl_zoomPerformed()));
    connect(browser, SIGNAL(si_offsetsChanged()), row, SLOT(sl_offsetsChanged()));
    connect(row, SIGNAL(si_mouseMovedToPos(const QPoint&)), annsArea, SIGNAL(si_mouseMovedToPos(const QPoint&)));
    connect(row, SIGNAL(si_removeRow()), SLOT(sl_removeRow()));
}

void AssemblyVariantRowManager::sl_removeRow() {
    QObject* s = sender();
    auto row = dynamic_cast<AssemblyVariantRow*>(s);
    SAFE_POINT(nullptr != row, "Internal error: NULL row widget", );

    model->sl_trackObjRemoved(row->getTrackObject());
}

}  // namespace U2
