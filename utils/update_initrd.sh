#/usr/bin/env bash

set -e

echo "CatK Initrd Updater."

usage() {
  echo "Usage: $0 <init> <initrd>"
  exit 1
}

if [[ $# -ne 2 ]]; then
  usage
fi

INITRD=$2
INIT=$1
TEMP_DIR=$(mktemp -d)

tar -xf "$INITRD" -C "$TEMP_DIR"
cp "$INIT" "$TEMP_DIR"
cd "$TEMP_DIR"
tar -cvf "$INITRD" . > /dev/null
echo "Cleaning up..."
rm -fdr "$TEMP_DIR"

echo "Finished. No errors reported."
