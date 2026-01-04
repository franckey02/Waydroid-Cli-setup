#!/bin/bash
# Fedora installer for Waydroid

install_fedora() {
    echo "Installing Waydroid on Fedora..."
    
    # Check if immutable variant
    if command -v rpm-ostree >/dev/null 2>&1; then
        echo "Detected immutable Fedora variant (Silverblue/Kinoite)"
        rpm-ostree install waydroid
        echo "Waydroid installed. Reboot required."
        echo "After reboot, run: sudo waydroid init"
        exit 0
    fi
    
    # Regular Fedora installation
    dnf update -y
    dnf install -y waydroid
    
    # Initialize
    echo "Initializing Waydroid..."
    
    if [ "$UNATTENDED" = true ]; then
        waydroid init
    else
        read -p "Initialize with Google Apps (GAPPS)? (Y/n): " gapps_choice
        if [[ ! $gapps_choice =~ ^[Nn] ]]; then
            waydroid init -s GAPPS
        else
            waydroid init
        fi
    fi
    
    # Enable service
    systemctl enable --now waydroid-container.service
    
    echo "Waydroid installation completed on Fedora"
    echo "For Fedora, use these OTA URLs if needed:"
    echo "System OTA: https://ota.waydro.id/system"
    echo "Vendor OTA: https://ota.waydro.id/vendor"
}
