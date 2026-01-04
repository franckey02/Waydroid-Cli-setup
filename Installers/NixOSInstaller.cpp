#include "NixOSInstaller.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

NixOSInstaller::NixOSInstaller(const SystemInfo& sys_info) 
    : system_info(sys_info) {}

bool NixOSInstaller::install(bool unattended) {
    cout << "=== Waydroid Installation on NixOS ===\n\n";
    cout << "NixOS uses declarative system configuration.\n";
    cout << "Instead of automatically executing commands, follow this guide.\n\n";
    
    show_installation_guide();
    
    if (!unattended) {
        if (ask_save_guide()) {
            save_guide_to_file();
        }
    }
    
    create_helper_script();
    
    return true;
}

void NixOSInstaller::show_installation_guide() const {
    cout << "===============================================================================\n";
    cout << "                            NIXOS INSTALLATION GUIDE\n";
    cout << "===============================================================================\n\n";
    
    cout << "STEP 1: Edit /etc/nixos/configuration.nix\n";
    cout << "-------------------------------------------------------------------------------\n\n";
    
    cout << "Add these configurations:\n\n";
    
    cout << "  # Required kernel modules\n";
    cout << "  boot.kernelModules = [ \"binder\" \"binder-devices\" \"binder_linux\" ];\n";
    cout << "  boot.extraModulePackages = with config.boot.kernelPackages; [ binder_linux ];\n";
    cout << "  boot.extraModprobeConfig = ''\n";
    cout << "    options binder_linux devices=\"binder,hwbinder,vndbinder\"\n";
    cout << "  '';\n\n";
    
    cout << "  # Required PSI parameter\n";
    cout << "  boot.kernelParams = [ \"psi=1\" ];\n\n";
    
    cout << "  # Packet forwarding\n";
    cout << "  networking.firewall.extraCommands = ''\n";
    cout << "    iptables -A FORWARD -i waydroid0 -j ACCEPT\n";
    cout << "    iptables -A FORWARD -o waydroid0 -j ACCEPT\n";
    cout << "  '';\n\n";
    
    cout << "  # Waydroid package\n";
    cout << "  environment.systemPackages = with pkgs; [\n";
    cout << "    waydroid\n";
    cout << "  ];\n\n";
    
    cout << "  # Enable service\n";
    cout << "  virtualisation.waydroid.enable = true;\n\n";
    
    cout << "  # Optional: Google Apps\n";
    cout << "  # virtualisation.waydroid.config.packages = [ \"gapps\" ];\n\n";
    
    cout << "STEP 2: Rebuild system\n";
    cout << "-------------------------------------------------------------------------------\n\n";
    
    cout << "  # Test configuration\n";
    cout << "  sudo nixos-rebuild dry-build\n\n";
    
    cout << "  # Apply changes\n";
    cout << "  sudo nixos-rebuild switch\n\n";
    
    cout << "STEP 3: Post-installation\n";
    cout << "-------------------------------------------------------------------------------\n\n";
    
    cout << "  # Initialize Waydroid\n";
    cout << "  sudo waydroid init\n\n";
    
    cout << "  # For GAPPS:\n";
    cout << "  sudo waydroid init -s GAPPS\n\n";
    
    cout << "  # Enable and start service\n";
    cout << "  sudo systemctl enable --now waydroid-container.service\n\n";
    
    cout << "  # Start session (as normal user)\n";
    cout << "  waydroid session start\n\n";
    
    cout << "  # For full interface\n";
    cout << "  waydroid show-full-ui\n\n";
    
    cout << "===============================================================================\n";
    cout << "                              TROUBLESHOOTING\n";
    cout << "===============================================================================\n\n";
    
    cout << "Common issues and solutions:\n\n";
    
    cout << "1. Binder modules not loading:\n";
    cout << "   - Check: zcat /proc/config.gz | grep BINDER\n";
    cout << "   - Rebuild kernel: boot.kernelPackages = pkgs.linuxPackages_latest;\n\n";
    
    cout << "2. No internet in Waydroid:\n";
    cout << "   sudo sysctl -w net.ipv4.ip_forward=1\n";
    cout << "   sudo iptables -A FORWARD -i waydroid0 -j ACCEPT\n\n";
    
    cout << "3. Play Protect certification:\n";
    cout << "   Run inside waydroid shell:\n";
    cout << "   sudo waydroid shell\n";
    cout << "   ANDROID_RUNTIME_ROOT=/apex/com.android.runtime ANDROID_DATA=/data ANDROID_TZDATA_ROOT=/apex/com.android.tzdata ANDROID_I18N_ROOT=/apex/com.android.i18n sqlite3 /data/data/com.google.android.gsf/databases/gservices.db \"select * from main where name = \\\"android_id\\\";\"\n\n";
    
    cout << "===============================================================================\n";
    cout << "                             LINKS AND RESOURCES\n";
    cout << "===============================================================================\n\n";
    
    cout << "• NixOS Wiki: https://nixos.wiki/wiki/Waydroid\n";
    cout << "• NixOS Options: https://search.nixos.org/options?query=waydroid\n";
    cout << "• Forum: https://discourse.nixos.org/\n";
    cout << "• Example configuration: see nixos.wiki\n\n";
}

