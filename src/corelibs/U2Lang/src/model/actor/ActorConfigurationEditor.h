/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#include <U2Lang/ConfigurationEditor.h>

namespace U2 {
namespace Workflow {

class Actor;

/**
 * base class for Actor's configuration editor
 */
class U2LANG_EXPORT ActorConfigurationEditor : public ConfigurationEditor {
    Q_OBJECT
public:
    ActorConfigurationEditor();
    ActorConfigurationEditor(const ActorConfigurationEditor& other);

    virtual void setConfiguration(Actor* actor);
    ConfigurationEditor* clone() override;

protected:
    Actor* cfg;
};  // ActorConfigurationEditor

}  // namespace Workflow
}  // namespace U2
