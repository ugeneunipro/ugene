#!/bin/bash
OUTPUT_PATH="$1"

if [[ -z "$OUTPUT_PATH" ]]; then
  echo "Error: Path to the output file is not set"
  exit 1
fi

ssh localhost "/usr/local/bin/ffmpeg -y -f avfoundation -framerate 5 -capture_cursor 1 -i '1:none' -an -c:v mpeg4 -q:v 5 \"${OUTPUT_PATH}\""
