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

#include "SuggestCompleter.h"

#include <QHeaderView>
#include <QKeyEvent>
#include <QTreeWidget>

#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>

const int INVALID_ITEM_INDEX = -1;

namespace U2 {

BaseCompleter::BaseCompleter(CompletionFiller* filler, QLineEdit* parent /* = 0*/)
    : QObject(parent), filler(filler), editor(parent), lastChosenItemIndex(INVALID_ITEM_INDEX) {
    popup = new QTreeWidget(parent);
    popup->setWindowFlags(Qt::Popup);
    popup->setFocusPolicy(Qt::NoFocus);
    popup->setFocusProxy(parent);
    popup->setMouseTracking(true);

    popup->setColumnCount(1);
    popup->setUniformRowHeights(true);
    popup->setRootIsDecorated(false);
    popup->setEditTriggers(QTreeWidget::NoEditTriggers);
    popup->setSelectionBehavior(QTreeWidget::SelectRows);
    popup->setFrameStyle(QFrame::Box | QFrame::Plain);
    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->header()->hide();

    popup->installEventFilter(this);
    editor->installEventFilter(this);

    connect(popup, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(doneCompletion()));
    connect(editor, SIGNAL(textChanged(QString)), SLOT(sl_textChanged(QString)));
}

BaseCompleter::~BaseCompleter() {
    delete filler;
}

bool BaseCompleter::eventFilter(QObject* obj, QEvent* ev) {
    QEvent::Type eventType = ev->type();
    if (obj == editor) {
        if (eventType == QEvent::FocusOut) {
            QFocusEvent* focusEvent = static_cast<QFocusEvent*>(ev);
            if (focusEvent->reason() == Qt::PopupFocusReason) {
                return true;
            }
        }
        return false;
    }

    if (obj != popup) {
        return false;
    }

    if (eventType == QEvent::MouseButtonPress) {
        popup->hide();
        emit si_completerClosed();
        return false;
    }

    bool isConsumed = false;
    if (eventType == QEvent::KeyPress || eventType == QEvent::ShortcutOverride) {
        int key = static_cast<QKeyEvent*>(ev)->key();
        switch (key) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
                isConsumed = true;
                if (key == Qt::Key_Enter || key == Qt::Key_Return) {
                    doneCompletion();
                }
                popup->hide();
                editor->setFocus();
                emit si_completerClosed();
                break;
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Home:
            case Qt::Key_End:
            case Qt::Key_PageUp:
            case Qt::Key_PageDown:
                break;
            default:
                editor->setFocus();
                editor->event(ev);
        }
    }
    return isConsumed;
}

void BaseCompleter::showCompletion(const QStringList& choices) {
    if (choices.isEmpty()) {
        popup->hide();
        return;
    }

    popup->setUpdatesEnabled(false);
    popup->clear();
    for (int i = 0; i < choices.count(); ++i) {
        QTreeWidgetItem* item;
        item = new QTreeWidgetItem(popup);
        item->setText(0, choices[i]);
    }
    popup->setCurrentItem(popup->topLevelItem(0));
    popup->resizeColumnToContents(0);
    popup->adjustSize();
    popup->setUpdatesEnabled(true);

    int h = popup->sizeHintForRow(0) * qMin(10, choices.count()) + 3;
    popup->resize(editor->width(), h);
    popup->move(editor->mapToGlobal(QPoint(0, editor->height())));
    popup->show();
}

void BaseCompleter::doneCompletion() {
    popup->hide();
    editor->setFocus();
    QTreeWidgetItem* item = popup->currentItem();
    if (item) {
        QString result = filler->finalyze(editor->text(), item->text(0));
        editor->setText(result);
        editor->setCursorPosition(0);
        lastChosenItemIndex = popup->currentIndex().row();
        emit si_editingFinished();
    }
}

void BaseCompleter::sl_textChanged(const QString& typedText) {
    if (typedText.isEmpty()) {
        popup->hide();
        return;
    }
    QStringList suggestions = filler->getSuggestions(typedText);
    bool haveSingleValidChoice = suggestions.length() == 1 && suggestions.first() == typedText;
    if (haveSingleValidChoice) {
        lastChosenItemIndex = 0;
        emit si_editingFinished();
    } else {
        showCompletion(filler->getSuggestions(typedText));
    }
}

int BaseCompleter::getLastChosenItemIndex() const {
    return lastChosenItemIndex;
}

QStringList MSACompletionFiller::getSuggestions(const QString& userText) {
    QStringList result;
    QString userTextLc = userText.toLower();  // TODO: does toLower work correctly for non-Latin1 characters range?
    for (QString sequenceName : qAsConst(seqNameList)) {
        QString sequenceNameLc = sequenceName.toLower();
        if (sequenceNameLc.startsWith(userTextLc)) {
            result.append(sequenceName);
        }
    }
    if (result.isEmpty()) {
        result.append(defaultValue);
    }
    return result;
}

QString MSACompletionFiller::finalyze(const QString& /*editorText*/, const QString& suggestion) {
    return suggestion;
}

}  // namespace U2
