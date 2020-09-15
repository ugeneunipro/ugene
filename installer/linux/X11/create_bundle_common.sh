#!/bin/bash

function dump_symbols() {
  filename=$(basename "${1}")
  SYMBOL_FILE="${SYMBOLS_DIR}/${filename}.sym"
  "${ARCH}/dump_syms" "${1}" >"${SYMBOLS_DIR}/${filename}.sym" 2>>"${DUMP_SYMBOLS_LOG}"

  FILE_HEAD=$(head -n 1 "${SYMBOL_FILE}")
  FILE_HASH=$(echo "${FILE_HEAD}" | awk '{ print $4 }')
  FILE_NAME=$(echo "${FILE_HEAD}" | awk '{ print $5 }')

  DEST_PATH="${SYMBOLS_DIR}/${FILE_NAME}/${FILE_HASH}"
  mkdir -p "${DEST_PATH}"
  mv "${SYMBOL_FILE}" "${DEST_PATH}/${FILE_NAME}.sym"
}

function add-qt-library() {
  library=lib${1}.so.5
  cp -v "${QT_DIR}/lib/${library}" "${TARGET_APP_DIR}"
  strip -v "${TARGET_APP_DIR}/${library}"
}

function add-binary() {
  binary=$1
  cp -v "${RELEASE_DIR}/${binary}" "$TARGET_APP_DIR"
  dump_symbols "${TARGET_APP_DIR}/${binary}"
  strip -v "${TARGET_APP_DIR}/${binary}"
}

function add-core-library() {
  library=lib${1}.so
  cp -v "${RELEASE_DIR}/${library}" "${TARGET_APP_DIR}"
  dump_symbols "${TARGET_APP_DIR}/${library}"
  strip -v "${TARGET_APP_DIR}/${library}"
}

function add-plugin() {
  plugin=$1
  echo "Registering plugin: ${plugin}"

  PLUGIN_LIB="lib${plugin}.so"
  PLUGIN_DESC="${plugin}.plugin"
  PLUGIN_LICENSE="${plugin}.license"

  echo "$PLUGIN_LIB"
  echo

  if [ ! -f "${RELEASE_DIR}"/plugins/"${PLUGIN_LIB}" ]; then
    echo "Plugin library file not found: ${PLUGIN_LIB} !"
    exit 1
  fi

  if [ ! -f "${RELEASE_DIR}"/plugins/"${PLUGIN_DESC}" ]; then
    echo "Plugin descriptor file not found: ${PLUGIN_DESC} !"
    exit 1
  fi

  if [ ! -f "${RELEASE_DIR}"/plugins/"${PLUGIN_LICENSE}" ]; then
    echo "Plugin descriptor file not found: ${PLUGIN_LICENSE} !"
    exit 1
  fi

  cp -v "${RELEASE_DIR}/plugins/${PLUGIN_LIB}" "${TARGET_APP_DIR}/plugins/"
  cp -v "${RELEASE_DIR}/plugins/${PLUGIN_DESC}" "${TARGET_APP_DIR}/plugins/"
  cp -v "${RELEASE_DIR}/plugins/${PLUGIN_LICENSE}" "${TARGET_APP_DIR}/plugins/"
  dump_symbols "${TARGET_APP_DIR}/plugins/${PLUGIN_LIB}"
  strip -v "${TARGET_APP_DIR}/plugins/${PLUGIN_LIB}"
}
