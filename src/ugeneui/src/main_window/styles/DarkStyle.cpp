/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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

#include "DarkStyle.h"

#include <U2Core/U2SafePoints.h>


DarkStyle::DarkStyle(QStyle* style)
    : QProxyStyle(style) {}

DarkStyle::~DarkStyle() {
    qApp->setStyleSheet("");
}

QIcon DarkStyle::standardIcon(QStyle::StandardPixmap standardIcon, const QStyleOption* option, const QWidget* widget) const {
    if (standardIcon == QStyle::SP_ToolBarHorizontalExtensionButton) {
        return QIcon(":/darkstyle/icon_tooltip_extend_horizontal.png");
    } else if (standardIcon == QStyle::SP_ToolBarVerticalExtensionButton) {
        return QIcon(":/darkstyle/icon_tooltip_extend_vertical.png");
    }
    return QProxyStyle::standardIcon(standardIcon, option, widget);
}

void DarkStyle::polish(QPalette& palette) {
    // modify palette to dark
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Active, QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::WindowText,
                     QColor(127, 127, 127));
    palette.setColor(QPalette::Base, QColor(48, 48, 48));
    palette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, QColor(53, 53, 53));
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    palette.setColor(QPalette::Midlight, QColor(28, 28, 28));
    palette.setColor(QPalette::Mid, QColor(95, 95, 95));
    palette.setColor(QPalette::Shadow, QColor(210, 210, 210));
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText,
                     QColor(127, 127, 127));
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(173, 216, 230));
    palette.setColor(QPalette::LinkVisited, QColor(157, 139, 214));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                     QColor(127, 127, 127));
}

void DarkStyle::polish(QApplication *app) {
    CHECK(app != nullptr, );;

    // loadstylesheet
    QFile darkstyle(QStringLiteral(":/darkstyle/darkstyle.qss"));
    if (darkstyle.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // set stylesheet
        QString qsStylesheet = QString::fromLatin1(darkstyle.readAll());
        app->setStyleSheet(qsStylesheet);
        darkstyle.close();
    }
}
