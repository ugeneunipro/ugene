#include <U2Core/disable-warnings.h>
U2_DISABLE_WARNINGS

// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:
//
// $Id: vector3.C,v 1.2 2002/02/27 12:21:29 sturm Exp $

#include <BALL/MATHS/vector3.h>

namespace BALL
{
#ifdef BALL_HAS_EXTERN_TEMPLATES
	template class TVector3<float>;

#ifdef BALL_COMPILER_MSVC
	template class std::vector<Vector3>;
#endif

#endif
}
