#include "OpenSUSEInstaller.h"
#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

OpenSUSEInstaller::OpenSUSEInstaller(const SystemInfo& sys_info) 
    : system_info(sys_info) {}

bool OpenSUSEInstaller::install(bool unattended) {
    cout << "Installing Waydroid on openSUSE...\n";
    
    // Detectar versión
    string version = detect_version();
    cout << "Detected version: " << version << "\n";
    
    // Actualizar sistema
    if (!update_system()) {
        cerr << "Error: Failed to update system\n";
        return false;
    }
    
    // Agregar repositorio
    if (!add_repository(version)) {
        cerr << "Error: Failed to add repository\n";
        return false;
    }
    
    // Configurar kernel
    if (!configure_kernel()) {
        cerr << "Error: Failed to configure kernel\n";
        return false;
    }
    
    // Instalar paquete
    if (!install_package()) {
        cerr << "Error: Failed to install Waydroid\n";
        return false;
    }
    
    // Configurar AppArmor
    if (!configure_apparmor()) {
        cerr << "Error: Failed to configure AppArmor\n";
        return false;
    }
    
    // Inicializar
    if (!initialize_waydroid()) {
        return false;
    }
    
    // Habilitar servicio
    if (!enable_service()) {
        return false;
    }
    
    // Configurar firewall
    configure_firewall();
    
    // Mostrar advertencia de reinicio
    show_reboot_warning();
    
    return true;
}

string OpenSUSEInstaller::detect_version() const {
    // Leer de os-release
    ifstream file("/etc/os-release");
    string line;
    
    while (getline(file, line)) {
        if (line.find("PRETTY_NAME=") == 0) {
            string pretty_name = line.substr(12);
            // Eliminar comillas
            pretty_name.erase(remove(pretty_name.begin(), pretty_name.end(), '"'), 
                            pretty_name.end());
            
            if (pretty_name.find("Tumbleweed") != string::npos) {
                return "tumbleweed";
            } else if (pretty_name.find("Slowroll") != string::npos) {
                return "slowroll";
            }
        } else if (line.find("VERSION_ID=") == 0) {
            string version = line.substr(11);
            version.erase(remove(version.begin(), version.end(), '"'), version.end());
            
            if (version.find("15") == 0) {
                return "leap" + version;
            }
        }
    }
    
    // Default a Tumbleweed
    return "tumbleweed";
}

bool OpenSUSEInstaller::update_system() const {
    cout << "Updating system...\n";
    return system("zypper dup -y") == 0;
}

bool OpenSUSEInstaller::add_repository(const string& version) const {
    cout << "Adding Waydroid repository...\n";
    
    string repo_url;
    if (version.find("tumbleweed") != string::npos) {
        repo_url = "https://download.opensuse.org/repositories/home:runa-chin:Waydroid/openSUSE_Tumbleweed/home:runa-chin:Waydroid.repo";
    } else if (version.find("slowroll") != string::npos) {
        repo_url = "https://download.opensuse.org/repositories/home:runa-chin:Waydroid/openSUSE_Slowroll/home:runa-chin:Waydroid.repo";
    } else if (version.find("leap") != string::npos) {
        // Extraer número de versión
        string ver_num = version.substr(4); // Remove "leap"
        repo_url = "https://download.opensuse.org/repositories/home:runa-chin:Waydroid/openSUSE_Leap_" + 
                   ver_num + "/home:runa-chin:Waydroid.repo";
    } else {
        // Default a Tumbleweed
        repo_url = "https://download.opensuse.org/repositories/home:runa-chin:Waydroid/openSUSE_Tumbleweed/home:runa-chin:Waydroid.repo";
    }
    
    string cmd = "zypper addrepo -f " + repo_url + " waydroid";
    if (system(cmd.c_str()) != 0) {
        return false;
    }
    
    // Actualizar repositorios
    return system("zypper refresh") == 0;
}

bool OpenSUSEInstaller::configure_kernel() const {
    cout << "Configuring kernel parameters...\n";
    
    // Instalar grubby
    if (system("zypper install -y grubby") != 0) {
        cerr << "Warning: Failed to install grubby\n";
        return false;
    }
    
    // Obtener kernel actual
    FILE* pipe = popen("uname -r", "r");
    if (!pipe) return false;
    
    char kernel_version[256];
    if (fgets(kernel_version, sizeof(kernel_version), pipe) == nullptr) {
        pclose(pipe);
        return false;
    }
    pclose(pipe);
    
    // Eliminar newline
    string kernel_str(kernel_version);
    kernel_str.erase(kernel_str.find_last_not_of(" \n\r\t") + 1);
    
    // Agregar parámetro psi=1
    string cmd = "grubby --update-kernel=/boot/vmlinuz-" + kernel_str + 
                 " --args=\"psi=1\"";
    
    return system(cmd.c_str()) == 0;
}

bool OpenSUSEInstaller::install_package() const {
    cout << "Installing Waydroid...\n";
    return system("zypper install -y waydroid") == 0;
}

bool OpenSUSEInstaller::configure_apparmor() const {
    cout << "Configuring AppArmor...\n";
    
    // Crear archivo de configuración para dnsmasq
    ofstream file("/etc/apparmor.d/usr.sbin.dnsmasq.waydroid");
    if (!file.is_open()) {
        cerr << "Error: Cannot create AppArmor configuration file\n";
        return false;
    }
    
    file << "@{run}/waydroid-lxc/ r,\n";
    file << "@{run}/waydroid-lxc/* rw,\n";
    file.close();
    
    // Recargar AppArmor
    return system("systemctl reload apparmor") == 0;
}

bool OpenSUSEInstaller::initialize_waydroid() const {
    cout << "Initializing Waydroid...\n";
    return system("waydroid init") == 0;
}

bool OpenSUSEInstaller::enable_service() const {
    cout << "Enabling service...\n";
    return system("systemctl enable --now waydroid-container.service") == 0;
}

bool OpenSUSEInstaller::configure_firewall() const {
    // openSUSE generalmente usa firewalld o SuSEfirewall2
    cout << "Configuring firewall...\n";
    
    if (system("command -v firewall-cmd >/dev/null 2>&1") == 0) {
        // Usar firewalld
        bool success = true;
        success &= system("firewall-cmd --permanent --zone=trusted --add-port=67/udp") == 0;
        success &= system("firewall-cmd --permanent --zone=trusted --add-port=53/udp") == 0;
        success &= system("firewall-cmd --permanent --zone=trusted --add-forward") == 0;
        success &= system("firewall-cmd --reload") == 0;
        return success;
    }
    
    return true; // No hay firewall configurable encontrado
}

void OpenSUSEInstaller::show_reboot_warning() const {
    cout << "\nIMPORTANT: REBOOT REQUIRED\n";
    cout << "openSUSE requires a reboot for kernel parameter changes (psi=1)\n";
    cout << "\nAfter reboot, Waydroid will be ready to use.\n";
}
