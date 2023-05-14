#!/bin/bash

# This Script restores original 'data' & 'common_data' dirs before each GUI test run.

if [ -z "${UGENE_TESTS_DIR_NAME}" ]; then
  echo "UGENE_TESTS_DIR_NAME is not set!"
  exit 1
fi

if [ -z "${UGENE_DATA_DIR_NAME}" ]; then
  echo "UGENE_DATA_DIR_NAME is not set!"
  exit 1
fi

# Sync tests dir.
rsync -a --delete test/_common_data/ "${UGENE_TESTS_DIR_NAME}/_common_data/" || return 1

# Sync data dir.
rsync -a --delete "data/" "${UGENE_DATA_DIR_NAME}/" || return 1

# Copy default ini file if set.
if [[ ("${UGENE_USE_MASTER_USER_INI}" -eq "1") && (-n "${UGENE_USER_INI}") && (-f "${UGENE_MASTER_USER_INI}") ]]; then
  mkdir -p "$(dirname "${UGENE_USER_INI}")" && cp "${UGENE_MASTER_USER_INI}" "${UGENE_USER_INI}"
fi
