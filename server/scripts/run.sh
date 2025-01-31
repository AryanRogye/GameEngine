#!/bin/bash

# Get the absolute path of the script's directory
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
# Navigate to the project root (assumed to be one level above the script's location)
cd "$SCRIPT_DIR/.." || exit


# Ensure the script is being run in the project root directory
BUILD_DIR="build"
# Navigate to the build directory
cd "$BUILD_DIR" || { echo "Error: Failed to enter build directory ($BUILD_DIR)"; exit 1; }

./server
