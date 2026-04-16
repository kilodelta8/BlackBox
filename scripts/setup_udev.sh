#!/bin/bash
# Project BlackBox - Device Persistent Naming Setup
# Run this once on the Pi to ensure dongles always map to the same name.

cat <<EOF | sudo tee /etc/udev/rules.d/99-blackbox.rules
# RTL-SDR V4
SUBSYSTEM=="usb", ATTRS{idVendor}=="0bda", ATTRS{idProduct}=="2838", SYMLINK+="bb_sdr"

# Alfa Wi-Fi (AWUS036ACM)
SUBSYSTEM=="usb", ATTRS{idVendor}=="0e8d", ATTRS{idProduct}=="7612", SYMLINK+="bb_wifi"

# USB GPS
SUBSYSTEM=="usb", ATTRS{idVendor}=="1546", ATTRS{idProduct}=="01a7", SYMLINK+="bb_gps"

# Yaesu FT-891 CAT Control
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", SYMLINK+="bb_yaesu"
EOF

sudo udevadm control --reload-rules && sudo udevadm trigger
echo "Udev rules applied. Devices will now appear as /dev/bb_*"
