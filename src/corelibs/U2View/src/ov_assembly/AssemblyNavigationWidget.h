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

#ifndef __ASSEMBLY_NAVIGATION_WIDGET_H__
#define __ASSEMBLY_NAVIGATION_WIDGET_H__

#include <QLabel>

#include <U2Gui/OPWidgetFactory.h>
#include <U2Gui/U2SavableWidget.h>

namespace U2 {

class AssemblyBrowser;
class PositionSelector;

class AssemblyNavigationWidget : public QWidget {
    Q_OBJECT
public:
    AssemblyNavigationWidget(AssemblyBrowser *browser, QWidget *p = NULL);

public slots:
    void sl_updateZoomingState();

private:
    AssemblyBrowser *browser;
    PositionSelector *posSelector;
    U2SavableWidget savableTab;
};

class U2VIEW_EXPORT CoveredRegionsLabel : public QLabel {
    Q_OBJECT
public:
    CoveredRegionsLabel(AssemblyBrowser *browser, QWidget *p = NULL);

    void setAdditionalText(QString prefix, QString postfix);

public slots:
    void sl_updateContent();

private:
    AssemblyBrowser *browser;
    QString prefix;
    QString postfix;
};

class U2VIEW_EXPORT AssemblyNavigationWidgetFactory : public OPWidgetFactory {
    Q_OBJECT
public:
    AssemblyNavigationWidgetFactory();

    QWidget * createWidget(GObjectView *objView);
    OPGroupParameters getOPGroupParameters();

private:
    static const QString GROUP_ID;
    static const QString GROUP_ICON_STR;
    static const QString GROUP_DOC_PAGE;
};

} // namespace U2

#endif // __ASSEMBLY_NAVIGATION_WIDGET_H__
