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

#include <QVBoxLayout>

#include "AssemblyVariantRow.h"

#include "AssemblyVariantHint.h"

namespace U2 {

AssemblyVariantHint::AssemblyVariantHint(QWidget *parent)
: AssemblyReadsAreaHint(parent)
{

}

void AssemblyVariantHint::setData(const QList<U2Variant> &varList) {
    QString text;
    QListIterator<U2Variant> it(varList);
    while(it.hasNext()) {
        const U2Variant &v = it.next();
        text += "<table cellspacing=\"0\" cellpadding=\"0\" align=\"left\" width=\"20%\">";

        if (!v.publicId.isEmpty()) {
            text += QString("<tr><td><b>%1</b></td></tr>").arg(v.publicId);
        }
        QString position("<tr><td><b>%1:&nbsp;</b></td><td>%2</td></tr>");
        if (AssemblyVariantRow::isSNP(v)) {
            position = position.arg(tr("SNP position"));
        } else {
            position = position.arg(tr("Variant start position"));
        }
        text += position.arg(v.startPos+1);
        text += QString("<tr><td><b>%1:&nbsp;</b></td><td>%2</td></tr>").arg(tr("Source nucleotides")).arg(QString(v.refData));
        text += QString("<tr><td><b>%1:&nbsp;</b></td><td>%2</td></tr>").arg(tr("Variant nucleotides")).arg(QString(v.obsData));

        text += "</table>";

        if (it.hasNext()) {
            text += "<hr>";
        }
    }
    label->setText(text);
    this->setMaximumHeight(layout()->minimumSize().height());
}

void AssemblyVariantHint::leaveEvent(QEvent *) {
    AssemblyVariantRow *p = qobject_cast<AssemblyVariantRow*>(parent());
    QPoint curInParentCoords = p->mapFromGlobal(QCursor::pos());
    if(!p->rect().contains(curInParentCoords)) {
        p->sl_hideHint();
    }
}

void AssemblyVariantHint::mouseMoveEvent(QMouseEvent * e) {
    AssemblyVariantRow *p = qobject_cast<AssemblyVariantRow*>(parent());
    p->sl_hideHint();
    QFrame::mouseMoveEvent(e);
}

} // U2
