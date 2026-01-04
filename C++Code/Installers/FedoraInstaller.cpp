#include "FedoraInstaller.h"
#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

FedoraInstaller::FedoraInstaller(const SystemInfo& sys_info) 
    : system_info(sys_info) {}

bool FedoraInstaller::install(bool unattended) {
    cout << "Installing Waydroid on " 
         << DISTRO_NAMES.at(system_info.distribution) << "...\n";
    
    if (is_immutable_variant()) {
        cout << "Detected immutable Fedora variant (Silverblue/Kinoite)\n";
        return install_immutable();
    } else {
        return install_regular(unattended);
    }
}

bool FedoraInstaller::is_immutable_variant() const {
    // Verificar si rpm-ostree está presente
    if (system("command -v rpm-ostree >/dev/null 2>&1") != 0) {
        return false;
    }
    
    // Verificar si es un sistema ostree
    FILE* pipe = popen("rpm-ostree status 2>/dev/null | grep -q ostree", "r");
    if (!pipe) return false;
    
    int result = pclose(pipe);
    return (result == 0);
}

bool FedoraInstaller::install_regular(bool unattended) const {
    cout << "Installing on regular Fedora...\n";
    
    // Actualizar sistema
    cout << "Updating system...\n";
    if (system("dnf update -y") != 0) {
        cerr << "Error: Failed to update system\n";
        return false;
    }
    
    // Instalar Waydroid
    cout << "Installing Waydroid...\n";
    if (system("dnf install -y waydroid") != 0) {
        cerr << "Error: Failed to install Waydroid\n";
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
    
    // Configurar firewall
    configure_firewall();
    
    cout << "\nFor Fedora, use these OTA URLs if needed:\n";
    cout << "System OTA: https://ota.waydro.id/system\n";
    cout << "Vendor OTA: https://ota.waydro.id/vendor\n";
    
    return true;
}

bool FedoraInstaller::install_immutable() const {
    cout << "Installing on immutable Fedora variant...\n";
    
    // Instalar con rpm-ostree
    cout << "Installing Waydroid with rpm-ostree...\n";
    if (system("rpm-ostree install waydroid") != 0) {
        cerr << "Error: Failed to install Waydroid with rpm-ostree\n";
        return false;
    }
    
    cout << "\nWaydroid installed successfully.\n";
    cout << "REBOOT REQUIRED to apply changes.\n";
    cout << "\nAfter reboot, run:\n";
    cout << "  sudo waydroid init\n";
    cout << "  sudo systemctl enable --now waydroid-container.service\n";
    
    return true;
}

bool FedoraInstaller::initialize_waydroid(bool with_gapps) const {
    cout << "Initializing Waydroid...\n";
    
    if (with_gapps) {
        cout << "Initializing with Google Apps (GAPPS)...\n";
        return system("waydroid init -s GAPPS") == 0;
    } else {
        cout << "Initializing without Google Apps...\n";
        return system("waydroid init") == 0;
    }
}

bool FedoraInstaller::enable_service() const {
    cout << "Enabling service...\n";
    return system("systemctl enable --now waydroid-container.service") == 0;
}

bool FedoraInstaller::configure_firewall() const {
    // Verificar si firewalld está instalado
    if (system("command -v firewall-cmd >/dev/null 2>&1") != 0) {
        return true; // No hay firewalld, nada que configurar
    }
    
    cout << "Configuring firewall...\n";
    
    bool success = true;
    
    // Agregar puertos a zona trusted
    success &= system("firewall-cmd --permanent --zone=trusted --add-port=67/udp") == 0;
    success &= system("firewall-cmd --permanent --zone=trusted --add-port=53/udp") == 0;
    success &= system("firewall-cmd --permanent --zone=trusted --add-forward") == 0;
    success &= system("firewall-cmd --permanent --zone=trusted --add-interface=waydroid0") == 0;
    success &= system("firewall-cmd --reload") == 0;
    
    return success;
}

bool FedoraInstaller::ask_gapps_confirmation() const {
    string response;
    cout << "\nInitialize with Google Apps (GAPPS)? (Y/n): ";
    getline(cin, response);
    
    return response.empty() || response[0] == 'Y' || response[0] == 'y';
}
