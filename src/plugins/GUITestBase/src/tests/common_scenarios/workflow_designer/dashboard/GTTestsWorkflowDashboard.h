/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2018 UniPro <ugene@unipro.ru>
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

#ifndef _U2_GT_TESTS_WORKFLOW_DASHBOARD_H_
#define _U2_GT_TESTS_WORKFLOW_DASHBOARD_H_

#include <U2Test/UGUITestBase.h>

namespace U2 {

namespace GUITest_common_scenarios_workflow_dashboard {
#undef GUI_TEST_SUITE
#define GUI_TEST_SUITE "GUITest_common_scenarios_workflow_dashboard"

GUI_TEST_CLASS_DECLARATION(misc_test_0001)
GUI_TEST_CLASS_DECLARATION(misc_test_0002)
GUI_TEST_CLASS_DECLARATION(misc_test_0003)
GUI_TEST_CLASS_DECLARATION(misc_test_0004)

GUI_TEST_CLASS_DECLARATION(tree_nodes_creation_test_0001)
GUI_TEST_CLASS_DECLARATION(tree_nodes_creation_test_0002)
GUI_TEST_CLASS_DECLARATION(tree_nodes_creation_test_0003)
GUI_TEST_CLASS_DECLARATION(tree_nodes_creation_test_0004)

GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0001)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0002)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0003)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0004)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0005)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0006)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0007)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0008)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0009)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0010)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0011)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0012)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0013)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0014)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0015)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0016)
GUI_TEST_CLASS_DECLARATION(tool_launch_nodes_test_0017)

GUI_TEST_CLASS_DECLARATION(view_opening_test_0001)
GUI_TEST_CLASS_DECLARATION(view_opening_test_0002)
GUI_TEST_CLASS_DECLARATION(view_opening_test_0003)
GUI_TEST_CLASS_DECLARATION(view_opening_test_0004)

GUI_TEST_CLASS_DECLARATION(output_dir_scanning_test_0001)
GUI_TEST_CLASS_DECLARATION(output_dir_scanning_test_0002)
GUI_TEST_CLASS_DECLARATION(output_dir_scanning_test_0003)
GUI_TEST_CLASS_DECLARATION(output_dir_scanning_test_0004)
GUI_TEST_CLASS_DECLARATION_SET_TIMEOUT(output_dir_scanning_test_0005_1, 720000)
GUI_TEST_CLASS_DECLARATION_SET_TIMEOUT(output_dir_scanning_test_0005, 720000)
GUI_TEST_CLASS_DECLARATION_SET_TIMEOUT(output_dir_scanning_test_0006, 720000)
GUI_TEST_CLASS_DECLARATION_SET_TIMEOUT(output_dir_scanning_test_0007, 720000)
GUI_TEST_CLASS_DECLARATION_SET_TIMEOUT(output_dir_scanning_test_0008, 720000)

#undef GUI_TEST_SUITE
}    // namespace GUITest_common_scenarios_workflow_dashboard

}    // namespace U2

#endif    // _U2_GT_TESTS_WORKFLOW_DASHBOARD_H_
