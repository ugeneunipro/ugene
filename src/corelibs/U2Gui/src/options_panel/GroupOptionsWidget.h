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

#ifndef _U2_GROUP_OPTIONS_WIDGET_H_
#define _U2_GROUP_OPTIONS_WIDGET_H_

#include <QWidget>

#include <U2Core/global.h>

class QLabel;
class QVBoxLayout;

namespace U2 {

/**
 * Widget with some options shown when a group header has been pressed.
 */
class U2GUI_EXPORT GroupOptionsWidget : public QWidget
{
    Q_OBJECT
public:
    GroupOptionsWidget(const QString& groupId, const QString& title, const QString& documentationPage, QWidget* widget);

    inline const QString& getGroupId() { return groupId; }
    inline static int getMinWidgetWidth() { return MIN_WIDGET_WIDTH; }
    inline static int getMaxWidgetWidth() { return MAX_WIDGET_WIDTH; }
    inline const QString& getTitle() { return title; }

private:
    QString     groupId;
    QWidget*    widget;
    QLabel*     titleWidget;
    QString     title;

    QVBoxLayout*        mainLayout;

    static const int TITLE_HEIGHT = 30;
    static const int MIN_WIDGET_WIDTH = 300;
    static const int MAX_WIDGET_WIDTH = 500;
};

} // namespace

#endif
