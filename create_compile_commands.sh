#!/bin/bash

# Get the absolute path of the script's directory
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
# Navigate to the project root (assumed to be one level above the script's location)
cd "$SCRIPT_DIR/" || exit


# Ensure the script is being run in the project root directory
BUILD_DIR="build"

# Create the build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
  mkdir "$BUILD_DIR"
fi

# Navigate to the build directory
cd "$BUILD_DIR" || exit

# Run CMake with the option to generate compile_commands.json
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

# Copy the compile_commands.json to the project root if needed
if [ -f "compile_commands.json" ]; then
  cp compile_commands.json ..
  echo "compile_commands.json generated and copied to the project root."
else
  echo "Failed to generate compile_commands.json. Check your CMake setup."
fi
