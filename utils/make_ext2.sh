#!/bin/bash

echo 'CatK Ext-2 ROOT DISK Creator.'

# Function to display usage information
usage() {
  echo "Usage: $0 [--label volume_label] <source-folder> <output-image>"
  exit 1
}

# Check if the first argument is --label
if [ "$1" == "--label" ]; then
  if [ "$#" -ne 4 ]; then
    usage
  fi
  VOLUME_LABEL=$2
  SOURCE_FOLDER=$3
  OUTPUT_IMAGE=$4
else
  if [ "$#" -ne 2 ]; then
    usage
  fi
  SOURCE_FOLDER=$1
  OUTPUT_IMAGE=$2
  VOLUME_LABEL=""
fi

# Check if the source folder exists
if [ ! -d "$SOURCE_FOLDER" ]; then
  echo "Error: Source folder '$SOURCE_FOLDER' does not exist."
  exit 1
fi

# Determine the size needed for the image
# Use `du` to get the size of the folder in kilobytes and add some extra space
SIZE=$(du -sk "$SOURCE_FOLDER" | cut -f1)
EXTRA_SPACE=1024 # Add extra 1MB
IMAGE_SIZE=$((SIZE + EXTRA_SPACE))

# Create an empty image file
dd if=/dev/zero of="$OUTPUT_IMAGE" bs=1K count="$IMAGE_SIZE"

# Create an EXT-2 filesystem on the image, optionally setting the label
if [ -z "$VOLUME_LABEL" ]; then
  mkfs.ext2 -F "$OUTPUT_IMAGE" -I 128
else
  mkfs.ext2 -F -L "$VOLUME_LABEL" "$OUTPUT_IMAGE" -I 128
fi

# Create a temporary mount point
MOUNT_POINT=$(mktemp -d)

# Mount the image
sudo mount -o loop "$OUTPUT_IMAGE" "$MOUNT_POINT"

# Copy the files to the mounted image
sudo cp -r "$SOURCE_FOLDER"/* "$MOUNT_POINT"

# Unmount the image
sudo umount "$MOUNT_POINT"

# Remove the temporary mount point
rmdir "$MOUNT_POINT"

echo "EXT-2 image '$OUTPUT_IMAGE' created successfully from folder '$SOURCE_FOLDER'."
if [ -n "$VOLUME_LABEL" ]; then
  echo "Volume label set to '$VOLUME_LABEL'."
fi
