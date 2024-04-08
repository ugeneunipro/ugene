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

#pragma once

#include <harness/UGUITestBase.h>

namespace U2 {
namespace GUITest_preliminary_actions {

#define PRELIMINARY_ACTION_DECLARATION(className) GUI_TEST_CLASS_DECLARATION(className)
#define PRELIMINARY_ACTION_DEFINITION(className) GUI_TEST_CLASS_DEFINITION(className)

#undef GUI_TEST_SUITE
#define GUI_TEST_SUITE "GUITest_preliminary_actions"

// 'Pre action' do some actions that are needed to prepare UGENE for testing
PRELIMINARY_ACTION_DECLARATION(pre_action_0000)
PRELIMINARY_ACTION_DECLARATION(pre_action_0001)
PRELIMINARY_ACTION_DECLARATION(pre_action_0002)
PRELIMINARY_ACTION_DECLARATION(pre_action_0003)
PRELIMINARY_ACTION_DECLARATION(pre_action_0004)
PRELIMINARY_ACTION_DECLARATION(pre_action_0005)

#undef GUI_TEST_SUITE

}  // namespace GUITest_preliminary_actions
}  // namespace U2
