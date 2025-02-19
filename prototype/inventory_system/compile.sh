#!/bin/bash

# Get the absolute path of the script's directory
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)"
# Navigate to the project root (assumed to be one level above the script's location)
cd "$SCRIPT_DIR/" || exit

BUILD_DIR="build"

# Ensure the build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR" || { echo "Error: Failed to enter build directory ($BUILD_DIR)"; exit 1; }

# Function to compile the project
compile_project() {
    cmake .. && make
}

# First attempt to compile
if ! compile_project; then
    echo "Error: Initial build failed. Retrying after cleaning..."
    cd "$SCRIPT_DIR" || exit
    rm -rf "$BUILD_DIR"
    mkdir "$BUILD_DIR"
    cd "$BUILD_DIR" || exit

    # Second and final attempt
    if ! compile_project; then
        echo "Error: Build failed twice. Exiting..."
        exit 1
    fi
fi

echo "Build completed successfully!"
./prototype_map_system
