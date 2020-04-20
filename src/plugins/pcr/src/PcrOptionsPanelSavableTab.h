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

#ifndef _U2_PCR_OPTIONS_PANEL_SAVABLE_TAB_H_
#define _U2_PCR_OPTIONS_PANEL_SAVABLE_TAB_H_

#include <U2Gui/U2SavableWidget.h>

namespace U2 {

class InSilicoPcrOptionPanelWidget;

class PcrOptionsPanelSavableTab : public U2SavableWidget {
public:
    PcrOptionsPanelSavableTab(QWidget *wrappedWidget, MWMDIWindow *contextWindow);
    ~PcrOptionsPanelSavableTab();

    QVariant getChildValue(const QString &childId) const;
    void setChildValue(const QString &childId, const QVariant &value);
    bool childValueIsAcceptable(const QString &childId, const QVariant &value) const;

protected:
    QString getChildId(QWidget *child) const;
    QWidget * getChildWidgetById(const QString &childId) const;
    bool childExists(const QString &childId) const;

private:
    QWidget * getPrimerEditWidgetById(const QString &childId) const;

    InSilicoPcrOptionPanelWidget *originalWrappedWidget;
};

} // namespace U2

#endif // _U2_PCR_OPTIONS_PANEL_SAVABLE_TAB_H_
