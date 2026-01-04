#include "Installer.h"
#include <iostream>
#include <memory>
#include "installers/ArchInstaller.h"
#include "installers/DebianInstaller.h"
#include "installers/FedoraInstaller.h"
#include "installers/OpenSUSEInstaller.h"
#include "installers/VoidInstaller.h"
#include "installers/NixOSInstaller.h"

using namespace std;

Installer::Installer(const SystemInfo& sys_info) 
    : system_info(sys_info), unattended_mode(false) {
    
    // Inicializar instaladores específicos según sea necesario
    if (sys_info.distribution == Distro::ARCH || 
        sys_info.distribution == Distro::MANJARO) {
        arch_installer = make_unique<ArchInstaller>(sys_info);
    } else if (sys_info.distribution == Distro::DEBIAN || 
               sys_info.distribution == Distro::UBUNTU) {
        debian_installer = make_unique<DebianInstaller>(sys_info);
    } else if (sys_info.distribution == Distro::FEDORA || 
               sys_info.distribution == Distro::RHEL || 
               sys_info.distribution == Distro::CENTOS) {
        fedora_installer = make_unique<FedoraInstaller>(sys_info);
    } else if (sys_info.distribution == Distro::OPENSUSE) {
        opensuse_installer = make_unique<OpenSUSEInstaller>(sys_info);
    } else if (sys_info.distribution == Distro::VOID) {
        void_installer = make_unique<VoidInstaller>(sys_info);
    } else if (sys_info.distribution == Distro::NIXOS) {
        nixos_installer = make_unique<NixOSInstaller>(sys_info);
    }
}

Installer::~Installer() = default;

bool Installer::install(bool unattended) {
    unattended_mode = unattended;
    
    cout << "=== Waydroid C++ Installer ===\n\n";
    
    if (!system_info.is_root) {
        cerr << "Error: This installer requires root privileges.\n";
        cerr << "Please run with sudo or as root.\n";
        return false;
    }
    
    print_system_info();
    
    if (!check_requirements()) {
        return false;
    }
    
    if (!unattended_mode) {
        cout << "\nThis will install Waydroid on your system.\n";
        if (!ask_confirmation("Continue with installation?")) {
            cout << "Installation cancelled.\n";
            return false;
        }
    }
    
    bool result = false;
    
    switch (system_info.distribution) {
        case Distro::ARCH:
        case Distro::MANJARO:
            result = install_arch();
            break;
            
        case Distro::DEBIAN:
        case Distro::UBUNTU:
            result = install_debian();
            break;
            
        case Distro::FEDORA:
        case Distro::RHEL:
        case Distro::CENTOS:
            result = install_fedora();
            break;
            
        case Distro::OPENSUSE:
            result = install_opensuse();
            break;
            
        case Distro::VOID:
            result = install_void();
            break;
            
        case Distro::NIXOS:
            result = install_nixos();
            break;
            
        default:
            cerr << "Error: Unsupported distribution.\n";
            return false;
    }
    
    if (result) {
        cout << "\n=== Installation completed successfully ===\n";
        cout << "You can now start Waydroid with:\n";
        cout << "  waydroid session start\n";
        cout << "  waydroid show-full-ui\n";
    } else {
        cerr << "\n=== Installation failed ===\n";
    }
    
    return result;
}

void Installer::print_system_info() const {
    cout << "System Information:\n";
    cout << "  Distribution: ";
    
    auto it = DISTRO_NAMES.find(system_info.distribution);
    if (it != DISTRO_NAMES.end()) {
        cout << it->second;
    } else {
        cout << "Unknown";
    }
    
    if (!system_info.version.empty()) {
        cout << " " << system_info.version;
    }
    
    cout << "\n  Architecture: " << system_info.architecture;
    cout << "\n  Session: " << system_info.session_type;
    cout << "\n  Running as root: " << (system_info.is_root ? "Yes" : "No");
    cout << endl;
}

bool Installer::check_requirements() const {
    // Verificar sesión Wayland (recomendado)
    if (system_info.session_type != "wayland") {
        cout << "\nWarning: You are not in a Wayland session.\n";
        cout << "Waydroid works best in Wayland.\n";
        
        if (!unattended_mode) {
            if (!ask_confirmation("Continue anyway?")) {
                return false;
            }
        }
    }
    
    return true;
}

bool Installer::install_arch() {
    if (!arch_installer) {
        return false;
    }
    return arch_installer->install(unattended_mode);
}

bool Installer::install_debian() {
    if (!debian_installer) {
        return false;
    }
    return debian_installer->install(unattended_mode);
}

bool Installer::install_fedora() {
    if (!fedora_installer) {
        return false;
    }
    return fedora_installer->install(unattended_mode);
}

bool Installer::install_opensuse() {
    if (!opensuse_installer) {
        return false;
    }
    return opensuse_installer->install(unattended_mode);
}

bool Installer::install_void() {
    if (!void_installer) {
        return false;
    }
    return void_installer->install(unattended_mode);
}

bool Installer::install_nixos() {
    if (!nixos_installer) {
        return false;
    }
    return nixos_installer->install(unattended_mode);
}

bool Installer::ask_confirmation(const string& message) const {
    string response;
    cout << "\n" << message << " [Y/n]: ";
    getline(cin, response);
    
    if (response.empty() || response[0] == 'Y' || response[0] == 'y') {
        return true;
    }
    return false;
}
