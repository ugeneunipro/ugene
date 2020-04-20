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

#ifndef CREATEELEMENTWITHSCRIPTDIALOGFILLER_H
#define CREATEELEMENTWITHSCRIPTDIALOGFILLER_H

#include "utils/GTUtilsDialog.h"

namespace U2{
using namespace HI;

class CreateElementWithScriptDialogFiller : public Filler
{
public:
    CreateElementWithScriptDialogFiller(HI::GUITestOpStatus& os, QString _name):Filler(os, "CreateScriptBlockDialog"),
        name(_name){}
    void commonScenario();
private:
    QString name;
};

class ScriptEditorDialogFiller: public Filler
{
public:
    ScriptEditorDialogFiller(HI::GUITestOpStatus& os, QString _url = "", QString _text = "", bool _checkSyntax = false, QString _checkSyntaxResult = ""): Filler(os, "ScriptEditorDialog"),
        url(_url),
        text(_text),
        checkSyntaxResult(_checkSyntaxResult),
        checkSyntax(_checkSyntax) {}
    void commonScenario();
private:
    QString url;
    QString text;
    QString checkSyntaxResult;
    bool checkSyntax;
};

class ScriptEditorDialogSyntaxChecker: public Filler
{
public:
    ScriptEditorDialogSyntaxChecker(HI::GUITestOpStatus& os, QString _text = "", QString _message = ""): Filler(os, "ScriptEditorDialog"),
        text(_text),
        message(_message){}
    void commonScenario();
private:
    QString text, message;
};
}

#endif // CREATEELEMENTWITHSCRIPTDIALOGFILLER_H
