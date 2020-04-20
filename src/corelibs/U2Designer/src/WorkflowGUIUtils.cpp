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

#include "WorkflowGUIUtils.h"

#include <U2Lang/Descriptor.h>

#include <QPainter>
#include <QPalette>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include <QUrl>


namespace U2 {

void DesignerGUIUtils::paintSamplesArrow(QPainter* painter) {
    QPen pen(Qt::darkGray);
    pen.setWidthF(2);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    QFont f = painter->font();
    painter->resetTransform();
    f.setFamily("Times New Roman");
    f.setPointSizeF(20);
    f.setItalic(true);
    painter->setFont(f);

    QRectF approx(50,50, 400, 400);
    QString txt = QObject::tr("Select a sample to start");
    QRectF res = painter->boundingRect(approx, Qt::AlignLeft | Qt::AlignTop, txt);
    res.adjust(-5,-3,15,3);

    QPainterPath p(QPointF(5, res.center().y()));
    p.lineTo(res.topLeft());
    p.lineTo(res.topRight());
    p.lineTo(res.bottomRight());
    p.lineTo(res.bottomLeft());
    p.closeSubpath();
    QColor yc = QColor(255,255,160);//QColor(Qt::yellow).lighter();yc.setAlpha(127);
    painter->fillPath(p, QBrush(yc));
    painter->drawPath(p);
    painter->setPen(Qt::black);
    painter->drawText(approx, Qt::AlignLeft | Qt::AlignTop, txt);
}

void DesignerGUIUtils::paintSamplesDocument(QPainter* painter, QTextDocument* doc, int w, int h, const QPalette& pal) {
    int pageWidth = qMax(w - 100, 100);
    int pageHeight = qMax(h - 100, 100);
    if (pageWidth != doc->pageSize().width()) {
        doc->setPageSize(QSize(pageWidth, pageHeight));
    }

    QSize ts = doc->size().toSize();

    QRect textRect(w / 2 - pageWidth / 2,
        h / 2 - pageHeight / 2,
        pageWidth,
        pageHeight);
    textRect.setSize(ts);
    int pad = 10;
    QRect clearRect = textRect.adjusted(-pad, -pad, pad, pad);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 63));
    int shade = 10;
    painter->drawRect(clearRect.x() + clearRect.width() + 1,
        clearRect.y() + shade,
        shade,
        clearRect.height() + 1);
    painter->drawRect(clearRect.x() + shade,
        clearRect.y() + clearRect.height() + 1,
        clearRect.width() - shade + 1,
        shade);

    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setBrush(QColor(255, 255, 255/*, 220*/));
    painter->setPen(Qt::black);
    painter->drawRect(clearRect);

    painter->setClipRegion(textRect, Qt::IntersectClip);
    painter->translate(textRect.topLeft());

    QAbstractTextDocumentLayout::PaintContext ctx;

    QLinearGradient g(0, 0, 0, textRect.height());
    g.setColorAt(0, Qt::black);
    g.setColorAt(0.9, Qt::black);
    g.setColorAt(1, Qt::transparent);

    //pal.setBrush(QPalette::Text, g);

    ctx.palette = pal;
    ctx.clip = QRect(0, 0, textRect.width(), textRect.height());
    doc->documentLayout()->draw(painter, ctx);
}

void DesignerGUIUtils::setupSamplesDocument(const Descriptor& d, const QIcon& ico, QTextDocument* doc) {
    bool hasIcon = (ico.availableSizes().size() > 0);
    QString text =
            QString(hasIcon ?
                        "<html>"
                        "<table align='center' border='0' cellpadding='3' cellspacing='3'>"
                        "<tr><td colspan='2'><h1 align='center'>%1</h1></td></tr>"
                        "<tr>"
                            "<td valign='middle' width='20%'><img src=\"%2\"/></td>"
                            "<td valign='bottom'><br>%3</td></tr>"
                        "<tr><td colspan='2' valign='top'>%4<br></td></tr>"
                        "<tr><td colspan='2' bgcolor='gainsboro' align='center'><font color='maroon' size='+2' face='Courier'><b>%5</b></font></td></tr>"
                        "</table>"
                        "</html>"
                      :
                        "<html>"
                        "<table align='center' border='0' cellpadding='3' cellspacing='3'>"
                        "<tr><td><h1 align='center'>%1</h1></td></tr>"
                        "<tr>%2"
                            "<td valign='bottom'><br>%3</td></tr>"
                        "<tr><td valign='top' halign='right'>%4<br></td></tr>"
                        "<tr><td bgcolor='gainsboro' align='center'><font color='maroon' size='+2' face='Courier'><b>%5</b></font></td></tr>"
                        "</table>"
                        "</html>"
                        );
    QString img("img://img");

    if (hasIcon) {
        doc->addResource(QTextDocument::ImageResource, QUrl(img), ico.pixmap(200));
    }

    QString body = d.getDocumentation().toHtmlEscaped().replace("\n", "<br>");
    int brk = body.indexOf("<br><br>");
    int shift = 8;
    if (brk <= 0) {
        brk = body.indexOf("<br>");
        shift = 4;
    }
    QString body2;
    if (brk > 0) {
        body2 = body.mid(brk + shift);
        body = body.left(brk);
    }
    text = text.arg(d.getDisplayName()).arg(hasIcon ? img : "").arg(body).arg(body2)
        .arg(QObject::tr("Double click to load the sample"));
    doc->setHtml(text);
    QFont f;
    //f.setFamily("Times New Roman");
    f.setPointSizeF(12);
    doc->setDefaultFont(f);
}

}//namespace
