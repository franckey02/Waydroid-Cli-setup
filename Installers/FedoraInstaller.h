#ifndef FEDORA_INSTALLER_H
#define FEDORA_INSTALLER_H

#include "../src/SystemDetector.h"
#include <string>

class FedoraInstaller {
public:
    FedoraInstaller(const SystemInfo& sys_info);
    
    bool install(bool unattended = false);
    
private:
    SystemInfo system_info;
    
    bool is_immutable_variant() const;
    bool install_regular() const;
    bool install_immutable() const;
    bool initialize_waydroid(bool with_gapps) const;
    bool enable_service() const;
    bool configure_firewall() const;
    
    bool ask_gapps_confirmation() const;
};

#endif // FEDORA_INSTALLER_H
