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

#ifndef _U2_GROUP_HEADER_IMAGE_WIDGET_H_
#define _U2_GROUP_HEADER_IMAGE_WIDGET_H_

#include <QLabel>
#include <U2Core/global.h>

namespace U2 {

/**
 * Widget with image that represents a group header.
 */
class U2GUI_EXPORT GroupHeaderImageWidget : public QLabel
{
    Q_OBJECT
public:
    GroupHeaderImageWidget(const QString& groupId, const QPixmap& image);

    inline const QString& getGroupId() { return groupId; }

    void setHeaderSelected();
    void setHeaderDeselected();

    void changeState() {emit si_groupHeaderPressed(groupId);}

signals:
    /** Emitted when the widget has been pressed */
    void si_groupHeaderPressed(QString groupId);

protected:
    virtual void mousePressEvent(QMouseEvent*);

private:
    QString groupId;

    static const QString HEADER_COMMON_STYLE;
};


} // namespace

#endif

