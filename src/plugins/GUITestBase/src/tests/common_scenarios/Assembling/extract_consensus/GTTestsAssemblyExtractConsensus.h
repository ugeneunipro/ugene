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

#ifndef _U2_GUITEST_ASSEMBLY_EXTRACT_CONSENSUS_H_
#define _U2_GUITEST_ASSEMBLY_EXTRACT_CONSENSUS_H_

#include <harness/UGUITestBase.h>

namespace U2 {

namespace GUITest_assembly_extract_consensus {
#undef GUI_TEST_SUITE
#define GUI_TEST_SUITE "GUITest_assembly_extract_consensus"

GUI_TEST_CLASS_DECLARATION(test_0001_single_input)
GUI_TEST_CLASS_DECLARATION(test_0002_multiple_input)
GUI_TEST_CLASS_DECLARATION(test_0003_wrong_input)

#undef GUI_TEST_SUITE
}  // namespace GUITest_assembly_extract_consensus

}  // namespace U2

#endif  // _U2_GUITEST_ASSEMBLY_EXTRACT_CONSENSUS_H_
