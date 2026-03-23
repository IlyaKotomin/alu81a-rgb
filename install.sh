#!/usr/bin/env bash
set -euo pipefail

PREFIX="${PREFIX:-/usr/local}"

for cmd in gcc make; do
    if ! command -v "$cmd" &>/dev/null; then
        echo "Missing dependency: $cmd"
        echo "Install it and try again."
        exit 1
    fi
done

make clean
make PREFIX="$PREFIX"

if [[ "$EUID" -ne 0 ]]; then
    sudo make PREFIX="$PREFIX" install
    sudo udevadm control --reload-rules && sudo udevadm trigger
    sudo usermod -aG input "${SUDO_USER:-$USER}"
else
    make PREFIX="$PREFIX" install
    udevadm control --reload-rules && udevadm trigger
    usermod -aG input "$USER"
fi

echo ""
echo "Done. Log out and back in for the input group change to take effect."
echo "Try: alu81a-rgb --color red"
