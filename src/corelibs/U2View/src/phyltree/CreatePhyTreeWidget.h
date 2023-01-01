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

    /** Saves the current UI widget state into the settings data model. */
    virtual void fillSettings(CreatePhyTreeSettings& settings) = 0;

    /** Stores current UI widget state into U2::Settings. */
    virtual void storeSettings() = 0;

    /** Resets current widget & U2::Settings state to the algorithm defaults. */
    virtual void restoreDefault() = 0;

    virtual bool checkSettings(QString& message, const CreatePhyTreeSettings& settings);
    virtual bool checkMemoryEstimation(QString& message, const MultipleSequenceAlignment& ma, const CreatePhyTreeSettings& settings);

    static QString getAppSettingsRoot();
};

}  // namespace U2

#endif  // _U2_CREATE_PHY_TREE_WIDGET_H_
