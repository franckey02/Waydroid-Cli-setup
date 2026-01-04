#ifndef OPENSUSE_INSTALLER_H
#define OPENSUSE_INSTALLER_H

#include "../src/SystemDetector.h"
#include <string>

class OpenSUSEInstaller {
public:
    OpenSUSEInstaller(const SystemInfo& sys_info);
    
    bool install(bool unattended = false);
    
private:
    SystemInfo system_info;
    
    std::string detect_version() const;
    bool update_system() const;
    bool add_repository(const std::string& version) const;
    bool configure_kernel() const;
    bool install_package() const;
    bool configure_apparmor() const;
    bool initialize_waydroid() const;
    bool enable_service() const;
    bool configure_firewall() const;
    
    void show_reboot_warning() const;
};

#endif // OPENSUSE_INSTALLER_H
