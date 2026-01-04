#include "Uninstaller.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <filesystem>

using namespace std;

Uninstaller::Uninstaller(const SystemInfo& sys_info) 
    : system_info(sys_info), unattended_mode(false) {}

bool Uninstaller::uninstall(bool unattended) {
    unattended_mode = unattended;
    
    cout << "=== Waydroid Uninstaller ===\n\n";
    
    if (!system_info.is_root) {
        cerr << "Error: Uninstaller requires root privileges.\n";
        return false;
    }
    
    if (!unattended_mode) {
        cout << "This will completely remove Waydroid from your system.\n";
        cout << "This action cannot be undone!\n";
        
        if (!ask_confirmation("Are you sure you want to continue?")) {
            cout << "Uninstallation cancelled.\n";
            return false;
        }
    }
    
    cout << "\nStarting uninstallation...\n";
    
    // 1. Detener servicios
    cout << "1. Stopping services...\n";
    if (!stop_services()) {
        cerr << "Warning: Failed to stop some services\n";
    }
    
    // 2. Remover paquete
    cout << "2. Removing package...\n";
    if (!remove_package()) {
        cerr << "Error: Failed to remove package\n";
        return false;
    }
    
    // 3. Remover archivos de configuración
    cout << "3. Removing configuration files...\n";
    if (!remove_config_files()) {
        cerr << "Warning: Failed to remove some configuration files\n";
    }
    
    // 4. Remover módulos del kernel
    cout << "4. Removing kernel modules...\n";
    if (!remove_kernel_modules()) {
        cout << "No kernel modules to remove\n";
    }
    
    cout << "\n=== Uninstallation completed ===\n";
    cout << "Waydroid has been completely removed from your system.\n";
    cout << "You may need to reboot for all changes to take effect.\n";
    
    return true;
}

bool Uninstaller::is_installed() const {
    // Verificar si el comando waydroid existe
    return exec_system_command("command -v waydroid >/dev/null 2>&1");
}

bool Uninstaller::remove_package() {
    string cmd;
    
    switch (system_info.package_manager) {
        case PackageManager::PACMAN:
            cmd = "pacman -Rns waydroid --noconfirm";
            break;
            
        case PackageManager::APT:
            cmd = "apt remove --purge waydroid -y";
            break;
            
        case PackageManager::DNF:
        case PackageManager::YUM:
            cmd = "dnf remove waydroid -y";
            break;
            
        case PackageManager::ZYPPER:
            cmd = "zypper remove waydroid -y";
            break;
            
        case PackageManager::XBPS:
            cmd = "xbps-remove -R waydroid -y";
            break;
            
        default:
            cerr << "Error: Unknown package manager\n";
            return false;
    }
    
    return exec_system_command(cmd);
}

bool Uninstaller::remove_config_files() {
    bool success = true;
    
    // Lista de directorios y archivos a eliminar
    vector<string> paths = {
        "/var/lib/waydroid",
        "/var/lib/waydroid-lxc",
        "/etc/modules-load.d/waydroid.conf",
        "/etc/modprobe.d/binder_linux.conf",
        "/etc/apparmor.d/usr.sbin.dnsmasq.waydroid"
    };
    
    for (const auto& path : paths) {
        if (file_exists(path)) {
            string cmd = "rm -rf " + path;
            if (!exec_system_command(cmd)) {
                cerr << "Warning: Failed to remove " << path << "\n";
                success = false;
            }
        }
    }
    
    // Directorios de usuario (si existen)
    string home = getenv("HOME");
    if (!home.empty()) {
        vector<string> user_paths = {
            home + "/waydroid",
            home + "/.local/share/waydroid",
            home + "/.local/share/applications/*aydroid*"
        };
        
        for (const auto& path : user_paths) {
            string cmd = "rm -rf " + path;
            exec_system_command(cmd); // No es crítico si falla
        }
    }
    
    return success;
}

bool Uninstaller::remove_kernel_modules() {
    bool success = true;
    
    // Verificar si hay módulos DKMS instalados
    if (file_exists("/usr/src/binder_linux-")) {
        string cmd = "dkms remove binder_linux --all";
        if (!exec_system_command(cmd)) {
            cerr << "Warning: Failed to remove DKMS modules\n";
            success = false;
        }
        
        cmd = "rm -rf /usr/src/binder_linux-*";
        exec_system_command(cmd);
    }
    
    return success;
}

bool Uninstaller::stop_services() {
    bool success = true;
    
    // Detener contenedor si está corriendo
    if (exec_system_command("pgrep -f waydroid-container >/dev/null 2>&1")) {
        if (!exec_system_command("waydroid container stop 2>/dev/null")) {
            cerr << "Warning: Failed to stop waydroid container\n";
            success = false;
        }
    }
    
    // Detener servicio systemd si existe
    if (exec_system_command("systemctl list-unit-files | grep -q waydroid-container")) {
        if (!exec_system_command("systemctl stop waydroid-container.service 2>/dev/null")) {
            cerr << "Warning: Failed to stop systemd service\n";
            success = false;
        }
        
        if (!exec_system_command("systemctl disable waydroid-container.service 2>/dev/null")) {
            cerr << "Warning: Failed to disable systemd service\n";
            success = false;
        }
    }
    
    return success;
}

bool Uninstaller::ask_confirmation(const string& message) const {
    string response;
    cout << "\n" << message << " (y/N): ";
    getline(cin, response);
    
    if (!response.empty() && (response[0] == 'Y' || response[0] == 'y')) {
        return true;
    }
    return false;
}

bool Uninstaller::exec_system_command(const string& cmd) const {
    int result = system(cmd.c_str());
    return result == 0;
}

bool Uninstaller::file_exists(const string& path) const {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}
