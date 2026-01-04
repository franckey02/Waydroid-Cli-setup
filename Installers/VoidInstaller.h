#ifndef VOID_INSTALLER_H
#define VOID_INSTALLER_H

#include "../src/SystemDetector.h"
#include <string>

class VoidInstaller {
public:
    VoidInstaller(const SystemInfo& sys_info);
    
    bool install(bool unattended = false);
    
private:
    SystemInfo system_info;
    
    bool update_system() const;
    bool install_package() const;
    bool initialize_waydroid(bool with_gapps) const;
    bool enable_service() const;
    void show_void_readme() const;
    
    bool ask_gapps_confirmation() const;
};

#endif // VOID_INSTALLER_H
