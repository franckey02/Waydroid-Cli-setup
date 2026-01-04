#!/bin/bash

# Waydroid Multi-Distro Installer CLI
# A unified tool to install and configure Waydroid across different distributions.

# Terminal Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

printf "${BLUE}Starting Waydroid Multi-Distro Installer...${NC}\n"

# Environment Detection
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$ID
else
    printf "${RED}Error: Could not detect your distribution.${NC}\n"
    exit 1
fi

case $OS in
    "arch")
        printf "${GREEN}Arch Linux detected. Installing package...${NC}\n"
        sudo pacman -S --noconfirm waydroid
        printf "${YELLOW}Requirement: Ensure you use a compatible kernel (e.g., linux-zen) or install binder_linux-dkms modules.${NC}\n"
        sudo systemctl enable --now waydroid-container.service
        ;;

    "ubuntu"|"debian"|"linuxmint"|"pop")
        printf "${GREEN}Debian/Ubuntu derivative detected. Adding official repository...${NC}\n"
        sudo apt update && sudo apt install -y curl ca-certificates
        curl -s https://repo.waydro.id | sudo bash
        sudo apt install -y waydroid
        ;;

    "fedora")
        if [ -f /etc/ostree/variant ]; then
            printf "${GREEN}Fedora Immutable (Silverblue/Kinoite) detected.${NC}\n"
            rpm-ostree install waydroid
        else
            printf "${GREEN}Fedora Workstation detected.${NC}\n"
            sudo dnf install -y waydroid
        fi
        printf "${YELLOW}Note: Use these URLs during initialization if prompted:${NC}\n"
        printf "System OTA: https://ota.waydro.id/system\nVendor OTA: https://ota.waydro.id/vendor\n"
        ;;

    "opensuse-tumbleweed"|"opensuse-slowroll")
        printf "${GREEN}openSUSE detected. Running system update...${NC}\n"
        sudo zypper dup -y
        # Add repository based on variant
        sudo zypper addrepo https://download.opensuse.org/repositories/home:runa-chin/openSUSE_Tumbleweed/home:runa-chin.repo && sudo zypper ref
        sudo zypper install -y grubby waydroid
        # Enable PSI=1
        sudo grubby --update-kernel="/boot/vmlinuz-$(uname -r)" --args="psi=1"
        sudo systemctl enable waydroid-container
        # AppArmor permission fix
        printf "${YELLOW}Configuring AppArmor for dnsmasq...${NC}\n"
        echo -e "@{run}/waydroid-lxc/ r,\n@{run}/waydroid-lxc/* rw," | sudo tee -a /etc/apparmor.d/usr.sbin.dnsmasq
        printf "${RED}CRITICAL: Please reboot your system to apply kernel parameters (PSI=1).${NC}\n"
        ;;

    "void")
        printf "${GREEN}Void Linux detected.${NC}\n"
        sudo xbps-install -S waydroid
        printf "${YELLOW}Check /usr/share/doc/waydroid/README.voidlinux for post-install steps.${NC}\n"
        ;;

    "nixos")
        printf "${BLUE}=== NixOS Installation Instructions ===${NC}\n"
        printf "NixOS is a declarative system. Please apply these changes manually:\n"
        printf "1. Open your ${YELLOW}/etc/nixos/configuration.nix${NC}\n"
        printf "2. Add: ${GREEN}virtualisation.waydroid.enable = true;${NC}\n"
        printf "3. Ensure graphics are enabled: ${GREEN}hardware.graphics.enable = true;${NC}\n"
        printf "4. Run: ${BLUE}sudo nixos-rebuild switch${NC}\n"
        exit 0
        ;;

    *)
        printf "${RED}Distribution '$OS' is not automatically supported yet.${NC}\n"
        exit 1
        ;;
esac

# Post-Installation Menu
printf "\n${BLUE}Post-Installation Options:${NC}\n"
printf "1) Initialize Waydroid (Vanilla)\n"
printf "2) Enable Multi-Window Mode\n"
printf "3) Get Google Services GSF ID (for Play Store registration)\n"
printf "4) Exit\n"
read -p "Select an option: " opt

case $opt in
    1)
        waydroid init
        ;;
    2)
        waydroid prop set persist.waydroid.multi_windows true
        printf "${GREEN}Multi-window enabled. Restart your Waydroid session.${NC}\n"
        ;;
    3)
        printf "${YELLOW}Make sure Waydroid is running before this step.${NC}\n"
        sudo waydroid shell ANDROID_RUNTIME_ROOT=/apex/com.android.runtime ANDROID_DATA=/data ANDROID_TZDATA_ROOT=/apex/com.android.tzdata ANDROID_I18N_ROOT=/apex/com.android.i18n sqlite3 /data/data/com.google.android.gsf/databases/gservices.db "select * from main where name = \"android_id\";"
        ;;
    *)
        printf "Exiting installer.\n"
        ;;
esac
