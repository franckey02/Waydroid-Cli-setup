#!/bin/bash
# NixOS instructive guide for Waydroid

install_nixos() {
    echo "=== Waydroid Installation on NixOS ==="
    echo ""
    echo "NixOS uses declarative configuration. Follow these steps:"
    echo ""
    
    cat << 'EOF'
STEP 1: Edit /etc/nixos/configuration.nix

Add these configurations:

  # Kernel modules
  boot.kernelModules = [ "binder" "binder-devices" "binder_linux" ];
  boot.extraModulePackages = with config.boot.kernelPackages; [ binder_linux ];
  boot.extraModprobeConfig = ''
    options binder_linux devices="binder,hwbinder,vndbinder"
  '';

  # PSI parameter
  boot.kernelParams = [ "psi=1" ];

  # Packet forwarding
  networking.firewall.extraCommands = ''
    iptables -A FORWARD -i waydroid0 -j ACCEPT
    iptables -A FORWARD -o waydroid0 -j ACCEPT
  '';

  # Waydroid package
  environment.systemPackages = with pkgs; [
    waydroid
  ];

  # Enable service
  virtualisation.waydroid.enable = true;

  # Optional: Google Apps
  # virtualisation.waydroid.config.packages = [ "gapps" ];

STEP 2: Rebuild system

  sudo nixos-rebuild dry-build    # Test configuration
  sudo nixos-rebuild switch       # Apply changes

STEP 3: Post-installation

  # Initialize Waydroid
  sudo waydroid init
  # For GAPPS: sudo waydroid init -s GAPPS

  # Enable service
  sudo systemctl enable --now waydroid-container.service

  # Start session (as user)
  waydroid session start

STEP 4: Verify

  # Check service
  sudo systemctl status waydroid-container

  # Start full UI
  waydroid show-full-ui

TROUBLESHOOTING:

1. No internet in Waydroid:
   sudo sysctl -w net.ipv4.ip_forward=1
   sudo iptables -A FORWARD -i waydroid0 -j ACCEPT

2. Play Protect certification:
   Run inside waydroid shell:
   sudo waydroid shell
   ANDROID_RUNTIME_ROOT=/apex/com.android.runtime ANDROID_DATA=/data ANDROID_TZDATA_ROOT=/apex/com.android.tzdata ANDROID_I18N_ROOT=/apex/com.android.i18n sqlite3 /data/data/com.google.android.gsf/databases/gservices.db "select * from main where name = \"android_id\";"

RESOURCES:

• NixOS Wiki: https://nixos.wiki/wiki/Waydroid
• Options: https://search.nixos.org/options?query=waydroid
EOF

    echo ""
    echo "Waydroid configuration guide shown for NixOS"
    echo "Follow the steps above to install Waydroid."
}
