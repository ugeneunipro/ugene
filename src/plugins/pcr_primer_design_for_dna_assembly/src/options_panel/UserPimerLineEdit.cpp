/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#include "UserPimerLineEdit.h"

#include <U2Core/U2SafePoints.h>

#include <QTextDocument>
#include <QEvent>

#include <QTextLayout>

namespace U2 {

UserPimerLineEdit::UserPimerLineEdit(QWidget* parent)
    : PrimerLineEdit(parent, false) {
    connect(this, SIGNAL(selectionChanged()), SLOT(sl_selectionChanged()));
    connect(this, SIGNAL(textEdited(const QString&)), SLOT(sl_textEdited(const QString&)));
    connect(this, SIGNAL(textChanged(const QString&)), SLOT(sl_textChanged(const QString&)));
}

void UserPimerLineEdit::setLeftEnd(const QString& leftEnd) {
    CHECK(!leftEnd.isEmpty(), );

    QString textWithoutLeft5End = text();
    if (leftEndSize != 0) {
        textWithoutLeft5End = textWithoutLeft5End.right(textWithoutLeft5End.size() - leftEndSize);
    }
    lastLeftEndSize = leftEndSize;
    leftEndSize = leftEnd.size();
    changeLeftEndColor = true;
    setText(leftEnd + textWithoutLeft5End);
}

void UserPimerLineEdit::setRightEnd(const QString& rightEnd) {
    CHECK(!rightEnd.isEmpty(), );

    QString textWithoutRight3End = text();
    if (rightEndSize != 0) {
        textWithoutRight3End = textWithoutRight3End.left(textWithoutRight3End.size() - rightEndSize);
    }
    lastRightEndSize = rightEndSize;
    rightEndSize = rightEnd.size();
    changeRightEndColor = true;
    setText(textWithoutRight3End + rightEnd);
}

UserPimerLineEditResult UserPimerLineEdit::getData() const {
    UserPimerLineEditResult res;
    res.text = text();
    res.leftEndSize = leftEndSize;
    res.rightEndSize = rightEndSize;
    return res;
}

void UserPimerLineEdit::setData(const UserPimerLineEditResult& data) {
    leftEndSize = data.leftEndSize;
    rightEndSize = data.rightEndSize;
    setText(data.text);
    updateColors();
}

void UserPimerLineEdit::sl_selectionChanged() {
    lastSelectionStart = selectionStart();
    lastSelectedTextSize = selectedText().size();
}

void UserPimerLineEdit::sl_textEdited(const QString& text) {
    if (lastSelectedTextSize == 0) {
        // We don't have a selection, so it's either
        // an insertion or deletion, but not both.
        int delta = text.size() - lastTextSize;
        if (delta > 0) {
            // User has inserted text.
            int pos = cursorPosition() - delta;
            for (int i = 0; i < delta; i++) {
                int primerSize = qMax(0, lastTextSize - (leftEndSize + rightEndSize));
                if (0 <= pos && pos < leftEndSize) {
                    leftEndSize++;
                } else if (leftEndSize + primerSize < pos && pos <= leftEndSize + primerSize + rightEndSize) {
                    rightEndSize++;
                }
            }
        } else {
            // User has erased text.
            int pos = cursorPosition();
            int primerSize = qMax(0, lastTextSize - (leftEndSize + rightEndSize));
            if (0 <= pos && pos < leftEndSize) {
                leftEndSize--;
            } else if (leftEndSize + primerSize <= pos && pos <= leftEndSize + primerSize + rightEndSize) {
                rightEndSize--;
            }
        }
    } else {
        // There was a selection, so we have both removed
        // and inserted text.
        int startSelectionPos = lastSelectionStart;
        int endSelectionPos = startSelectionPos + lastSelectedTextSize;
        int primerSize = qMax(0, lastTextSize - (leftEndSize + rightEndSize));
        int rightStartPos = leftEndSize + primerSize;
        int rightEndPos = leftEndSize + primerSize + rightEndSize;
        if (0 <= startSelectionPos && startSelectionPos < leftEndSize) {
            leftEndSize = startSelectionPos;
        }
        if (rightStartPos <= endSelectionPos && endSelectionPos <= rightEndPos) {
            rightEndSize = rightEndPos - endSelectionPos;
        }
    }
}

void UserPimerLineEdit::sl_textChanged(const QString& text) {
    if (lastSelectedTextSize == 0 && !changeLeftEndColor && !changeRightEndColor) {
        // We don't have a selection, so it's either
        // an insertion or deletion, but not both.
        int delta = text.size() - lastTextSize;
        if (delta > 0) {
            // User has inserted text.
            int pos = cursorPosition() - delta;
            for (int i = 0; i < delta; i++) {
                colors.insert(pos, getColorForInsertedText(pos));
            }
        } else {
            // User has erased text.
            int pos = cursorPosition();
            colors.erase(colors.begin() + pos, colors.begin() + pos - delta);
        }
    } else {
        // There was a selection, so we have both removed
        // and inserted text.
        int pos = lastSelectionStart;
        int removedCount = lastSelectedTextSize;
        int insertedCount = cursorPosition() - pos;
        if (changeLeftEndColor) {
            pos = 0;
            removedCount = lastLeftEndSize;
            insertedCount = leftEndSize;
        } else if (changeRightEndColor) {
            pos = leftEndSize + qMax(0, lastTextSize - (leftEndSize + lastRightEndSize));
            removedCount = lastRightEndSize;
            insertedCount = rightEndSize;
        }

        colors.erase(colors.begin() + pos, colors.begin() + pos + removedCount);
        for (int i = 0; i < insertedCount; i++) {
            colors.insert(pos, getColorForInsertedText(pos));
        }
    }
    changeLeftEndColor = false;
    changeRightEndColor = false;

    setCharColors(colors);
}

void UserPimerLineEdit::setCharColors(const QList<QColor>& newColors) {
    QList<QInputMethodEvent::Attribute> attributes;
    int size = newColors.size();
    attributes.reserve(size);
    for (int i = 0; i < size; i++) {
        if (newColors[i].isValid()) {
            QTextCharFormat charFormat;
            charFormat.setForeground(QBrush(newColors[i]));
            const int start = i - cursorPosition();
            const int length = 1;
            attributes.append(QInputMethodEvent::Attribute(QInputMethodEvent::TextFormat, start, length, charFormat));
        }
    }
    QInputMethodEvent ime(QString(), attributes);
    QLineEdit::inputMethodEvent(&ime);

    lastTextSize = text().size();
    colors = newColors;
}

QColor UserPimerLineEdit::getColorForInsertedText(int pos) const {
    QColor resultColor;
    int primerSize = qMax(0, lastTextSize - (leftEndSize + rightEndSize));
    if (changeLeftEndColor || (0 <= pos && pos < leftEndSize)) {
        resultColor = Qt::red;
    } else if (!changeLeftEndColor && !changeRightEndColor && leftEndSize <= pos && pos <= leftEndSize + primerSize) {
        resultColor = Qt::black;
    } else if (changeRightEndColor || (leftEndSize + primerSize <= pos && pos <= leftEndSize + primerSize + rightEndSize)) {
        resultColor = Qt::darkGreen;
    } else {
        FAIL("Unexpected pos", Qt::black);
    }

    return resultColor;
}

void UserPimerLineEdit::updateColors() {
    QList<QColor> newColors;
    for (int i = 0; i < text().size(); i++) {
        newColors.insert(i, getColorForInsertedText(i));
    }
    setCharColors(newColors);
}

}
