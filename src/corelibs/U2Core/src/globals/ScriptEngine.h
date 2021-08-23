// QScriptEngine is a deprecated QT package and produces warnings during compilation with modern compilers.
// QT team does not want to update/support this package anymore: so we mute these warnings.

#ifdef __GNUC__
#    include <features.h>
#    pragma GCC diagnostic push
#    if __GNUC_PREREQ(8, 1)
#        pragma GCC diagnostic ignored "-Wcast-function-type"
#    endif
#endif
#include <QScriptEngine>
#ifdef __GNUC__
#    pragma GCC diagnostic pop
#endif
