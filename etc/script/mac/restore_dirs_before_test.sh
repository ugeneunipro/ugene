#!/bin/bash

# This Script restores original 'data' & 'common_data' dirs before each GUI test run.

# echo "Running restore_dirs_before_test.sh, dir: $(pwd) UGENE_USE_MASTER_USER_INI: ${UGENE_USE_MASTER_USER_INI} UGENE_USER_INI: ${UGENE_USER_INI}" >> ~/cleanup.log

# Sync tests dir.
rsync -a --delete "test_data/_common_data/" "test/_common_data/" || return 1

# Sync data dir.
rsync -a --delete "ugene_git/data/" "ugene_app/Unipro UGENE.app/Contents/MacOS/data/" || return 1

# Copy default ini file if set.
if [[ ("${UGENE_USE_MASTER_USER_INI}" -eq "1") && (-n "${UGENE_USER_INI}") && (-f "${UGENE_MASTER_USER_INI}") ]]; then
  mkdir -p "$(dirname "${UGENE_USER_INI}")" && cp "${UGENE_MASTER_USER_INI}" "${UGENE_USER_INI}"
fi
