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

#ifndef _U2_SCRIPT_EDITOR_DELEGATE_H_
#define _U2_SCRIPT_EDITOR_DELEGATE_H_

#include <QWidget>

class QBoxLayout;
class QLineEdit;
class QTextEdit;

namespace U2 {

enum ScriptEditorType {
    TEXT_EDIT,
    LINE_EDIT
};

class AbstractScriptEditorDelegate : public QWidget {
    Q_OBJECT
public:
    AbstractScriptEditorDelegate(QWidget *parent);
    virtual ~AbstractScriptEditorDelegate();

    virtual void setText(const QString &text) = 0;
    virtual QString text() const = 0;
    virtual int cursorLineNumber() const = 0;
    virtual void installScriptHighlighter() const;

    static AbstractScriptEditorDelegate * createInstance(QWidget *parent, ScriptEditorType type);

signals:
    void si_textChanged();
    void si_cursorPositionChanged();

protected:
    QBoxLayout *layout;
};

//////////////////////////////////////////////////////////////////////////
////       Implementations       /////////////////////////////////////////

class LineEditDelegate : public AbstractScriptEditorDelegate {
public:
    LineEditDelegate(QWidget *parent);

    void setText(const QString &text);
    QString text() const;
    int cursorLineNumber() const;

private:
    QLineEdit *edit;
};

class TextEditDelegate : public AbstractScriptEditorDelegate {
    Q_OBJECT
public:
    TextEditDelegate(QWidget *parent);

    void setText(const QString &text);
    QString text() const;
    int cursorLineNumber() const;
    void installScriptHighlighter() const;

private:
    QTextEdit *edit;
};

}

#endif // _U2_SCRIPT_EDITOR_DELEGATE_H_
