// QScriptEngine is a deprecated QT package and produces warnings during compilation with modern compilers.
// QT team does not want to update/support this package anymore: so we mute these warnings.

#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpragmas"  // Ignore all unknown (for example when an old GCC is used) pragmas below.
#    pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
#include <QScriptEngine>
#ifdef __GNUC__
#    pragma GCC diagnostic pop
#endif
