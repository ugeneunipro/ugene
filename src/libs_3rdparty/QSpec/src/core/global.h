#pragma once

#include <qglobal.h>

#ifdef BUILDING_QSPEC_DLL
#    define HI_EXPORT Q_DECL_EXPORT
#else
#    define HI_EXPORT Q_DECL_IMPORT
#endif
