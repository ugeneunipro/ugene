// -*- Mode: C++; tab-width: 2; -*-
// vi: set ts=2:

#ifndef BALL_CONFIG_CONFIG_H
#define BALL_CONFIG_CONFIG_H

#include <QtGlobal>

#ifdef _WINDOWS
typedef signed char             int8_t;
typedef unsigned char           uint8_t;
typedef signed short            int16_t;
typedef unsigned short          uint16_t;
typedef signed int              int32_t;
typedef unsigned int            uint32_t;
typedef long long int           int64_t;
typedef unsigned long long int  uint64_t;
#else
#include <stdint.h>
#endif // _WINDOWS

// Here are some global configuration flags for BALL

// BALL_DEBUG enables some debugging methods
// change the DEBUG entry in configure to define this flag.
// If in debug mode, inline functions won't be compiled
// as inline by defining BALL_NO_INLINE_FUNCTIONS
// This facilitates debugging, as the debugger can always
// find the corresponding source code line.
// See also COMMON/debug.h for these symbols.
/* #undef BALL_DEBUG */
/* #undef BALL_NO_INLINE_FUNCTIONS */

// The path to the folder where BALL is installed.
//#define BALL_PATH "/usr/local"
//#define BALL_DATA_PATH "/usr/local/share/BALL/data"

// The string describes the binary format.
//#define BALL_BINFMT "Linux-x86_64"

// The processor architecture
//#define BALL_ARCH "x86_64"

// The operating system
//#define BALL_OS "Linux"

// Some convenient shortcuts for operating systems we often encounter
// NOTE: we'd like to deprecate these, so try using BALL_OS instead
// whenever possible
#ifdef Q_OS_UNIX
#define BALL_OS_LINUX
#endif
/* #undef BALL_OS_SOLARIS */
/* #undef BALL_OS_SUNOS */
/* #undef BALL_OS_IRIX */
/* #undef BALL_OS_FREEBSD */
/* #undef BALL_OS_NETBSD */
/* #undef BALL_OS_OPENBSD */
#ifdef Q_OS_DARWIN
#define BALL_OS_DARWIN
#endif
#ifdef Q_OS_WIN
#define BALL_OS_WINDOWS
#define BALL_PLATFORM_WINDOWS
#endif

// Deprecated, only for compatibility
/* #undef BALL_PLATFORM_WINDOWS */

// Define compiler specifics

// Microsoft Visual Studio .NET
/* #undef BALL_COMPILER_MSVC */
// GNU g++
//#define BALL_COMPILER_GXX
// Intel C++
/* #undef BALL_COMPILER_INTEL */

// The compiler name.
//#define BALL_COMPILER "GXX"

// Defines for the compiler version (major.minor.minor_minor)
//#define BALL_COMPILER_VERSION 4.8.0
//#define BALL_COMPILER_VERSION_MAJOR 4
//#define BALL_COMPILER_VERSION_MINOR 8
/* #undef BALL_COMPILER_VERSION_MINOR_MINOR */

// define symbols for the endianness of the system
//#define BALL_LITTLE_ENDIAN
/* #undef BALL_BIG_ENDIAN */

// Type sizes

// This flag is defined on 64bit architectures
//#define BALL_64BIT_ARCHITECTURE

// define some symbols for the (bit)size of some builtin types
#define BALL_CHAR_SIZE 1
#define BALL_SHORT_SIZE 2
#define BALL_INT_SIZE 4
#define BALL_LONG_SIZE 8
#define BALL_USHORT_SIZE 2
#define BALL_UINT_SIZE 4
#define BALL_ULONG_SIZE 8
#define BALL_SIZE_T_SIZE 8
#define BALL_POINTER_SIZE 8
#define BALL_FLOAT_SIZE 4
#define BALL_DOUBLE_SIZE 8

// define platform independant types for unsigned 16|32|64 bit numbers
#define BALL_UINT16 uint16_t
#define BALL_UINT32 uint32_t
#define BALL_UINT64 uint64_t
#define BALL_INT16 int16_t
#define BALL_INT32 int32_t
#define BALL_INT64 int64_t

// Define a signed/unsigned numeric type of 64 bit length (used for
// platform independent persistence: stores pointers)
// this usually defaults to unsigned long on 64 bit architectures
// and unsigned long long on 32 bit machines
#define BALL_ULONG64_TYPE uint64_t
#define BALL_LONG64_TYPE int64_t

