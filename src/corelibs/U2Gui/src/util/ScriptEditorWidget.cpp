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
#include <QSplitter>
#include <QBoxLayout>

#include "ScriptHighlighter.h"
#include "ScriptEditorWidget.h"

const char *SCRIPT_TEXT_PROPERTY_NAME = "script text";

namespace U2 {

ScriptEditorWidget::ScriptEditorWidget(QWidget *parent, ScriptEditorType typeOfField)
    : QWidget(parent)
{
    scriptContainer = new QSplitter(Qt::Vertical, this);
    scriptContainer->setFocusPolicy(Qt::NoFocus);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    layout->setMargin(0);
    layout->addWidget(scriptContainer);

    variablesEdit = new QTextEdit(scriptContainer);
    variablesEdit->setReadOnly(true);

    new ScriptHighlighter(variablesEdit->document());

    scriptEdit = AbstractScriptEditorDelegate::createInstance(scriptContainer, typeOfField);
    scriptEdit->installScriptHighlighter();
    connect(scriptEdit, SIGNAL(si_textChanged()), SIGNAL(si_textChanged()));
    connect(scriptEdit, SIGNAL(si_cursorPositionChanged()), SIGNAL(si_cursorPositionChanged()));
}

void ScriptEditorWidget::setVariablesText(const QString &variablesText) {
    variablesEdit->setText(variablesText);
}

QString ScriptEditorWidget::variablesText() const {
    return variablesEdit->toPlainText();
}

void ScriptEditorWidget::setScriptText(const QString &text) {
    scriptEdit->setText(text);
}

QString ScriptEditorWidget::scriptText() const {
    return scriptEdit->text();
}

int ScriptEditorWidget::scriptEditCursorLineNumber() const {
    return scriptEdit->cursorLineNumber();
}

} // namespace U2
