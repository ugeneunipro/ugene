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

#ifndef _U2_MSA_UPDATED_WIDGET_INTERFACE_H_
#define _U2_MSA_UPDATED_WIDGET_INTERFACE_H_

#include <U2Core/MultipleSequenceAlignmentObject.h>
#include "MSAEditor.h"

namespace U2 {

class UpdatedWidgetSettings {
public:
    UpdatedWidgetSettings()
        : ma(NULL), ui(NULL), autoUpdate(true) {}
    const MultipleSequenceAlignmentObject* ma;
    MsaEditorWgt*            ui;
    bool                    autoUpdate;
};

enum DataState {
    DataIsOutdated,
    DataIsValid,
    DataIsBeingUpdated
};

class UpdatedWidgetInterface{
public:
    virtual ~UpdatedWidgetInterface() {}
    virtual void onAlignmentChanged(const MultipleSequenceAlignment& maBefore, const MaModificationInfo& modInfo) = 0;
    virtual void setSettings(const UpdatedWidgetSettings* settings) = 0;
    virtual void cancelPendingTasks() = 0;
    virtual QWidget* getWidget() = 0;
    virtual const UpdatedWidgetSettings& getSettings() const = 0;
    virtual void updateWidget() = 0;
    virtual QString getHeaderText() const = 0;
};

} //namespace


#endif
