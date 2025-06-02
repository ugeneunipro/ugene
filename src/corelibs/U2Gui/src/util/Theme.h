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

#include <QColor>

#include <U2Core/AppContext.h>
#include <U2Core/global.h>

#include <U2Gui/MainWindow.h>

namespace U2 {

class U2GUI_EXPORT Theme : public QObject {
    Q_OBJECT
public:
    static QColor errorColorTextFieldColor() {
        return AppContext::getMainWindow()->isDarkTheme() ? QColor(206, 20, 0) : QColor(255, 200, 200);
    }

    static QString errorColorTextFieldStr() {
        auto errorColorTextField = errorColorTextFieldColor();
        return QString("rgb(%1, %2, %3)").arg(errorColorTextField.red()).arg(errorColorTextField.green()).arg(errorColorTextField.blue());
    }

    static QColor errorColorLabelColor() {
        return AppContext::getMainWindow()->isDarkTheme() ? QColor(255, 127, 127) : QColor(166, 57, 46);
    }
    static QString errorColorLabelStr() {
        auto errorColorLabel = errorColorLabelColor();
        return QString("rgb(%1, %2, %3)").arg(errorColorLabel.red()).arg(errorColorLabel.green()).arg(errorColorLabel.blue());
    }
    static QString errorColorLabelHtmlStr() {
        return AppContext::getMainWindow()->isDarkTheme() ? "#FF7F7F" : "#A6392E";
    }  // the same as errorColorLabelStr()

    static QColor wdParameterLabelColor() {
        return AppContext::getMainWindow()->isDarkTheme() ? QColor(255, 127, 127) : QColor(255, 0, 0);
    }

    static QString wdParameterLabelStr() {
        return wdParameterLabelColor().name();
    }

    static QString wdWizardTopicColorStr() {
        return AppContext::getMainWindow()->isDarkTheme() ? "#7ABCFF" : "#0c3762";
    }

    static QString warningColorLabelHtmlStr() {
        return AppContext::getMainWindow()->isDarkTheme() ? "#FFCF48" : "#FF8B19";
    }

    static QColor infoHintColor() {
        return AppContext::getMainWindow()->isDarkTheme() ? QColor("#47D452") : QColor("green");
    }
    static QString infoColorLabelHtmlStr() {
        return infoHintColor().name();
    }
    static QString infoHintStyleSheet() {
        return QString("color: %1; font: bold").arg(infoHintColor().name());
    }

    static QColor successColor() {
        return infoHintColor();
    }
    static QString successColorLabelHtmlStr() {
        return successColor().name();
    }
    static QString successColorLabelStr() {
        return QString("rgb(%1, %2, %3)").arg(successColor().red()).arg(successColor().green()).arg(successColor().blue());
    }

    static QColor selectionBackgroundColor() {
        return AppContext::getMainWindow()->isDarkTheme() ? QColor("#4D4E51") : QColor("#EAEDF7");
    }

    static QString linkColorLabelStr() {
        QColor result = QPalette().shadow().color();
#ifdef Q_OS_DARWIN
        if (AppContext::getMainWindow()->isDarkTheme()) {
            result = QColor(190, 190, 190);
        } else {
            result = Qt::gray;
        }
#endif
        return result.name();
    }

    static QString hyperlinkColorLabelHtmlStr() {
        return AppContext::getMainWindow()->isDarkTheme() ? "#add8e6" : QColor(Qt::blue).name();
    }

};

}  // namespace U2
