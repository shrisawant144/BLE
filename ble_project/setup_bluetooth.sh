#!/bin/bash

# BLE Setup Script
# Diagnoses and fixes common Bluetooth issues for gattlib applications

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "========================================="
echo "  BLE Setup & Diagnostic Script"
echo "========================================="
echo

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo -e "${YELLOW}Warning: Some commands require root. Re-run with sudo if issues persist.${NC}"
    echo
fi

# 1. Check for Bluetooth adapter
echo "[1/5] Checking for Bluetooth adapter..."
if hciconfig 2>/dev/null | grep -q "hci"; then
    ADAPTER=$(hciconfig | grep "hci" | head -1 | cut -d: -f1)
    echo -e "${GREEN}✓ Found Bluetooth adapter: $ADAPTER${NC}"
else
    echo -e "${RED}✗ No Bluetooth adapter found!${NC}"
    echo "  Please check if your Bluetooth dongle/hardware is connected."
    exit 1
fi

# 2. Check Bluetooth service status
echo
echo "[2/5] Checking Bluetooth service..."
if systemctl is-active --quiet bluetooth; then
    echo -e "${GREEN}✓ Bluetooth service is running${NC}"
else
    echo -e "${YELLOW}! Bluetooth service is not running${NC}"
    echo "  Starting Bluetooth service..."
    if [ "$EUID" -eq 0 ]; then
        systemctl start bluetooth
        sleep 2
        if systemctl is-active --quiet bluetooth; then
            echo -e "${GREEN}✓ Bluetooth service started successfully${NC}"
        else
            echo -e "${RED}✗ Failed to start Bluetooth service${NC}"
            systemctl status bluetooth --no-pager
            exit 1
        fi
    else
        echo -e "${YELLOW}  Run: sudo systemctl start bluetooth${NC}"
    fi
fi

# 3. Check if adapter is UP
echo
echo "[3/5] Checking adapter state..."
if hciconfig "$ADAPTER" 2>/dev/null | grep -q "UP RUNNING"; then
    echo -e "${GREEN}✓ Adapter is UP and RUNNING${NC}"
else
    echo -e "${YELLOW}! Adapter is DOWN${NC}"
    echo "  Bringing adapter up..."
    if [ "$EUID" -eq 0 ]; then
        hciconfig "$ADAPTER" up
        sleep 1
        if hciconfig "$ADAPTER" | grep -q "UP RUNNING"; then
            echo -e "${GREEN}✓ Adapter is now UP${NC}"
        else
            echo -e "${RED}✗ Failed to bring adapter up${NC}"
            exit 1
        fi
    else
        echo -e "${YELLOW}  Run: sudo hciconfig $ADAPTER up${NC}"
    fi
fi

# 4. Check D-Bus (required by gattlib)
echo
echo "[4/5] Checking D-Bus service..."
if systemctl is-active --quiet dbus; then
    echo -e "${GREEN}✓ D-Bus service is running${NC}"
else
    echo -e "${RED}✗ D-Bus service is not running${NC}"
    echo "  This is required for gattlib to work."
    if [ "$EUID" -eq 0 ]; then
        systemctl start dbus
    else
        echo -e "${YELLOW}  Run: sudo systemctl start dbus${NC}"
    fi
fi

# 5. Optional: Enable Bluetooth service at boot
echo
echo "[5/5] Checking if Bluetooth is enabled at boot..."
if systemctl is-enabled --quiet bluetooth 2>/dev/null; then
    echo -e "${GREEN}✓ Bluetooth service is enabled at boot${NC}"
else
    echo -e "${YELLOW}! Bluetooth service is NOT enabled at boot${NC}"
    echo "  To enable auto-start at boot, run:"
    echo -e "${YELLOW}  sudo systemctl enable bluetooth${NC}"
fi

# Summary
echo
echo "========================================="
echo "  Summary"
echo "========================================="
echo
hciconfig "$ADAPTER" | head -3
echo
echo -e "${GREEN}Setup complete! You can now run your BLE application:${NC}"
echo "  sudo ./ble_scan"
echo
