#ifndef DEBIAN_INSTALLER_H
#define DEBIAN_INSTALLER_H

#include "../src/SystemDetector.h"
#include <string>

class DebianInstaller {
public:
    DebianInstaller(const SystemInfo& sys_info);
    
    bool install(bool unattended = false);
    
private:
    SystemInfo system_info;
    
    bool detect_codename(std::string& codename) const;
    bool update_system() const;
    bool install_prerequisites() const;
    bool add_repository(const std::string& codename) const;
    bool install_package() const;
    bool initialize_waydroid(bool with_gapps) const;
    bool enable_service() const;
    bool configure_firewall() const;
    
    bool ask_gapps_confirmation() const;
};

#endif // DEBIAN_INSTALLER_H
