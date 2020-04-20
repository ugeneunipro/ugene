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

#include <QLineEdit>
#include <QTextEdit>
#include <QBoxLayout>

#include "ScriptHighlighter.h"

#include "ScriptEditorDelegate.h"

namespace U2 {

//////////////////////////////////////////////////////////////////////////
////AbstractScriptEditorDelegate /////////////////////////////////////////

AbstractScriptEditorDelegate::AbstractScriptEditorDelegate(QWidget *parent) : QWidget(parent) {
    layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->setMargin(0);
}

AbstractScriptEditorDelegate::~AbstractScriptEditorDelegate() {

}

void AbstractScriptEditorDelegate::installScriptHighlighter() const {

}

AbstractScriptEditorDelegate * AbstractScriptEditorDelegate::createInstance(QWidget *parent,
    ScriptEditorType type)
{
    AbstractScriptEditorDelegate *result = NULL;
    switch (type) {
    case LINE_EDIT:
        result = new LineEditDelegate(parent);
        break;
    case TEXT_EDIT:
        result = new TextEditDelegate(parent);
        break;
    default:
        Q_ASSERT(false);
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
////       LineEditDelegate      /////////////////////////////////////////

LineEditDelegate::LineEditDelegate(QWidget *parent) : AbstractScriptEditorDelegate(parent) {
    edit = new QLineEdit(this);
    layout->addWidget(edit);
}

void LineEditDelegate::setText(const QString &text) {
    edit->setText(text);
}

QString LineEditDelegate::text() const {
    return edit->text();
}

int LineEditDelegate::cursorLineNumber() const {
    return 0;
}

//////////////////////////////////////////////////////////////////////////
////       TextEditDelegate      /////////////////////////////////////////

TextEditDelegate::TextEditDelegate(QWidget *parent) : AbstractScriptEditorDelegate(parent) {
    edit = new QTextEdit(this);
    layout->addWidget(edit);
    connect(edit, SIGNAL(textChanged()), SIGNAL(si_textChanged()));
    connect(edit, SIGNAL(cursorPositionChanged()), SIGNAL(si_cursorPositionChanged()));
}

void TextEditDelegate::setText(const QString &text) {
    edit->setText(text);
}

QString TextEditDelegate::text() const {
    return edit->toPlainText();
}

int TextEditDelegate::cursorLineNumber() const {
    return edit->textCursor().block().blockNumber();
}

void TextEditDelegate::installScriptHighlighter() const {
    new ScriptHighlighter(edit->document());
}

} // namespace U2
