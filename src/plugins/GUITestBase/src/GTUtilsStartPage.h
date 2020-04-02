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


#ifndef _U2_GT_UTILS_START_PAGE_
#define _U2_GT_UTILS_START_PAGE_

#include "GTGlobals.h"
#include <primitives/GTWebView.h>

#include <U2Gui/U2WebView.h>

namespace U2 {
using namespace HI;

class GTUtilsStartPage
{
public:
    enum Buttons{OpenFile, CreateSequence, CreateWorkflow, QuickStartGuide};
    static void openStartPage(HI::GUITestOpStatus &os);
    static U2WebView* getStartPage(HI::GUITestOpStatus &os);
    static HIWebElement getButton(HI::GUITestOpStatus &os, Buttons button);
    static void clickButton(HI::GUITestOpStatus &os, Buttons button);
    static void clickResentDocument(HI::GUITestOpStatus &os, QString text);
    static void clickResentProject(HI::GUITestOpStatus &os, QString text);
};

}

#endif // _U2_GT_UTILS_START_PAGE_