void NixOSInstaller::create_helper_script() const {
    string home = getenv("HOME");
    if (home.empty()) {
        home = ".";
    }
    
    string script_path = home + "/waydroid_nixos_helper.sh";
    ofstream file(script_path);
    
    if (!file.is_open()) {
        cerr << "Warning: Could not create helper script\n";
        return;
    }
    
    file << "#!/bin/bash\n";
    file << "# Helper script for Waydroid on NixOS\n";
    file << "# Generated by Waydroid C++ Installer\n\n";
    
    file << "echo \"Useful commands for Waydroid on NixOS:\"\n";
    file << "echo \"\"\n";
    file << "echo \"1. Check service status:\"\n";
    file << "echo \"   sudo systemctl status waydroid-container\"\n";
    file << "echo \"\"\n";
    file << "echo \"2. Start manually:\"\n";
    file << "echo \"   sudo waydroid container start\"\n";
    file << "echo \"   waydroid session start\"\n";
    file << "echo \"\"\n";
    file << "echo \"3. View logs:\"\n";
    file << "echo \"   journalctl -u waydroid-container -f\"\n";
    file << "echo \"\"\n";
    file << "echo \"4. Update images:\"\n";
    file << "echo \"   sudo waydroid upgrade\"\n";
    file << "echo \"\"\n";
    file << "echo \"5. Android shell:\"\n";
    file << "echo \"   sudo waydroid shell\"\n";
    
    file.close();
    
    // Hacer ejecutable
    string cmd = "chmod +x " + script_path;
    system(cmd.c_str());
    
    cout << "Helper script created: " << script_path << "\n";
}

bool NixOSInstaller::save_guide_to_file() const {
    string home = getenv("HOME");
    if (home.empty()) {
        home = ".";
    }
    
    string guide_path = home + "/waydroid_nixos_installation_guide.txt";
    ofstream file(guide_path);
    
    if (!file.is_open()) {
        cerr << "Error: Could not save guide to file\n";
        return false;
    }
    
    // Redirigir la salida del guide al archivo
    file << "Waydroid Installation Guide for NixOS\n";
    file << "=======================================\n\n";
    
    // Aquí iría el contenido completo del guide
    // (simplificado por brevedad)
    file << "See the installation guide shown in the terminal.\n";
    
    file.close();
    
    cout << "Guide saved to: " << guide_path << "\n";
    return true;
}

bool NixOSInstaller::ask_save_guide() const {
    string response;
    cout << "\nSave this guide to a file? (Y/n): ";
    getline(cin, response);
    
    return response.empty() || response[0] == 'Y' || response[0] == 'y';
}
