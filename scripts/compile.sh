#!/bin/bash

# Get the absolute path of the script's directory
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
# Navigate to the project root (assumed to be one level above the script's location)
cd "$SCRIPT_DIR/.." || exit


# Ensure the script is being run in the project root directory
BUILD_DIR="build"

# Clean and recreate the build directory
rm -rf "$BUILD_DIR" && mkdir "$BUILD_DIR"

# Navigate to the build directory
cd "$BUILD_DIR" || { echo "Error: Failed to enter build directory ($BUILD_DIR)"; exit 1; }

# Run CMake and Make
cmake .. && make || { echo "Error: Build failed"; exit 1; }

echo "Build completed successfully!"
