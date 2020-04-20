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

#ifndef _U2_ANNOTATION_GROUP_TESTS_H_
#define _U2_ANNOTATION_GROUP_TESTS_H_

#include <unittest.h>

namespace U2 {

DECLARE_TEST(AnnotationGroupUnitTest, get_IdObject);
DECLARE_TEST(AnnotationGroupUnitTest, getSet_Name);
DECLARE_TEST(AnnotationGroupUnitTest, groupHierarchy);
DECLARE_TEST(AnnotationGroupUnitTest, getSet_Annotations);
DECLARE_TEST(AnnotationGroupUnitTest, findAnnotationsInSubtree);
DECLARE_TEST(AnnotationGroupUnitTest, getSet_Subgroups);

} //namespace U2

DECLARE_METATYPE(AnnotationGroupUnitTest, get_IdObject)
DECLARE_METATYPE(AnnotationGroupUnitTest, getSet_Name)
DECLARE_METATYPE(AnnotationGroupUnitTest, groupHierarchy)
DECLARE_METATYPE(AnnotationGroupUnitTest, getSet_Annotations)
DECLARE_METATYPE(AnnotationGroupUnitTest, findAnnotationsInSubtree)
DECLARE_METATYPE(AnnotationGroupUnitTest, getSet_Subgroups)

#endif //_U2_ANNOTATION_GROUP_TESTS_H_
