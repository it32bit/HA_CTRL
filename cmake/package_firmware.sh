#!/bin/bash
APP_VER=$(cat "$1")
BOOT_VER=$(cat "$2")
BIN_DIR="$3"
OUT_DIR="$4"
PROJECT_NAME="$5"

ZIP_NAME="${PROJECT_NAME}_App_v${APP_VER}_Boot_v${BOOT_VER}.zip"

mkdir -p "$OUT_DIR"
cd "$BIN_DIR" || exit 1
zip -r "$ZIP_NAME" .
mv "$ZIP_NAME" "$OUT_DIR/"
