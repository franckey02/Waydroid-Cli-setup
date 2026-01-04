#!/bin/bash
# Waydroid Installer - Multi-distro CLI installer
# Supported: Arch, Void, Debian/Ubuntu, Fedora, openSUSE, NixOS

set -e  # Exit on error

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALLERS_DIR="$SCRIPT_DIR/modules/installers"

# Global variables
DISTRO=""
VERSION=""
PKG_MANAGER=""
ARCH=""
UNATTENDED=false

# Detect system
detect_system() {
    echo "Detecting system..."
    
    # Detect architecture
    ARCH=$(uname -m)
    
    # Detect distribution
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
        VERSION=$VERSION_ID
        
        # Handle derivatives
        case $ID in
            linuxmint|popos|elementary|zorin)
                DISTRO="debian"
                ;;
            manjaro|endeavouros)
                DISTRO="arch"
                ;;
            rhel|centos)
                DISTRO="fedora"
                ;;
        esac
    elif [ -f /etc/void-release ]; then
        DISTRO="void"
        VERSION=$(cat /etc/void-release | awk '{print $2}')
    fi
    
    # Detect package manager
    if command -v apt >/dev/null 2>&1; then
        PKG_MANAGER="apt"
    elif command -v pacman >/dev/null 2>&1; then
        PKG_MANAGER="pacman"
    elif command -v dnf >/dev/null 2>&1; then
        PKG_MANAGER="dnf"
    elif command -v zypper >/dev/null 2>&1; then
        PKG_MANAGER="zypper"
    elif command -v xbps-install >/dev/null 2>&1; then
        PKG_MANAGER="xbps"
    elif command -v nix-env >/dev/null 2>&1; then
        PKG_MANAGER="nix"
    else
        PKG_MANAGER="unknown"
    fi
    
    echo "Detected: $DISTRO $VERSION ($ARCH)"
    echo "Package manager: $PKG_MANAGER"
}

# Check if running as root
check_root() {
    if [ "$EUID" -ne 0 ]; then
        echo "This script requires root privileges."
        echo "Please run with sudo or as root."
        exit 1
    fi
}

# Main installation function
install_waydroid() {
    detect_system
    
    if [ "$PKG_MANAGER" = "unknown" ]; then
        echo "Error: Could not detect package manager"
        exit 1
    fi
    
    # Load appropriate installer
    case $DISTRO in
        arch)
            source "$INSTALLERS_DIR/arch.sh"
            install_arch
            ;;
        void)
            source "$INSTALLERS_DIR/void.sh"
            install_void
            ;;
        debian|ubuntu)
            source "$INSTALLERS_DIR/debian.sh"
            install_debian
            ;;
        fedora)
            source "$INSTALLERS_DIR/fedora.sh"
            install_fedora
            ;;
        opensuse|suse)
            source "$INSTALLERS_DIR/opensuse.sh"
            install_opensuse
            ;;
        nixos)
            source "$INSTALLERS_DIR/nixos.sh"
            install_nixos
            ;;
        *)
            echo "Error: Unsupported distribution: $DISTRO"
            exit 1
            ;;
    esac
    
    echo "Waydroid installation completed!"
}

# Parse arguments
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -u|--unattended)
                UNATTENDED=true
                shift
                ;;
            *)
                echo "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
}

# Show help
show_help() {
    cat << EOF
Waydroid Installer - Multi-distro CLI installer

Usage: $0 [OPTIONS]

Options:
  -h, --help       Show this help message
  -u, --unattended Unattended mode (no questions)

Supported distributions:
  Arch Linux and derivatives
  Void Linux
  Debian/Ubuntu and derivatives
  Fedora/RHEL/CentOS
  openSUSE
  NixOS (instructive mode)

Examples:
  sudo ./waydroid-installer.sh
  sudo ./waydroid-installer.sh --unattended
EOF
}

# Main execution
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    parse_args "$@"
    check_root
    install_waydroid
fi
