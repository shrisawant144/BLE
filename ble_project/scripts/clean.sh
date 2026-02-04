#!/bin/bash

echo "Cleaning BLE project..."

rm -rf build/
find . -name "*.o" -delete
find . -name "*.a" -delete
find . -name "*.so" -delete
find . -name "compile_commands.json" -delete
find . -name "CMakeCache.txt" -delete
find . -name "CMakeFiles" -type d -exec rm -rf {} + 2>/dev/null || true

echo "Clean complete!"
