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
#include "AssemblyRuler.h"

#include <math.h>

#include <QLine>
#include <QPainter>
#include <QMouseEvent>

#include <U2Core/FormatUtils.h>

#include "AssemblyBrowser.h"
#include "AssemblyReadsArea.h" //TODO get rid of cross-widget dependencies ?
#include "AssemblyBrowserSettings.h"

namespace U2 {

static const int FIXED_HEIGHT = 36;
static const int AXIS_LINE_Y = 6;

//see drawRuler() for details about short/long/border notches
//these are Y coords of all of the notches
static const int BORDER_NOTCH_START = 2;
static const int BORDER_NOTCH_END = 20;
static const int SHORT_NOTCH_START = 5;
static const int SHORT_NOTCH_END = 10;
static const int LONG_NOTCH_START = 3;
static const int LONG_NOTCH_END = 18;
static const int LABELS_END = LONG_NOTCH_END + 2;

AssemblyRuler::AssemblyRuler(AssemblyBrowserUi * ui_) :
QWidget(ui_), ui(ui_), browser(ui->getWindow()), model(ui->getModel()), cursorPos(0),
showCoords(AssemblyBrowserSettings::getShowCoordsOnRuler()),
showCoverage(AssemblyBrowserSettings::getShowCoverageOnRuler())
{
    setFixedHeight(FIXED_HEIGHT);
    connectSlots();
    sl_redraw();
    setMouseTracking(true);

    QObject *startPositionParent = new QObject(this);
    startPositionParent->setObjectName("start position");
    startPositionObject = new QObject(startPositionParent);

    this->setObjectName("AssemblyRuler");
}

void AssemblyRuler::connectSlots() {
    connect(browser, SIGNAL(si_zoomOperationPerformed()), SLOT(sl_redraw()));
    connect(browser, SIGNAL(si_offsetsChanged()), SLOT(sl_redraw()));
}

void AssemblyRuler::drawAll() {
    if(!model->isEmpty()) {
        QSize currentSize = size() * devicePixelRatio();
        if (cachedView.size() != currentSize) {
            cachedView = QPixmap(currentSize);
            cachedView.setDevicePixelRatio(devicePixelRatio());
            redraw = true;
        }
        if (redraw) {
            cachedView.fill(Qt::transparent);
            QPainter p(&cachedView);
            redraw = false;
            drawRuler(p);
        }
        QPixmap cachedViewCopy(cachedView);
        cachedViewCopy.setDevicePixelRatio(devicePixelRatio());
        {
            QPainter p(&cachedViewCopy);
            drawCursor(p);
        }
        QPainter p(this);
        p.drawPixmap(0, 0, cachedViewCopy);
    }
}

namespace {
    int numOfDigits(qint64 n) {
        assert(n >= 0);
        return QString::number(n).length();
    }
}

void AssemblyRuler::drawCursor(QPainter & p) {
    //1. draw the cursor itself
    p.setPen(Qt::darkRed);
    p.drawLine(cursorPos, BORDER_NOTCH_START, cursorPos, BORDER_NOTCH_END);
    p.drawLine(cursorPos+1, BORDER_NOTCH_START, cursorPos+1, BORDER_NOTCH_END);

    //2. find current position
    qint64 posXInAsm = browser->calcAsmPosX(cursorPos);


    //3. format the string, add coverage if needed
    // pos + 1 because of 1-based coords
    QString cursorLabel = FormatUtils::formatNumberWithSeparators(posXInAsm + 1);
    if(showCoverage) {
        qint32 coverage = browser->getCoverageAtPos(posXInAsm);
        if (coverage >=0){//not have info about coverage yet
            cursorLabel += " C " + FormatUtils::formatNumberWithSeparators(coverage);
        }
    }
    int textWidth = p.fontMetrics().width(cursorLabel);
    int textHeight = p.fontMetrics().height();
    QRect offsetRect(cursorPos - textWidth/2, LABELS_END, textWidth, textHeight);
    if(offsetRect.left() < 0) {
        offsetRect.moveLeft(0);
    }
    if(offsetRect.right() > width() - 1) {
        offsetRect.moveRight(width() - 1);
    }

    //4. draw cursor label
    p.drawText(offsetRect, Qt::AlignCenter, cursorLabel);
    startPositionObject->setObjectName(cursorLabel);

    if(!showCoords) {
        return;
    }

    //5. draw cached labels. Skip labels intersecting the cursor label
    assert(cachedLabelsRects.size() == cachedLabels.size());
    for(int i = 0; i < cachedLabels.size(); i++) {
        const QRect & labelRect = cachedLabelsRects.at(i);
        if(!labelRect.intersects(offsetRect)) {
            p.drawImage(labelRect, cachedLabels.at(i));
        }
    }
}

void AssemblyRuler::drawRuler(QPainter & p) {
    cachedLabelsRects.clear();
    cachedLabels.clear();
    p.setPen(Qt::black);

    // draw the axis + left and right border notches
    {
        // axis
        p.drawLine(0, AXIS_LINE_Y, width(), AXIS_LINE_Y);
        // borders
        p.drawLine(0, BORDER_NOTCH_START, 0, BORDER_NOTCH_END);
        p.drawLine(width()-1, BORDER_NOTCH_START, width()-1, BORDER_NOTCH_END);
    }

    int lettersPerZ = browser->calcAsmCoordX(50);
    int interval = pow((double)10, numOfDigits(lettersPerZ)-1); //interval between notches
    //int pixInterval = browser->calcPixelCoord(interval);

    int globalOffset = browser->getXOffsetInAssembly();
    qint64 firstLetterWithNotch = globalOffset - 1;
    while((firstLetterWithNotch + 1) % interval != 0) {
        ++firstLetterWithNotch;
    }

    int start = firstLetterWithNotch - globalOffset;
    int end = browser->basesCanBeVisible();

    int bigInterval = interval * 10; //interval between long notches
    int halfCell = browser->getCellWidth() / 2;
    int lastLabelRight = 0; //used to skip intersecting labels

    //iterate over notches to draw
    for(int i = start; i < end; i+=interval) {
        int x_pix = browser->calcPainterOffset(i) + halfCell;
        //draw long notches + labels for "big interval"
        int oneBasedOffset = globalOffset + i + 1;
        if(oneBasedOffset == 1 || oneBasedOffset % bigInterval == 0) {
            //draw long notch
            p.drawLine(x_pix, LONG_NOTCH_START, x_pix, LONG_NOTCH_END);

            //draw labels
            QString offsetStr = FormatUtils::formatNumberWithSeparators(oneBasedOffset);
            int textWidth = p.fontMetrics().width(offsetStr);
            int textHeight = p.fontMetrics().height();
            QRect offsetRect(x_pix - textWidth/2, LABELS_END, textWidth, textHeight);

            if(offsetRect.left() > lastLabelRight) {
                //render image with label and cache it. all images will be drawn on mouseMove event
                QImage img(textWidth * devicePixelRatio(), textHeight * devicePixelRatio(), QImage::Format_ARGB32);
                img.setDevicePixelRatio(devicePixelRatio());
                QPainter labelPainter(&img);
                img.fill(Qt::transparent);
                labelPainter.drawText(QRect(0, 0, textWidth, textHeight), Qt::AlignCenter, offsetStr);

                lastLabelRight = offsetRect.right() + 15; //prevent intersecting or too close labels

                cachedLabelsRects.append(offsetRect);
                cachedLabels.append(img);
            }
        } else {
            //draw short notches
            p.drawLine(x_pix, SHORT_NOTCH_START, x_pix, SHORT_NOTCH_END);
        }
    }
}

void AssemblyRuler::sl_handleMoveToPos(const QPoint & pos) {
    int cellWidth = browser->getCellWidth();
    if(cellWidth == 0) {
        cursorPos = pos.x();
        redraw = true;
        update();
    } else {
        //redraw cursor only if it points to the new cell
        int cellNumOld = cursorPos / cellWidth;
        int cellNumNew = pos.x() / cellWidth;
        if(cellNumOld != cellNumNew) {
            cursorPos = cellNumNew * cellWidth + cellWidth / 2;
            redraw = true;
            update();
        }
    }
}

void AssemblyRuler::paintEvent(QPaintEvent * e) {
    drawAll();
    QWidget::paintEvent(e);
}

void AssemblyRuler::mouseMoveEvent(QMouseEvent * e) {
    sl_handleMoveToPos(e->pos());
    QWidget::mouseMoveEvent(e);
}

void AssemblyRuler::sl_redraw() {
    cachedView = QPixmap(size() * devicePixelRatio());
    cachedView.setDevicePixelRatio(devicePixelRatio());
    redraw = true;
    update();
}

void AssemblyRuler::setShowCoordsOnRuler(bool sh) {
    AssemblyBrowserSettings::setShowCoordsOnRuler(sh);
    showCoords = sh;
    redraw = true;
    update();
}

bool AssemblyRuler::getShowCoordsOnRuler() const {
    return showCoords;
}

void AssemblyRuler::setShowCoverageOnRuler(bool value) {
    AssemblyBrowserSettings::setShowCoverageOnRuler(value);
    showCoverage = value;
    update();
}

bool AssemblyRuler::getShowCoverageOnRuler() const {
    return showCoverage;
}

}
