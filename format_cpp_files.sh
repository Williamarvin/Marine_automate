#!/bin/bash

# Check if clang-format is installed
if ! command -v clang-format &> /dev/null; then
    echo "clang-format could not be found. Please install it first."
    exit 1
fi

# Find and format all .cpp and .h files recursively
find . -name "*.cpp" -o -name "*.h" | while read -r file; do
    echo "Formatting $file"
    clang-format -i "$file"
done

echo "All C++ files formatted."
