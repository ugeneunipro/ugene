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

#ifndef _U2_SCRIPT_EDITOR_DIALOG_H_
#define _U2_SCRIPT_EDITOR_DIALOG_H_

#include <U2Core/global.h>

#include <QDialog>

class Ui_ScriptEditorDialog;

namespace U2 {

class ScriptEditorWidget;

class U2GUI_EXPORT ScriptEditorDialog : public QDialog {
    Q_OBJECT
public:
    ScriptEditorDialog(QWidget* p, const QString& roHeaderText, const QString & text = QString());
    ~ScriptEditorDialog();

    void setScriptText(const QString& text);

    void setScriptPath(const QString& path);

    QString getScriptText() const;

    static QString getScriptsFileFilter();

private slots:
    void sl_checkSyntax();
    void sl_openScript();
    void sl_saveScript();
    void sl_saveAsScript();
    void sl_nameChanged(const QString &);
    void sl_scriptChanged();
    void sl_cursorPositionChanged();

private:
    void updateState();
    void save(const QString &url);

    ScriptEditorWidget *scriptEdit;
    Ui_ScriptEditorDialog *ui;
};

} //namespace

#endif
