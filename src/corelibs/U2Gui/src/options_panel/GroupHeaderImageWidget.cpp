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

#include "GroupHeaderImageWidget.h"

#include <U2Core/U2SafePoints.h>


namespace U2 {


const QString GroupHeaderImageWidget::HEADER_COMMON_STYLE = "border-style: solid;"
    "border-top-width: 1px;"
    "border-right-width: 1px;"
    "border-bottom-width: 1px;"
    "border-color: palette(shadow);"
    "border-bottom-right-radius: 3px;"
    "border-top-right-radius: 3px;"
    "padding: 5px;"
    "margin-top: 4px;"
    "margin-right: 3px;";


GroupHeaderImageWidget::GroupHeaderImageWidget(const QString& _groupId, const QPixmap& image)
    : groupId(_groupId)
{
    this->setPixmap(image);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    int HEADER_MIN_SIZE = 32;
    setMinimumSize(HEADER_MIN_SIZE, HEADER_MIN_SIZE);

    setHeaderDeselected();

    this->setObjectName(groupId);
}


void GroupHeaderImageWidget::mousePressEvent(QMouseEvent* /*event*/)
{
    SAFE_POINT(NULL != groupId, "Internal error: group header with NULL group ID was pressed.",);

    emit si_groupHeaderPressed(groupId);
}


void GroupHeaderImageWidget::setHeaderSelected()
{
    setStyleSheet(QString(
        "background: palette(window);"
        "border-left-width: 0px;"
        ).append(HEADER_COMMON_STYLE));
}


void GroupHeaderImageWidget::setHeaderDeselected()
{
    setStyleSheet(QString(
        "background: qlineargradient(x1:0, y1:0.5, x2:0.4, y2:0.5, stop:0 palette(mid), stop:1 palette(button));"
        "border-left-width: 1px;"
        ).append(HEADER_COMMON_STYLE));
}

} // namespace

