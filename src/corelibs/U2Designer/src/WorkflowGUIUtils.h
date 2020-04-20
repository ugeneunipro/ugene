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

#ifndef _U2_WORKFLOW_GUI_UTILS_H_
#define _U2_WORKFLOW_GUI_UTILS_H_

#include <U2Core/global.h>

class QPainter;
class QTextDocument;
class QPalette;
class QIcon;

namespace U2 {

class Descriptor;

class U2DESIGNER_EXPORT DesignerGUIUtils {
public:
    static void paintSamplesArrow(QPainter* painter);
    static void paintSamplesDocument(QPainter* painter, QTextDocument* doc, int pageWidth, int pageHeight, const QPalette& pal);
    static void setupSamplesDocument(const Descriptor& d, const QIcon& ico, QTextDocument* res);
};

}//namespace

#endif
