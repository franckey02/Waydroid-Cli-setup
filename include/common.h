#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <map>

// Tipos de distribuciones soportadas
enum class Distro {
    UNKNOWN,
    ARCH,
    DEBIAN,
    UBUNTU,
    FEDORA,
    OPENSUSE,
    VOID,
    NIXOS,
    RHEL,
    CENTOS,
    MANJARO,
    POPOS,
    LINUXMINT
};

// Tipos de gestores de paquetes
enum class PackageManager {
    UNKNOWN,
    PACMAN,
    APT,
    DNF,
    ZYPPER,
    XBPS,
    NIX,
    YUM
};

// Estructura para información del sistema
struct SystemInfo {
    Distro distribution;
    std::string version;
    PackageManager package_manager;
    std::string architecture;
    std::string session_type;
    bool is_root;
    
    SystemInfo() : distribution(Distro::UNKNOWN), 
                   package_manager(PackageManager::UNKNOWN),
                   is_root(false) {}
};

// Constantes
const std::map<Distro, std::string> DISTRO_NAMES = {
    {Distro::ARCH, "Arch Linux"},
    {Distro::DEBIAN, "Debian"},
    {Distro::UBUNTU, "Ubuntu"},
    {Distro::FEDORA, "Fedora"},
    {Distro::OPENSUSE, "openSUSE"},
    {Distro::VOID, "Void Linux"},
    {Distro::NIXOS, "NixOS"},
    {Distro::RHEL, "RHEL"},
    {Distro::CENTOS, "CentOS"},
    {Distro::MANJARO, "Manjaro"},
    {Distro::POPOS, "Pop!_OS"},
    {Distro::LINUXMINT, "Linux Mint"}
};

// Funciones auxiliares
namespace utils {
    std::string exec_command(const std::string& cmd);
    bool file_exists(const std::string& path);
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    bool contains(const std::string& str, const std::string& substr);
}

#endif // COMMON_H
