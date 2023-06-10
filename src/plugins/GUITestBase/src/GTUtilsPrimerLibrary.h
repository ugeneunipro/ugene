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

class QAbstractButton;
class QTableView;

namespace U2 {

class GTUtilsPrimerLibrary {
public:
    enum Button { Add,
                  Edit,
                  Remove,
                  Import,
                  Export,
                  Close,
                  Temperature };
    static QWidget* openLibrary();
    static void clickButton(Button button);
    static QAbstractButton* getButton(Button button);
    static int librarySize();
    static QString getPrimerSequence(int number);
    static QString getPrimerSequence(const QString& name);
    static QPoint getPrimerPoint(int number);
    static void clickPrimer(int number);
    static void clearLibrary();
    static void addPrimer(const QString& name, const QString& data);
    static void selectPrimers(const QList<int>& numbers);
    static void selectAll();

private:
    static QTableView* table();
};

}  // namespace U2
