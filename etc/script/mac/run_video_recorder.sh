#!/bin/bash
OUTPUT_PATH="$1"

if [[ -z "$OUTPUT_PATH" ]]; then
  echo "Error: Path to the output file is not set"
  exit 1
fi

ssh localhost "/usr/local/bin/ffmpeg -y -f avfoundation -r 5 -i ':0' \"${OUTPUT_PATH}\""
