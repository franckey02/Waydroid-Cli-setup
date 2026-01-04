#ifndef ARCH_INSTALLER_H
#define ARCH_INSTALLER_H

#include "../src/SystemDetector.h"
#include <string>

class ArchInstaller {
public:
    ArchInstaller(const SystemInfo& sys_info);
    
    bool install(bool unattended = false);
    
private:
    SystemInfo system_info;
    
    bool install_packages();
    bool initialize_waydroid(bool with_gapps);
    bool enable_service();
    bool check_aur_helper();
    bool install_with_aur(const std::string& helper);
    
    bool ask_gapps_confirmation() const;
};

#endif // ARCH_INSTALLER_H
