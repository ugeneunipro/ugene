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

#pragma once

#include <GTGlobals.h>

#include <U2Core/U2OpStatus.h>
#include <U2Core/U2Type.h>

class QTableView;

namespace U2 {
using namespace HI;

class GTUtilsPcr {
public:
    static void setPrimer(U2Strand::Direction direction, const QByteArray& primer);
    static void setMismatches(U2Strand::Direction direction, int mismatches);
    static void setPerfectMatch(int number);
    static void setMaxProductSize(int number);
    static void setUseAmbiguousBases(bool useAmbiguousBases);
    static QWidget* browseButton(U2Strand::Direction direction);
    static int productsCount();
    static QString getResultRegion(int number);
    static QPoint getResultPoint(int number);
    static QPoint getDetailsPoint();
    static QString getPrimerInfo(U2Strand::Direction direction);
    static QWidget* primerBox(U2Strand::Direction direction);
    static void clearPcrDir();

private:
    static QTableView* getTable();
};

}  // namespace U2
