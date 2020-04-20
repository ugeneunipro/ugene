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

#ifndef _U2_ANNOTATION_TESTS_H_
#define _U2_ANNOTATION_TESTS_H_

#include <core/dbi/DbiTest.h>

#include <unittest.h>

namespace U2 {

DECLARE_TEST(AnnotationUnitTest, get_IdObjectData);
DECLARE_TEST(AnnotationUnitTest, getSet_Name);
DECLARE_TEST(AnnotationUnitTest, getSet_LocationOperator);
DECLARE_TEST(AnnotationUnitTest, getSet_Strand);
DECLARE_TEST(AnnotationUnitTest, getSet_Location);
DECLARE_TEST(AnnotationUnitTest, getSet_Regions);
DECLARE_TEST(AnnotationUnitTest, getSet_Qualifiers);
DECLARE_TEST(AnnotationUnitTest, find_Qualifiers);
DECLARE_TEST(AnnotationUnitTest, get_Group);
DECLARE_TEST(AnnotationUnitTest, getSet_Case);

} //namespace U2

DECLARE_METATYPE(AnnotationUnitTest, get_IdObjectData)
DECLARE_METATYPE(AnnotationUnitTest, getSet_Name)
DECLARE_METATYPE(AnnotationUnitTest, getSet_LocationOperator)
DECLARE_METATYPE(AnnotationUnitTest, getSet_Strand)
DECLARE_METATYPE(AnnotationUnitTest, getSet_Location)
DECLARE_METATYPE(AnnotationUnitTest, getSet_Regions)
DECLARE_METATYPE(AnnotationUnitTest, getSet_Qualifiers)
DECLARE_METATYPE(AnnotationUnitTest, find_Qualifiers)
DECLARE_METATYPE(AnnotationUnitTest, get_Group)
DECLARE_METATYPE(AnnotationUnitTest, getSet_Case)

#endif //_U2_ANNOTATION_TESTS_H_
