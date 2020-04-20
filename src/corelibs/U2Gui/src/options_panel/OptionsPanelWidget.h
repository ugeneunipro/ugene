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

#ifndef _U2_OPTIONS_PANEL_WIDGET_H_
#define _U2_OPTIONS_PANEL_WIDGET_H_

#include <QScrollArea>

#include <U2Core/global.h>

class QVBoxLayout;

namespace U2 {

class GroupHeaderImageWidget;
class GroupOptionsWidget;

/**
 * Is used to add vertical scroll bar to the option groups (when needed)
 */
class OptionsScrollArea : public QScrollArea
{
public:
    OptionsScrollArea(QWidget *parent = NULL);

    /** Ensures that the scroll area would have an appropriate width */
    virtual QSize sizeHint() const;
};


/** Options Panel Widget state */
enum OPMainWidgetState {OPMainWidgetState_Opened, OPMainWidgetState_Closed};

/**
 * Serves as a parent widget for all header image and option widgets.
 * Provides general layout and style of the widgets (but it DOESN'T handle the behavior of selecting a group!).
 */
class U2GUI_EXPORT OptionsPanelWidget : public QFrame
{
    Q_OBJECT
public:
    /** Constructor. Initializes the layout and style of the widget */
    OptionsPanelWidget();

    /** Creates a new header image widget and owns it by putting it into the layout. */
    GroupHeaderImageWidget* createHeaderImageWidget(const QString& groupId, const QPixmap& image);

    /**
     * Creates a new options widget and owns it by putting into the layout.
     * Additional (common) widgets, if any, are put below the widget.
     */
    GroupOptionsWidget* createOptionsWidget(const QString& groupId,
                                            const QString& title,
                                            const QString& documentaionLink,
                                            QWidget* widget, QList<QWidget*> commonWidgets);

    /** Shows/hides the options scroll area widget (the left part of the OP) */
    void openOptionsPanel();
    void closeOptionsPanel();

    /** Specifies if the left part of the Options Panel is shown */
    inline OPMainWidgetState getState() { return opMainWidgetState; }

    /** Returns NULL if not found */
    GroupHeaderImageWidget* findHeaderWidgetByGroupId(const QString& groupId);

    /** Delete options widget (on the left side) */
    void deleteOptionsWidget(const QString& groupId);

    /** Verifies that a widget with the specified ID is present and makes it active */
    void focusOptionsWidget(const QString& groupId);

    /** Returns NULL if not found */
    GroupOptionsWidget* findOptionsWidgetByGroupId(const QString& groupId);

    QWidget * getOptionsWidget() const;

private:
    void initOptionsLayout();
    QWidget * initGroupsLayout();
    void initMainLayout(QWidget *groupsWidget);

private:
    /** Layouts */
    QVBoxLayout *optionsLayout;
    QVBoxLayout *groupsLayout;

    OptionsScrollArea *optionsScrollArea;

    OPMainWidgetState opMainWidgetState;

    QList<GroupHeaderImageWidget*> headerWidgets;
    QList<GroupOptionsWidget*> optionsWidgets;
};


} // namespace

#endif
