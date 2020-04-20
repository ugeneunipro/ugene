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

#ifndef _U2_THEME_H_
#define _U2_THEME_H_

#include <QColor>

#include <U2Core/global.h>

namespace U2 {

class U2CORE_EXPORT Theme: public QObject {
    Q_OBJECT
public:
    static QString errorColorTextFieldStr() { return "rgb(255, 152, 142)"; }
    static QString errorColorLabelStr() { return "rgb(166, 57, 46)"; }
    static QString errorColorLabelHtmlStr() { return "#A6392E"; }   // the same as errorColorLabelStr()

    static QString warningColorLabelHtmlStr() { return "#FF8B19"; }

    static QColor infoHintColor() { return QColor("green"); }
    static QString infoColorLabelHtmlStr() { return "#218F20"; }
    static QString infoHintStyleSheet() { return QString("color: %1; font: bold").arg(infoHintColor().name()); }

    static QColor successColor() { return QColor("green"); }
    static QString successColorLabelHtmlStr() { return successColor().name(); }
    static QString successColorLabelStr() { return QString("rgb(%1, %2, %3)").arg(successColor().red()).arg(successColor().green()).arg(successColor().blue()); }

    static QColor selectionBackgroundColor() { return QColor("#EAEDF7"); }

    static QString linkColorLabelStr() {
#ifdef Q_OS_MAC
        return "gray";
#else
        return "palette(shadow)";
#endif
    }
};

} // namespace U2

#endif
