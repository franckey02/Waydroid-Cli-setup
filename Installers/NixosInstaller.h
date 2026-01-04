#ifndef NIXOS_INSTALLER_H
#define NIXOS_INSTALLER_H

#include "../src/SystemDetector.h"
#include <string>

class NixOSInstaller {
public:
    NixOSInstaller(const SystemInfo& sys_info);
    
    bool install(bool unattended = false);
    
private:
    SystemInfo system_info;
    
    void show_installation_guide() const;
    void create_helper_script() const;
    bool save_guide_to_file() const;
    
    bool ask_save_guide() const;
};

#endif // NIXOS_INSTALLER_H
