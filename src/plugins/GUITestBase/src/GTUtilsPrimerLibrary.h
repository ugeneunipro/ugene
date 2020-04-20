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

#ifndef _GTUTILS_PRIMER_LIBRARY_H_
#define _GTUTILS_PRIMER_LIBRARY_H_

#include <U2Core/U2OpStatus.h>
#include <GTGlobals.h>

class QAbstractButton;
class QTableView;

namespace U2 {

class GTUtilsPrimerLibrary {
public:
    enum Button {Add, Edit, Remove, Import, Export, Close};
    static QWidget * openLibrary(HI::GUITestOpStatus &os);
    static void clickButton(HI::GUITestOpStatus &os, Button button);
    static QAbstractButton * getButton(HI::GUITestOpStatus &os, Button button);
    static int librarySize(HI::GUITestOpStatus &os);
    static QString getPrimerSequence(HI::GUITestOpStatus &os, int number);
    static QString getPrimerSequence(HI::GUITestOpStatus &os, const QString &name);
    static QPoint getPrimerPoint(HI::GUITestOpStatus &os, int number);
    static void clickPrimer(HI::GUITestOpStatus &os, int number);
    static void clearLibrary(HI::GUITestOpStatus &os);
    static void addPrimer(HI::GUITestOpStatus &os, const QString &name, const QString &data);
    static void selectPrimers(HI::GUITestOpStatus &os, const QList<int> &numbers);
    static void selectAll(HI::GUITestOpStatus &os);

private:
    static QTableView * table(HI::GUITestOpStatus &os);
};

} // U2

#endif // _GTUTILS_PRIMER_LIBRARY_H_
