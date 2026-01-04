#ifndef SYSTEM_DETECTOR_H
#define SYSTEM_DETECTOR_H

#include "common.h"
#include <string>

class SystemDetector {
public:
    SystemDetector();
    
    // Detectar información del sistema
    SystemInfo detect();
    
    // Detectar distribución específica
    Distro detect_distribution();
    
    // Detectar gestor de paquetes
    PackageManager detect_package_manager();
    
    // Detectar arquitectura
    std::string detect_architecture();
    
    // Detectar sesión gráfica
    std::string detect_session();
    
    // Verificar si es usuario root
    bool check_root();
    
private:
    // Métodos auxiliares
    Distro detect_from_os_release();
    Distro detect_from_etc_release();
    PackageManager detect_pm_by_command();
    
    SystemInfo system_info;
};

#endif // SYSTEM_DETECTOR_H
