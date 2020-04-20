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

#ifndef __ASSEMBLY_BROWSER_FACTORY_H__
#define __ASSEMBLY_BROWSER_FACTORY_H__

#include <U2Core/GObjectReference.h>
#include <U2Gui/ObjectViewModel.h>
#include <U2Gui/ObjectViewTasks.h>

namespace U2 {

class U2VIEW_EXPORT AssemblyBrowserFactory : public GObjectViewFactory {
    Q_OBJECT
public:
    AssemblyBrowserFactory(QObject * parent = 0);

    virtual bool canCreateView(const MultiGSelection & multiSelection);

    virtual Task * createViewTask(const MultiGSelection & multiSelection, bool single = false);

    virtual bool isStateInSelection(const MultiGSelection& multiSelection, const QVariantMap& stateData);

    virtual Task* createViewTask(const QString& viewName, const QVariantMap& stateData);

    virtual bool supportsSavedStates() const {return true;}

    static const GObjectViewFactoryId ID;
};

}

#endif
