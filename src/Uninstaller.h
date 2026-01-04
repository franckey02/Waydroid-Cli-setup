#ifndef UNINSTALLER_H
#define UNINSTALLER_H

#include "common.h"
#include <string>

class Uninstaller {
public:
    Uninstaller(const SystemInfo& sys_info);
    
    // Desinstalar Waydroid completamente
    bool uninstall(bool unattended = false);
    
    // Verificar si Waydroid está instalado
    bool is_installed() const;
    
private:
    SystemInfo system_info;
    bool unattended_mode;
    
    // Métodos de desinstalación específicos
    bool remove_package();
    bool remove_config_files();
    bool remove_kernel_modules();
    bool stop_services();
    
    // Funciones auxiliares
    bool ask_confirmation(const std::string& message) const;
    bool exec_system_command(const std::string& cmd) const;
    bool file_exists(const std::string& path) const;
};

#endif // UNINSTALLER_H
