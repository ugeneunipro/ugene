#!/bin/sh
set -eu

# Compute relative path from $1 to $2 (both absolute)
relpath() {
    FROM=$(cd "$1" && pwd)
    TO=$(cd "$(dirname "$2")" && pwd)/$(basename "$2")

    # Split into arrays
    IFS=/; set -- $FROM; FROM_PARTS=$*
    IFS=/; set -- $TO; TO_PARTS=$*

    # Find common prefix length
    IFS=/
    COMMON=
    while [ -n "$FROM_PARTS" ] && [ -n "$TO_PARTS" ] && [ "$(echo "$FROM_PARTS" | cut -d' ' -f1)" = "$(echo "$TO_PARTS" | cut -d' ' -f1)" ]; do
        COMMON="$COMMON/$(echo "$FROM_PARTS" | cut -d' ' -f1)"
        FROM_PARTS=$(echo "$FROM_PARTS" | cut -d' ' -f2-)
        TO_PARTS=$(echo "$TO_PARTS" | cut -d' ' -f2-)
    done

    # Count how many dirs to go up
    UP=""
    for part in $FROM_PARTS; do
        UP="../$UP"
    done

    REL="$UP$TO_PARTS"
    echo "$REL" | sed 's|//*|/|g' | sed 's|/$||'
}

if [ $# -lt 1 ]; then
  echo "Usage: $0 <root_directory>"
  exit 1
fi

ROOT_DIR="$1"

if [ ! -d "$ROOT_DIR" ]; then
  echo "Error: '$ROOT_DIR' is not a directory"
  exit 1
fi

echo "Scanning for binaries in: $ROOT_DIR"
echo

find "$ROOT_DIR" -type f | while read bin; do
  file_output=$(file "$bin")
  case "$file_output" in
    *Mach-O*)
      echo "fix-rpath: checking $bin"
      ;;
    *)
      continue
      ;;
  esac

  bin_dir=$(cd "$(dirname "$bin")" && pwd)
  bin_path="$bin_dir/$(basename "$bin")"

  # Replace @rpath links
  otool -L "$bin_path" 2>/dev/null | tail -n +2 | while read line; do
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
        install_name_tool -change "$path" "$new_path" "$bin_path"
        ;;
    esac
  done

  # Fix install_name (id) in dylib itself if it uses @rpath
  id=$(otool -D "$bin_path" 2>/dev/null | sed -n '2p')
  case "$id" in
    @rpath/*.dylib)
      dylib_name=$(basename "$id")
      new_id="@loader_path/$dylib_name"
      echo "  Rewriting install_name: $id → $new_id"
      install_name_tool -id "$new_id" "$bin_path"
      ;;
  esac
done

echo
echo "All @rpath references replaced with @loader_path in $ROOT_DIR"
