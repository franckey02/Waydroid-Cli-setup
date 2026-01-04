#!/bin/bash
# Debian/Ubuntu installer for Waydroid

install_debian() {
    echo "Installing Waydroid on Debian/Ubuntu..."
    
    # Detect codename
    local codename
    if command -v lsb_release >/dev/null 2>&1; then
        codename=$(lsb_release -cs)
    else
        codename=$(grep "VERSION_CODENAME" /etc/os-release | cut -d= -f2)
    fi
    
    # Update system
    apt update
    apt upgrade -y
    
    # Install prerequisites
    apt install -y curl ca-certificates
    
    # Add Waydroid repository
    echo "Adding Waydroid repository..."
    curl -s https://repo.waydro.id | bash -s "$codename"
    
    # Install Waydroid
    apt update
    apt install -y waydroid
    
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
    
    echo "Waydroid installation completed on Debian/Ubuntu"
}
