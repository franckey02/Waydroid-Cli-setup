#include "DebianInstaller.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

DebianInstaller::DebianInstaller(const SystemInfo& sys_info) 
    : system_info(sys_info) {}

bool DebianInstaller::install(bool unattended) {
    cout << "Installing Waydroid on " 
         << DISTRO_NAMES.at(system_info.distribution) << "...\n";
    
    // Detectar codename
    string codename;
    if (!detect_codename(codename)) {
        cerr << "Error: Could not detect distribution codename\n";
        return false;
    }
    
    cout << "Detected codename: " << codename << "\n";
    
    // Actualizar sistema
    if (!update_system()) {
        cerr << "Error: Failed to update system\n";
        return false;
    }
    
    // Instalar prerrequisitos
    if (!install_prerequisites()) {
        cerr << "Error: Failed to install prerequisites\n";
        return false;
    }
    
    // Agregar repositorio
    if (!add_repository(codename)) {
        cerr << "Error: Failed to add repository\n";
        return false;
    }
    
    // Instalar paquete
    if (!install_package()) {
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
    
    return true;
}

bool DebianInstaller::detect_codename(string& codename) const {
    // Intentar con lsb_release primero
    if (system("command -v lsb_release >/dev/null 2>&1") == 0) {
        FILE* pipe = popen("lsb_release -cs", "r");
        if (!pipe) return false;
        
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            codename = buffer;
            codename.erase(codename.find_last_not_of(" \n\r\t") + 1);
            pclose(pipe);
            return true;
        }
        pclose(pipe);
    }
    
    // Fallback: leer de /etc/os-release
    ifstream file("/etc/os-release");
    string line;
    
    while (getline(file, line)) {
        if (line.find("VERSION_CODENAME=") == 0) {
            codename = line.substr(17);
            // Eliminar comillas
            codename.erase(remove(codename.begin(), codename.end(), '"'), codename.end());
            return true;
        }
    }
    
    // Fallback para versiones conocidas
    if (system_info.distribution == Distro::UBUNTU) {
        if (!system_info.version.empty()) {
            if (system_info.version == "22.04") codename = "jammy";
            else if (system_info.version == "20.04") codename = "focal";
            else if (system_info.version == "18.04") codename = "bionic";
            else codename = "jammy"; // Default
            return true;
        }
    }
    
    return false;
}

bool DebianInstaller::update_system() const {
    cout << "Updating system...\n";
    return system("apt update && apt upgrade -y") == 0;
}

bool DebianInstaller::install_prerequisites() const {
    cout << "Installing prerequisites...\n";
    return system("apt install -y curl ca-certificates software-properties-common") == 0;
}

bool DebianInstaller::add_repository(const string& codename) const {
    cout << "Adding Waydroid repository...\n";
    
    // Método oficial via curl
    string cmd = "curl -s https://repo.waydro.id | bash -s " + codename;
    if (system(cmd.c_str()) == 0) {
        return true;
    }
    
    // Método alternativo manual
    cout << "Official method failed, trying alternative...\n";
    
    // Instalar gnupg
    if (system("apt install -y gnupg") != 0) {
        return false;
    }
    
    // Descargar clave GPG
    if (system("curl -fsSL https://repo.waydro.id/waydroid.gpg -o /tmp/waydroid.gpg") != 0) {
        return false;
    }
    
    // Mover clave
    if (system("mv /tmp/waydroid.gpg /usr/share/keyrings/waydroid.gpg") != 0) {
        return false;
    }
    
    // Agregar repositorio
    cmd = "echo \"deb [signed-by=/usr/share/keyrings/waydroid.gpg] https://repo.waydro.id/ " + 
          codename + " main\" | tee /etc/apt/sources.list.d/waydroid.list";
    
    return system(cmd.c_str()) == 0;
}

bool DebianInstaller::install_package() const {
    cout << "Installing Waydroid...\n";
    return system("apt update && apt install -y waydroid") == 0;
}

bool DebianInstaller::initialize_waydroid(bool with_gapps) const {
    cout << "Initializing Waydroid...\n";
    
    if (with_gapps) {
        cout << "Initializing with Google Apps (GAPPS)...\n";
        return system("waydroid init -s GAPPS") == 0;
    } else {
        cout << "Initializing without Google Apps...\n";
        return system("waydroid init") == 0;
    }
}

bool DebianInstaller::enable_service() const {
    cout << "Enabling service...\n";
    return system("systemctl enable --now waydroid-container.service") == 0;
}

bool DebianInstaller::configure_firewall() const {
    // Verificar si ufw está instalado y activo
    if (system("command -v ufw >/dev/null 2>&1") != 0) {
        return true; // No hay firewall ufw, nada que configurar
    }
    
    cout << "Configuring firewall...\n";
    
    // Agregar reglas
    bool success = true;
    success &= system("ufw allow 67/udp comment 'Waydroid DHCP'") == 0;
    success &= system("ufw allow 53/udp comment 'Waydroid DNS'") == 0;
    success &= system("ufw default allow FORWARD") == 0;
    
    return success;
}

bool DebianInstaller::ask_gapps_confirmation() const {
    string response;
    cout << "\nInitialize with Google Apps (GAPPS)? (Y/n): ";
    getline(cin, response);
    
    return response.empty() || response[0] == 'Y' || response[0] == 'y';
}
