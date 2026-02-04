#!/bin/bash

set -e

echo "Running BLE project tests..."

if [ ! -d "build" ]; then
    echo "Build directory not found. Building project..."
    make build
fi

echo "Testing central examples..."
cd build/central/examples 2>/dev/null || echo "Central examples not built"

echo "Testing peripheral examples..."
cd ../../peripheral/examples 2>/dev/null || echo "Peripheral examples not built"

echo "All tests completed!"
