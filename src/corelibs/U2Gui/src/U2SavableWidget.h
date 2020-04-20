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

#ifndef _U2_SAVABLE_WIDGET_H_
#define _U2_SAVABLE_WIDGET_H_

#include <QSet>
#include <QVariant>

#include <U2Core/global.h>

class QWidget;

namespace U2 {

class MWMDIWindow;

class U2GUI_EXPORT U2SavableWidget {
public:
    U2SavableWidget(QWidget *wrappedWidget, MWMDIWindow *contextWindow = NULL);
    virtual ~U2SavableWidget();

    virtual QString getWidgetId() const;
    virtual QSet<QString> getChildIds() const;
    virtual bool childValueIsAcceptable(const QString &childId, const QVariant &value) const;
    virtual QVariant getChildValue(const QString &childId) const;
    virtual void setChildValue(const QString &childId, const QVariant &value);

    MWMDIWindow * getContextWindow() const;

protected:
    virtual bool childCanBeSaved(QWidget *child) const;
    virtual QString getChildId(QWidget *child) const;
    virtual bool childExists(const QString &childId) const;
    virtual QWidget * getChildWidgetById(const QString &childId) const;
    virtual QSet<QWidget *> getCompoundChildren() const;

    QWidget *wrappedWidget;
    MWMDIWindow *contextWindow;
    bool widgetStateSaved;
};

} // namespace U2

#endif // _U2_SAVABLE_WIDGET_H_
