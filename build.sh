#!/bin/bash

# Define the build directory name
BUILD_DIR=build

# Check if the build directory exists, if not, create it
if [ ! -d "$BUILD_DIR" ]; then
	mkdir $BUILD_DIR
fi

# Change into the build directory
cd $BUILD_DIR

# Run CMake to configure the project and generate the build system
cmake ..

# Build the project
make -j12

# Optionally, change back to the original directory
cd ..
