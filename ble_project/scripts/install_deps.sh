#!/bin/bash

set -e

echo "Installing BLE project dependencies..."

if [ -f /etc/debian_version ]; then
    sudo apt-get update
    sudo apt-get install -y \
        build-essential \
        cmake \
        git \
        libbluetooth-dev \
        libglib2.0-dev \
        libdbus-1-dev \
        pkg-config \
        bluez \
        doxygen \
        graphviz
elif [ -f /etc/redhat-release ]; then
    sudo yum install -y \
        gcc \
        gcc-c++ \
        cmake \
        git \
        bluez-libs-devel \
        glib2-devel \
        dbus-devel \
        pkgconfig \
        bluez \
        doxygen \
        graphviz
else
    echo "Unsupported distribution. Please install dependencies manually."
    exit 1
fi

echo "Dependencies installed successfully!"
