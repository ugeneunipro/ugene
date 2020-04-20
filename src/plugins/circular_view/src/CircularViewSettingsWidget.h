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

#ifndef _U2_CIRCULAR_VIEW_SETTINGS_WIDGET_H_
#define _U2_CIRCULAR_VIEW_SETTINGS_WIDGET_H_

#include <U2Gui/U2SavableWidget.h>

#include "ui_CircularViewSettingsWidget.h"

namespace U2 {

struct CircularViewSettings;
class CircularViewSplitter;

class CircularViewSettingsWidget : public QWidget, private Ui_CircularViewSettingsWidget {
    Q_OBJECT
public:
    CircularViewSettingsWidget(CircularViewSettings* settings, CircularViewSplitter* splitter);

signals:
    void si_settingsChanged();
    void si_openCvButtonClicked(CircularViewSettings*);

private slots:
    void sl_modifySettings();
    void sl_cvSplitterWasCreatedOrRemoved(CircularViewSplitter*, CircularViewSettings*);
    void sl_openCvButton();

private:
    void initLayout();
    void connectSlots();

private:
    CircularViewSplitter *circularViewSplitter;
    CircularViewSettings *settings;
    QWidget *             settingsWidget;
    U2SavableWidget       savableWidget;
};

} // namepace

#endif // _U2_CIRCULAR_VIEW_SETTINGS_WIDGET_H_
