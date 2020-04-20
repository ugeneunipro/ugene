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

#ifndef _U2_CREATE_PHY_TREE_WIDGET_H_
#define _U2_CREATE_PHY_TREE_WIDGET_H_

#include <QWidget>

#include <U2Algorithm/CreatePhyTreeSettings.h>

#include <U2Core/MultipleSequenceAlignment.h>

namespace U2 {

class CreatePhyTreeSettings;
class PhyTreeDisplayOptionsWidget;

class U2VIEW_EXPORT CreatePhyTreeWidget : public QWidget {
public:
    CreatePhyTreeWidget(QWidget* parent);

    virtual void fillSettings(CreatePhyTreeSettings& settings) = 0;
    virtual void storeSettings() = 0;
    virtual void restoreDefault() = 0;
    virtual bool checkSettings(QString &message, const CreatePhyTreeSettings &settings);
    virtual bool checkMemoryEstimation(QString &message, const MultipleSequenceAlignment &ma, const CreatePhyTreeSettings &settings);
    virtual bool insertOutputOptionWidget(PhyTreeDisplayOptionsWidget *outputOptionsWidget);

    static QString settingsPath();
};

}   // namespace U2

#endif // _U2_CREATE_PHY_TREE_WIDGET_H_
