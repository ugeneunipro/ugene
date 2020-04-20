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

#ifndef _GTUTILS_PCR_H_
#define _GTUTILS_PCR_H_

#include <U2Core/U2OpStatus.h>
#include <U2Core/U2Type.h>
#include <GTGlobals.h>

class QTableView;

namespace U2 {
using namespace HI;

class GTUtilsPcr {
public:
    static void setPrimer(HI::GUITestOpStatus &os, U2Strand::Direction direction, const QByteArray &primer);
    static void setMismatches(HI::GUITestOpStatus &os, U2Strand::Direction direction, int mismatches);
    static void setPerfectMatch(HI::GUITestOpStatus &os, int number);
    static void setMaxProductSize(HI::GUITestOpStatus &os, int number);
    static QWidget * browseButton(HI::GUITestOpStatus &os, U2Strand::Direction direction);
    static int productsCount(HI::GUITestOpStatus &os);
    static QString getResultRegion(HI::GUITestOpStatus &os, int number);
    static QPoint getResultPoint(HI::GUITestOpStatus &os, int number);
    static QPoint getDetailsPoint(HI::GUITestOpStatus &os);
    static QString getPrimerInfo(HI::GUITestOpStatus &os, U2Strand::Direction direction);
    static QWidget * primerBox(HI::GUITestOpStatus &os, U2Strand::Direction direction);
    static void clearPcrDir(HI::GUITestOpStatus &os);

private:
    static QTableView * table(HI::GUITestOpStatus &os);
};

} // U2

#endif // _GTUTILS_PCR_H_
