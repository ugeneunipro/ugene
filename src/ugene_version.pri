# This file contains UGENE version info

# minimum UGENE version whose SQLite databases are compatible with this version
UGENE_MIN_VERSION_SQLITE=1.25.0

# minimum UGENE version whose MySQL databases are compatible with this version
UGENE_MIN_VERSION_MYSQL=1.25.0

# distribution info
isEmpty( U2_DISTRIBUTION_INFO ) {
U2_DISTRIBUTION_INFO=sources
}

# int version levels for executables
UGENE_VER_MAJOR=40
UGENE_VER_MINOR=0
UGENE_VER_SUFFIX=-dev

# product version
UGENE_VERSION=$${UGENE_VER_MAJOR}.$${UGENE_VER_MINOR}$${UGENE_VER_SUFFIX}
