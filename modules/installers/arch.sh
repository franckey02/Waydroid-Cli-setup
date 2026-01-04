#!/bin/bash
# Arch Linux installer for Waydroid

install_arch() {
    echo "Installing Waydroid on Arch Linux..."
    
    # Update system
    pacman -Syu --noconfirm
    
    # Install Waydroid
    pacman -S waydroid --noconfirm
    
    # Check for AUR if waydroid not found
    if ! command -v waydroid >/dev/null 2>&1; then
        echo "Waydroid not in official repos, checking AUR..."
        
        # Check for yay
        if command -v yay >/dev/null 2>&1; then
            yay -S waydroid --noconfirm
        elif command -v paru >/dev/null 2>&1; then
            paru -S waydroid --noconfirm
        else
            echo "Error: Waydroid not found and no AUR helper installed."
            echo "Install yay or paru, then run: yay -S waydroid"
            exit 1
        fi
    fi
    
    # Initialize Waydroid
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
    
    echo "Waydroid installation completed on Arch Linux"
}
