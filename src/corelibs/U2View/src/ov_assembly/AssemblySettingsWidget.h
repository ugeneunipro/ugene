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

#ifndef __ASSEMBLY_SETTINGS_WIDGET_H__
#define __ASSEMBLY_SETTINGS_WIDGET_H__

#include <QWidget>

#include <U2Gui/OPWidgetFactory.h>
#include <U2Gui/U2SavableWidget.h>

class QComboBox;
class QLabel;

namespace U2 {

class AssemblyBrowserUi;

class AssemblySettingsWidget : public QWidget
{
    Q_OBJECT
public:
    AssemblySettingsWidget(AssemblyBrowserUi * ui);

private slots:
    void sl_cellRendererChanged();
    void sl_changeCellRenderer(int newIndex);

    void sl_consensusAlgorithmChanged();
    void sl_changeConsensusAlgorithm(int newIndex);
private:
    QWidget * createReadsSettings();
    QWidget * createRulerSettings();
    QWidget * createConsensusSettings();

    AssemblyBrowserUi * ui;

    // Reads
    QComboBox *readsHighlightCombo;
    // Consensus
    QComboBox *algorithmCombo;
    QLabel *hint;

    U2SavableWidget savableTab;
};


class U2VIEW_EXPORT AssemblySettingsWidgetFactory : public OPWidgetFactory
{
    Q_OBJECT
public:
    AssemblySettingsWidgetFactory();
    virtual ~AssemblySettingsWidgetFactory(){}

    virtual QWidget* createWidget(GObjectView* objView);

    virtual OPGroupParameters getOPGroupParameters();

private:
    static const QString GROUP_ID;
    static const QString GROUP_ICON_STR;
    static const QString GROUP_DOC_PAGE;
};

} // namespace

#endif // #ifndef __ASSEMBLY_SETTINGS_WIDGET_H__
