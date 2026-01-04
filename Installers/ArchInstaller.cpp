#include "ArchInstaller.h"
#include <iostream>
#include <cstdlib>
#include <memory>

using namespace std;

ArchInstaller::ArchInstaller(const SystemInfo& sys_info) 
    : system_info(sys_info) {}

bool ArchInstaller::install(bool unattended) {
    cout << "Installing Waydroid on Arch Linux...\n";
    
    // Actualizar sistema
    cout << "Updating system...\n";
    if (system("pacman -Syu --noconfirm") != 0) {
        cerr << "Error: Failed to update system\n";
        return false;
    }
    
    // Intentar instalar desde repos oficiales
    cout << "Installing Waydroid...\n";
    bool installed = false;
    
    if (system("pacman -S waydroid --noconfirm") == 0) {
        installed = true;
    } else {
        // Intentar desde AUR
        cout << "Waydroid not in official repos, checking AUR...\n";
        if (check_aur_helper()) {
            installed = install_with_aur("yay");
        } else if (system("command -v paru >/dev/null 2>&1") == 0) {
            installed = install_with_aur("paru");
        } else {
            cerr << "Error: No AUR helper found. Install yay or paru.\n";
            return false;
        }
    }
    
    if (!installed) {
        return false;
    }
    
    // Inicializar
    bool with_gapps = false;
    if (!unattended) {
        with_gapps = ask_gapps_confirmation();
    }
    
    if (!initialize_waydroid(with_gapps)) {
        return false;
    }
    
    // Habilitar servicio
    if (!enable_service()) {
        return false;
    }
    
    return true;
}

bool ArchInstaller::check_aur_helper() {
    return system("command -v yay >/dev/null 2>&1") == 0;
}

bool ArchInstaller::install_with_aur(const string& helper) {
    string cmd = helper + " -S waydroid --noconfirm";
    return system(cmd.c_str()) == 0;
}

bool ArchInstaller::initialize_waydroid(bool with_gapps) {
    cout << "Initializing Waydroid...\n";
    
    if (with_gapps) {
        cout << "Initializing with Google Apps (GAPPS)...\n";
        return system("waydroid init -s GAPPS") == 0;
    } else {
        cout << "Initializing without Google Apps...\n";
        return system("waydroid init") == 0;
    }
}

bool ArchInstaller::enable_service() {
    cout << "Enabling service...\n";
    return system("systemctl enable --now waydroid-container.service") == 0;
}

bool ArchInstaller::ask_gapps_confirmation() const {
    string response;
    cout << "\nInitialize with Google Apps (GAPPS)? (Y/n): ";
    getline(cin, response);
    
    return response.empty() || response[0] == 'Y' || response[0] == 'y';
}
