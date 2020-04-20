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

#ifndef _U2_WIDGET_WITH_LOCAL_TOOLBAR_H_
#define _U2_WIDGET_WITH_LOCAL_TOOLBAR_H_

#include <U2Core/global.h>

#include <QToolButton>
#include <QWidget>


namespace U2 {

class LocalToolbar;

class U2GUI_EXPORT WidgetWithLocalToolbar : public QWidget {
    Q_OBJECT
public:
    WidgetWithLocalToolbar(QWidget* p = 0);

    void setContentLayout(QLayout* l);

    QToolButton* addActionToLocalToolbar(QAction* action);

    void setLocalToolBarObjectName(const QString& name);

protected:
    void setLocalToolbarVisible(bool visible);
    QWidget*                contentWidget;

private:
    LocalToolbar*   toolBar;
};

} // namespace

#endif // _U2_WIDGET_WITH_LOCAL_TOOLBAR_H_
