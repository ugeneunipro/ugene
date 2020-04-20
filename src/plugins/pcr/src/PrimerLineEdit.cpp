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

#include <QPainter>
#include <QStyleOption>

#include <U2Core/AppContext.h>
#include <U2Core/U2AlphabetUtils.h>

#include "PrimerLineEdit.h"

namespace U2 {

PrimerLineEdit::PrimerLineEdit(QWidget *parent)
: QLineEdit(parent)
{
    setValidator(new PrimerValidator(this));
}

void PrimerLineEdit::setInvalidatedText(const QString &text) {
    QString result = text;
    int pos = 0;
    if (QValidator::Acceptable != validator()->validate(result, pos)) {
        result = "";
    }
    setText(result);
}

void PrimerLineEdit::paintEvent(QPaintEvent *event) {
    QLineEdit::paintEvent(event);
    if (!text().isEmpty()) {
        return;
    }
    QPainter p(this);
    QColor col = palette().text().color();
    col.setAlpha(128);
    p.setPen(col);

    QRect r = placeHolderRect();

    QString left = fontMetrics().elidedText("5'", Qt::ElideRight, r.width());
    Qt::Alignment leftAlignment = QStyle::visualAlignment(Qt::LeftToRight, QFlag(Qt::AlignLeft));
    p.drawText(r, leftAlignment, left);

    QString right = fontMetrics().elidedText("3'", Qt::ElideRight, r.width());
    Qt::Alignment rightAlignment = QStyle::visualAlignment(Qt::LeftToRight, QFlag(Qt::AlignRight));
    p.drawText(r, rightAlignment, right);
}

QRect PrimerLineEdit::placeHolderRect() const {
    QStyleOptionFrame panel;
    initStyleOption(&panel);
    QRect r = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
    r.setX(r.x() + textMargins().left());
    r.setY(r.y() + textMargins().top());
    r.setRight(r.right() - textMargins().right());
    r.setBottom(r.bottom() - textMargins().bottom());

    QFontMetrics fm = fontMetrics();
    int minLB = qMax(0, -fm.minLeftBearing());
    int minRB = qMax(0, -fm.minRightBearing());
    int vscroll = r.y() + (r.height() - fm.height() + 1) / 2;
    static const int horizontalMargin = 2; // QLineEditPrivate::horizontalMargin
    QRect lineRect(r.x() + horizontalMargin, vscroll, r.width() - 2*horizontalMargin, fm.height());
    return lineRect.adjusted(minLB, 0, -minRB, 0);
}

PrimerValidator::PrimerValidator(QObject *parent, bool allowExtended)
: QRegExpValidator(parent)
{
    const DNAAlphabet* alphabet = AppContext::getDNAAlphabetRegistry()->findById(
                allowExtended ? BaseDNAAlphabetIds::NUCL_DNA_EXTENDED() : BaseDNAAlphabetIds::NUCL_DNA_DEFAULT());
    QByteArray alphabetChars = alphabet->getAlphabetChars(true);
    // Gaps are not allowed
    alphabetChars.remove(alphabetChars.indexOf('-'), 1);
    setRegExp(QRegExp(QString("[%1]+").arg(alphabetChars.constData())));
}

QValidator::State PrimerValidator::validate(QString &input, int &pos) const {
    input = input.simplified();
    input = input.toUpper();
    input.remove(" ");
    return QRegExpValidator::validate(input, pos);
}

} // U2
