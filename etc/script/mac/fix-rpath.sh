#!/bin/sh
set -eu

# Ensure a directory is passed
if [ $# -lt 1 ]; then
  echo "Usage: $0 <directory>"
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
      echo "🔍 Inspecting $bin"
      ;;
    *)
      continue
      ;;
  esac

  # Fix @rpath → @loader_path in linked dylibs
  otool -L "$bin" 2>/dev/null | tail -n +2 | while read line; do
    path=$(echo "$line" | awk '{print $1}')
    case "$path" in
      @rpath/*.dylib)
        dylib_name=$(basename "$path")
        new_path="@loader_path/$dylib_name"
        echo "  ➜ Rewriting: $path → $new_path"
        install_name_tool -change "$path" "$new_path" "$bin"
        ;;
    esac
  done

  # Fix @rpath in install_name (if it's a dylib)
  id=$(otool -D "$bin" 2>/dev/null | sed -n '2p')
  case "$id" in
    @rpath/*.dylib)
      dylib_name=$(basename "$id")
      new_id="@loader_path/$dylib_name"
      echo "  ➜ Rewriting install_name: $id → $new_id"
      install_name_tool -id "$new_id" "$bin"
      ;;
  esac
done

echo
echo "✅ All @rpath references replaced with @loader_path in $ROOT_DIR"
