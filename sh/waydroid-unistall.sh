#!/bin/bash
# Waydroid Uninstaller - Remove Waydroid completely

set -e  # Exit on error

# Detect package manager
detect_package_manager() {
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
    else
        PKG_MANAGER="unknown"
    fi
}

# Check if running as root
check_root() {
    if [ "$EUID" -ne 0 ]; then
        echo "This script requires root privileges."
        echo "Please run with sudo or as root."
        exit 1
    fi
}

# Stop Waydroid services
stop_services() {
    echo "Stopping Waydroid services..."
    
    # Stop container if running
    if pgrep -f "waydroid-container" > /dev/null; then
        waydroid container stop 2>/dev/null || true
    fi
    
    # Stop systemd service if exists
    if systemctl list-unit-files | grep -q waydroid-container; then
        systemctl stop waydroid-container.service 2>/dev/null || true
        systemctl disable waydroid-container.service 2>/dev/null || true
    fi
    
    echo "Services stopped"
}

# Remove Waydroid package
remove_package() {
    detect_package_manager
    
    echo "Removing Waydroid package..."
    
    case $PKG_MANAGER in
        apt)
            apt remove --purge waydroid -y
            ;;
        pacman)
            pacman -Rns waydroid --noconfirm
            ;;
        dnf)
            dnf remove waydroid -y
            ;;
        zypper)
            zypper remove waydroid -y
            ;;
        xbps)
            xbps-remove -R waydroid -y
            ;;
        *)
            echo "Warning: Unknown package manager, manual removal required"
            ;;
    esac
    
    echo "Package removed"
}

# Remove configuration files
remove_configs() {
    echo "Removing configuration files..."
    
    # System configurations
    rm -rf /var/lib/waydroid
    rm -rf /var/lib/waydroid-lxc
    
    # User configurations
    rm -rf ~/waydroid
    rm -rf ~/.local/share/waydroid
    rm -rf ~/.local/share/applications/*aydroid* 2>/dev/null || true
    
    # AppArmor configurations
    rm -f /etc/apparmor.d/usr.sbin.dnsmasq.waydroid 2>/dev/null || true
    
    echo "Configuration files removed"
}

# Remove kernel modules if installed via DKMS
remove_kernel_modules() {
    echo "Checking for kernel modules..."
    
    if [ -d "/usr/src/binder_linux-"* ]; then
        echo "Removing binder_linux DKMS module..."
        dkms remove binder_linux --all 2>/dev/null || true
        rm -rf /usr/src/binder_linux-* 2>/dev/null || true
    fi
    
    # Remove module configurations
    rm -f /etc/modules-load.d/waydroid.conf 2>/dev/null || true
    rm -f /etc/modprobe.d/binder_linux.conf 2>/dev/null || true
    
    echo "Kernel modules cleaned up"
}

# Main uninstallation
main() {
    echo "=== Waydroid Uninstaller ==="
    echo "This will completely remove Waydroid from your system."
    echo ""
    
    # Confirmation
    read -p "Are you sure you want to continue? (y/N): " confirm
    if [[ ! $confirm =~ ^[Yy] ]]; then
        echo "Uninstallation cancelled."
        exit 0
    fi
    
    check_root
    
    # Execute removal steps
    stop_services
    remove_package
    remove_configs
    remove_kernel_modules
    
    echo ""
    echo "=== Uninstallation completed ==="
    echo "Waydroid has been completely removed from your system."
    echo "You may need to reboot for all changes to take effect."
}

# Show help
show_help() {
    cat << EOF
Waydroid Uninstaller - Complete removal of Waydroid

Usage: sudo ./waydroid-uninstall.sh

This script will:
1. Stop all Waydroid services
2. Remove Waydroid package
3. Delete all configuration files
4. Remove kernel modules (if installed via DKMS)
5. Clean up user data

Warning: This action cannot be undone!
EOF
}

# Parse arguments
if [[ "$1" == "-h" || "$1" == "--help" ]]; then
    show_help
    exit 0
fi

# Run main function
main
