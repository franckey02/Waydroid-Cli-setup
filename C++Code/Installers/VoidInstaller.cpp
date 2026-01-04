#include "VoidInstaller.h"
#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

VoidInstaller::VoidInstaller(const SystemInfo& sys_info) 
    : system_info(sys_info) {}

bool VoidInstaller::install(bool unattended) {
    cout << "Installing Waydroid on Void Linux...\n";
    
    // Actualizar sistema
    if (!update_system()) {
        cerr << "Error: Failed to update system\n";
        return false;
    }
    
    // Instalar paquete
    if (!install_package()) {
        cerr << "Error: Failed to install Waydroid\n";
        return false;
    }
    
    // Mostrar README de Void
    show_void_readme();
    
    // Inicializar
    bool with_gapps = false;
    if (!unattended) {
        with_gapps = ask_gapps_confirmation();
    }
    
    if (!initialize_waydroid(with_gapps)) {
        return false;
    }
    
    // Habilitar servicio (runit)
    if (!enable_service()) {
        return false;
    }
    
    return true;
}

bool VoidInstaller::update_system() const {
    cout << "Updating system...\n";
    return system("xbps-install -Su -y") == 0;
}

bool VoidInstaller::install_package() const {
    cout << "Installing Waydroid...\n";
    return system("xbps-install -y waydroid") == 0;
}

bool VoidInstaller::initialize_waydroid(bool with_gapps) const {
    cout << "Initializing Waydroid...\n";
    
    if (with_gapps) {
        cout << "Initializing with Google Apps (GAPPS)...\n";
        return system("waydroid init -s GAPPS") == 0;
    } else {
        cout << "Initializing without Google Apps...\n";
        return system("waydroid init") == 0;
    }
}

bool VoidInstaller::enable_service() const {
    cout << "Enabling service (runit)...\n";
    
    // Void Linux usa runit, crear enlace simbólico
    return system("ln -sf /etc/sv/waydroid-container /var/service/") == 0;
}

void VoidInstaller::show_void_readme() const {
    string readme_path = "/usr/share/doc/waydroid/README.voidlinux";
    ifstream file(readme_path);
    
    if (file.is_open()) {
        cout << "\nVoid Linux specific instructions found in:\n";
        cout << readme_path << "\n\n";
        
        // Mostrar primeras líneas
        string line;
        int count = 0;
        while (getline(file, line) && count < 5) {
            cout << line << "\n";
            count++;
        }
        
        if (count == 5) {
            cout << "... (more in " << readme_path << ")\n";
        }
        
        cout << "\n";
        file.close();
    }
}

bool VoidInstaller::ask_gapps_confirmation() const {
    string response;
    cout << "\nInitialize with Google Apps (GAPPS)? (Y/n): ";
    getline(cin, response);
    
    return response.empty() || response[0] == 'Y' || response[0] == 'y';
}
