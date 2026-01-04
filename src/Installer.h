#ifndef INSTALLER_H
#define INSTALLER_H

#include "common.h"
#include <string>
#include <memory>

// Forward declarations para instaladores específicos
class ArchInstaller;
class DebianInstaller;
class FedoraInstaller;
class OpenSUSEInstaller;
class VoidInstaller;
class NixOSInstaller;

class Installer {
public:
    Installer(const SystemInfo& sys_info);
    ~Installer();
    
    // Instalar Waydroid
    bool install(bool unattended = false);
    
    // Mostrar información del sistema
    void print_system_info() const;
    
    // Verificar requisitos
    bool check_requirements() const;
    
private:
    SystemInfo system_info;
    bool unattended_mode;
    
    // Punteros a instaladores específicos
    std::unique_ptr<ArchInstaller> arch_installer;
    std::unique_ptr<DebianInstaller> debian_installer;
    std::unique_ptr<FedoraInstaller> fedora_installer;
    std::unique_ptr<OpenSUSEInstaller> opensuse_installer;
    std::unique_ptr<VoidInstaller> void_installer;
    std::unique_ptr<NixOSInstaller> nixos_installer;
    
    // Métodos auxiliares
    bool install_arch();
    bool install_debian();
    bool install_fedora();
    bool install_opensuse();
    bool install_void();
    bool install_nixos();
    
    // Funciones comunes
    bool ask_confirmation(const std::string& message) const;
    void show_gapps_prompt();
    bool initialize_waydroid(bool with_gapps = false);
    bool enable_service();
};

#endif // INSTALLER_H
