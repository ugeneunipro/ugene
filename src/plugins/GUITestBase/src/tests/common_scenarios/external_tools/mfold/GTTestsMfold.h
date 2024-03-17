/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2024 UniPro <ugene@unipro.ru>
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
namespace GUITest_common_scenarios_mfold {
#undef GUI_TEST_SUITE
#define GUI_TEST_SUITE "GUITest_common_scenarios_mfold"
GUI_TEST_CLASS_DECLARATION(test_0001_success)
GUI_TEST_CLASS_DECLARATION(test_0002_fail)
GUI_TEST_CLASS_DECLARATION(test_0003_limits)
GUI_TEST_CLASS_DECLARATION(test_0004_region)
GUI_TEST_CLASS_DECLARATION(test_0005_large)
GUI_TEST_CLASS_DECLARATION(test_0006_html_name)
#undef GUI_TEST_SUITE
}  // namespace GUITest_common_scenarios_mfold
}  // namespace U2
