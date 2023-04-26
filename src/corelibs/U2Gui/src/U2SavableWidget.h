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

#include <QSet>
#include <QVariant>

#include <U2Core/global.h>

class QWidget;

namespace U2 {

class MWMDIWindow;

class U2GUI_EXPORT U2SavableWidget {
public:
    U2SavableWidget(QWidget* wrappedWidget, MWMDIWindow* contextWindow = nullptr, const QStringList& excludeWidgetIds = {});
    virtual ~U2SavableWidget();

    virtual QString getWidgetId() const;
    virtual QSet<QString> getChildIds() const;
    virtual bool childValueIsAcceptable(const QString& childId, const QVariant& value) const;
    virtual QVariant getChildValue(const QString& childId) const;
    virtual void setChildValue(const QString& childId, const QVariant& value);

    MWMDIWindow* getContextWindow() const;

protected:
    virtual bool childCanBeSaved(QWidget* child) const;
    virtual QString getChildId(QWidget* child) const;
    virtual bool childExists(const QString& childId) const;
    virtual QWidget* getChildWidgetById(const QString& childId) const;
    virtual QSet<QWidget*> getCompoundChildren() const;
    bool isExcluded(const QString& childId) const;

    QWidget* wrappedWidget;
    MWMDIWindow* contextWindow;
    bool widgetStateSaved;
    /*
     * Widgets, which state shouldn't be saved.
     * This behaviour works for the widget and all it's children
     */
    QStringList excludeWidgetIds;
};

}  // namespace U2
