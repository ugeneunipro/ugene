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

#ifndef _U2_POSTERIOR_CHECKS_H_
#define _U2_POSTERIOR_CHECKS_H_

#include <U2Test/UGUITest.h>

namespace U2 {
namespace GUITest_posterior_checks {

#define POSTERIOR_CHECK_DECLARATION(className) GUI_TEST_CLASS_DECLARATION(className)
#define POSTERIOR_CHECK_DEFINITION(className) GUI_TEST_CLASS_DEFINITION(className)

#undef GUI_TEST_SUITE
#define GUI_TEST_SUITE "GUITest_posterior_checks"

POSTERIOR_CHECK_DECLARATION(post_check_0000)
POSTERIOR_CHECK_DECLARATION(post_check_0001)

#undef GUI_TEST_SUITE

}   // namespace GUITest_posterior_checks
}   // namespace U2

#endif // _U2_POSTERIOR_CHECKS_H_