// Defines an unsigned integer type of the same size as void*
#define BALL_POINTERSIZEUINT_TYPE unsigned long

// Defines an unsigned type that has the same length as size_t
#define BALL_SIZE_TYPE unsigned long//uint32_t

// Defines a signed type that has the same length as size_t
#define BALL_INDEX_TYPE int32_t

// Define the precision for the BALL Complex type.
#define BALL_COMPLEX_PRECISION float

// System header information
//#define BALL_HAS_UNISTD_H
/* #undef BALL_HAS_PROCESS_H */
#define BALL_HAS_TIME_H
#define BALL_HAS_LIMITS_H
//#define BALL_HAS_DIRENT_H
/* #undef BALL_HAS_DIRECT_H */
//#define BALL_HAS_PWD_H
//#define BALL_HAS_STDINT_H

#define BALL_HAS_SYS_IOCTL_H
//#define BALL_HAS_SYS_TIME_H
#define BALL_HAS_SYS_STAT_H
//#define BALL_HAS_SYS_TIMES_H
#define BALL_HAS_SYS_TYPES_H
//#define BALL_HAS_SYS_PARAM_H
//#define BALL_HAS_SYS_SOCKET_H
#define BALL_HAS_SYS_SYSINFO_H

// Define whether float.h has to be included along with limits.h
#define BALL_HAS_FLOAT_H

// Define if numeric_limits is defined in <limits>
//#define BALL_HAS_NUMERIC_LIMITS

#define BALL_HAS_REGEX_H

//#define BALL_HAS_NETINET_IN_H
//#define BALL_HAS_NETDB_H

// Some systems don't provide mode_t for us...
/* #undef mode_t */

// Define whether we can use <sstream> or whether we have to use the
// older <strstream>
#define BALL_HAS_SSTREAM

// on some systems (e.g. Solaris) we need to include /usr/include/ieeefp.h
// for some floating point functions
/* #undef BALL_HAS_IEEEFP_H */

// on some systems (e.g. Solaris) we need to include /usr/include/values.h
// for some floating point functions
#define BALL_HAS_VALUES_H

//#define BALL_HAS_KILL
#define BALL_HAS_SYSCONF

// Specific to the Windows port
#ifdef BALL_OS_WINDOWS
//#define BALL_HAS_WINDOWS_PERFORMANCE_COUNTER
//#define BALL_USE_WINSOCK

// MS VC++ doesn't define all required symbols in
// math.h if this symbol isn't defined *before* including
// math.h for the first time!
#define _USE_MATH_DEFINES

// Keep Windows from defining min/max, ERROR,... as preprocesor symbols!
// May he how thought of defining these by default rot in hell!
#ifndef NOMINMAX
#	define NOMINMAX 1
#endif

#define NOGDI
#define NOWINRES

#define WINDOWS_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#endif // BALL_OS_WINDOWS

// Define whether the compiler provides an ANSI-compliant implementation
// of <iostream>. This is required in COMMON/socket.h/C since we need
// the correct initialisation of the base classes. These base classes are
// either something like std::basic_ios<char> etc., or ios, istream etc.
#define BALL_HAS_ANSI_IOSTREAM

// Define whether inline functions may be used as template parameters.
#define BALL_HAS_INLINE_TPL_ARGS

// tr1 related variables

// Do we have an unordered_map, and if so, where?
//#define BALL_HAS_UNORDERED_MAP
//#define BALL_HAS_HASH_MAP

//#ifdef BALL_OS_WINDOWS
//#define BALL_HAS_STD_UNORDERED_MAP

//#else
/* #undef BALL_HAS_STD_UNORDERED_MAP */
//#define BALL_HAS_TR1_UNORDERED_MAP
//#endif
/* #undef BALL_HAS_BOOST_UNORDERED_MAP */

#define BALL_MAP_NAMESPACE std
#define BALL_MAP_NAME std::map<Key,T>

// Defines the prefix needed when including extensions to the stl
#define BALL_EXT_INCLUDE_PREFIX
/* #undef BALL_EXT_INCLUDE_PREFIX */

// Defines if extensions to hash must be defined in namespace std
#define BALL_EXTEND_HASH_IN_STD_NS

