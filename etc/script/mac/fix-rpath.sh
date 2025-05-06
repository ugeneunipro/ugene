#!/bin/sh
set -eu

if [ $# -lt 1 ]; then
  echo "Usage: $0 <root_directory>"
  exit 1
fi

ROOT_DIR="$1"

if [ ! -d "$ROOT_DIR" ]; then
  echo "Error: '$ROOT_DIR' is not a directory"
  exit 1
fi

ROOT_DIR=$(cd "$ROOT_DIR" && pwd)

# Compute relative path FROM=$1 TO=$2 (both relative to $ROOT_DIR).
relpath() {
    FROM=$(cd "$1" && pwd)
    TO_DIR=$(cd "$(dirname "$2")" && pwd)
    TO="$TO_DIR/$(basename "$2")"

    FROM_PARTS=$(echo "$FROM" | tr '/' ' ')
    TO_PARTS=$(echo "$TO" | tr '/' ' ')

    # Find common prefix.
    COMMON=""
    while [ -n "$FROM_PARTS" ] && [ -n "$TO_PARTS" ]; do
        F=$(echo "$FROM_PARTS" | cut -d' ' -f1)
        T=$(echo "$TO_PARTS" | cut -d' ' -f1)
        [ "$F" = "$T" ] || break
        COMMON="$COMMON/$F"
        FROM_PARTS=$(echo "$FROM_PARTS" | cut -d' ' -f2-)
        TO_PARTS=$(echo "$TO_PARTS" | cut -d' ' -f2-)
    done

    UP=""
    for _ in $FROM_PARTS; do
        UP="$UP../"
    done

    echo "$UP$(echo "$TO_PARTS" | tr ' ' '/')" | sed 's|//*|/|g' | sed 's|/$||'
}

echo "Scanning for Mach-O binaries in: $ROOT_DIR"
echo

find "$ROOT_DIR" -type f | while read bin; do
  file_output=$(file "$bin")
  case "$file_output" in
    *Mach-O*)
      echo "Inspecting $bin"
      ;;
    *)
      continue
      ;;
  esac

  bin_abs=$(cd "$(dirname "$bin")" && pwd)/$(basename "$bin")
  bin_dir=$(dirname "$bin_abs")

  # Replace @rpath links
  otool -L "$bin_abs" 2>/dev/null | tail -n +2 | while read line; do
    path=$(echo "$line" | awk '{print $1}')
    case "$path" in
      @rpath/*.dylib)
        dylib_name=$(basename "$path")
        dylib_path=$(find "$ROOT_DIR" -name "$dylib_name" -print -quit)
        if [ -z "$dylib_path" ]; then
          echo "  Warning: Could not locate $dylib_name in $ROOT_DIR"
          continue
        fi

        dylib_abs=$(cd "$(dirname "$dylib_path")" && pwd)/$(basename "$dylib_path")
        rel_loader_path=$(relpath "$bin_dir" "$dylib_abs")
        new_path="@loader_path/$rel_loader_path"

        echo "  Rewriting: $path → $new_path"
        install_name_tool -change "$path" "$new_path" "$bin_abs"
        ;;
    esac
  done

  # Fix install_name (id) in dylib itself if it uses @rpath
  id=$(otool -D "$bin_abs" 2>/dev/null | sed -n '2p')
  case "$id" in
    @rpath/*.dylib)
      dylib_name=$(basename "$id")
      new_id="@loader_path/$dylib_name"
      echo "  Rewriting install_name: $id → $new_id"
      install_name_tool -id "$new_id" "$bin_abs"
      ;;
  esac
done

echo
echo "All @rpath references replaced with @loader_path in $ROOT_DIR"