// Defines whether the HashMap already provides LongSize hashing
/* #undef BALL_NEEDS_LONGSIZE_HASH */

// Defines whether the GNU slist implementation could be found
// This is useful for e.g. an efficient HashGrid implementation
//#define BALL_HAS_GNU_SLIST

// Defines whether the extern keyword for templates is supported by the compiler.
// We can use this to speed up compilation and to reduce some warnings in under
// MSVC.
//#define BALL_HAS_EXTERN_TEMPLATES

// Defines whether the compiler supports c++0x-style rvalue references
// (int&&, ...)
///! #define BALL_HAS_RVALUE_REFERENCES

// Defines whether the c++ std lib implementation supports rvalue references in the std::string class
///! #define BALL_STD_STRING_HAS_RVALUE_REFERENCES

// Defines whether we can overload functions with LongIndex and LongSize safely
/* #undef BALL_ALLOW_LONG64_TYPE_OVERLOADS */

// This define is used in string.C and enables a workaround
// on those poor systems that do not define vsnprintf.
#define BALL_HAVE_VSNPRINTF

// This flag is used by GenericPDBFile
// if it is not set, the length of each line has
// to meet the PDB spcifications exactly.
// As virtually no existing PDB file fulfills
// this requirement, we disencourage its usage.
// Nevertheless, it can be quite useful for debugging.
/* #undef BALL_STRICT_PDB_LINE_IMPORT */

// the signature of the function arguments used in xdrrec_create differs
// from platform to platform, so we define some symbols describing the
// correct arguments: (void*, char*, int), (char*, char*, int), or (void)
// take arguments of type (void*, char*, int)
#define BALL_XDRREC_CREATE_CHAR_CHAR_INT 1
/* #undef BALL_XDRREC_CREATE_VOID_VOID_INT */
/* #undef BALL_XDRREC_CREATE_VOID_CHAR_INT */
/* #undef BALL_XDRREC_CREATE_VOID_VOID_UINT */
/* #undef BALL_XDRREC_CREATE_VOID */

// some platforms do not provide xdr_u_hyper, so we need a workaround for this
#define BALL_HAS_XDR_U_HYPER

// Define the argument type for xdr_u_hyper (64 bit)
#define BALL_XDR_UINT64_TYPE u_quad_t

// boost

// defined if we have boost::system support
//#define BALL_HAS_BOOST_SYSTEM

// defined if we have boost::iostreams support
//#define BALL_HAS_BOOST_IOSTREAMS

// defined if we have boost::thread support
//#define BALL_HAS_BOOST_THREAD

// defined if we have boost::regex support
//#define BALL_HAS_BOOST_REGEX

// defined if asio is taken from boost
//#define BALL_HAS_BOOST_ASIO

// defined if BALL was configured with asio support
//#define BALL_HAS_ASIO

// the namespace to use for asio
//#define BALL_ASIO_NAMESPACE boost::asio

// Define whether the BLAS library is available
#define BALL_HAS_BLAS

// Define whether LAPACK is available
#define BALL_HAS_LAPACK

// Define whether the FFTW library/header is available.
//#define BALL_HAS_FFTW
//#define BALL_HAS_FFTW_H

// Define which versions of fftw can be used: double, float, long double
//#define BALL_HAS_FFTW_DOUBLE
//#define BALL_HAS_FFTW_FLOAT
/* #undef BALL_HAS_FFTW_LONG_DOUBLE */

// The default traits for the FFTW classes
//#define BALL_FFTW_DEFAULT_TRAITS FloatTraits

// Define whether BALL was built with lpsolve support
#define BALL_HAS_LPSOLVE

// Define whether BALL was built with libsvm support
/* #undef BALL_HAS_LIBSVM */

// VIEW related options
#define BALL_HAS_VIEW

// QT options
#define BALL_QT_HAS_THREADS // Qt now always has thread support
#define QT_THREAD_SUPPORT

// Glew
//#define BALL_HAS_GLEW

// MPI
/* #undef BALL_HAS_MPI */

// Define for activated PYTHON support
//#define BALL_PYTHON_SUPPORT

// Maximum line length for reading from files (see source/FORMAT)
#define BALL_MAX_LINE_LENGTH 65535

#endif // BALL_CONFIG_CONFIG_H
